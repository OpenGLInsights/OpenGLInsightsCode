namespace support.skin

import kri.shade


public class Universal( kri.rend.tech.Basic ):
	private final	va		= kri.vb.Array()
	private final	tf		= kri.TransFeedback(1)
	private final	bu		= kri.shade.Bundle()
	private final	bones	= List[of kri.lib.par.spa.Shared](
		kri.lib.par.spa.Shared("bone[${i}]")
		for i in range(kri.Ant.Inst.caps.bones)
		).ToArray()
	
	private	final	maxBones	as byte	= 0
	private	final	pHas	= par.Value[of int]('has_data')	// vertex,normal,quat

	public def constructor():
		super('skin-uni')
		dict = kri.shade.par.Dict()
		for p as kri.meta.IBase in bones:
			p.link(dict)
		str = 'bones_number'
		pMax = par.Value[of int](str)
		dict.var(pHas,pMax)
		# prepare universal shader
		sa = bu.shader
		sa.add( '/lib/quat_v', '/skin/skin_v', '/skin/uni_v' )
		sa.feedback(true, 'to_vertex','to_normal','to_quat')
		bu.dicts.Add(dict)
		# extract number of bones
		bu.link()
		if not bu.LinkFail:
			loc = sa.getLocation(str)
			val = 0
			sa.getValue(loc,val)
			maxBones = val
		# finish
		spat = kri.Spatial.Identity
		bones[0].activate(spat)

	public override def process(con as kri.rend.link.Basic) as void:
		scene = kri.Scene.Current
		if not scene:	return
		for e in scene.entities:
			tag = e.seTag[of Tag]()
			if not e.visible or not tag or tag.Sync:
				continue
			# bind outputs
			pHas.Value = 7
			vDict = kri.vb.Dict( e.mesh )
			vos = (of kri.vb.Attrib: null,null,null)
			names = (of string: 'vertex','normal','quat')
			pHas.Value = vDict.fake(*names)
			isFirst = false
			for i in range( vos.Length ):
				vos[i] = e.store.find(names[i])
				if vos[i]:		continue
				orig = e.mesh.find(names[i])
				if not orig:	continue
				assert orig.Semant.Count==1
				isFirst = true
				ai = orig.Semant[0]
				vos[i] = v = kri.vb.Attrib()
				v.Semant.Add(ai)
				v.initUnit( e.mesh.nVert )
				e.store.buffers.Add(v)
			tf.Bind( *vos )
			# check limits
			nBone = tag.skel.bones.Length
			if isFirst and nBone >= maxBones:
				kri.lib.Journal.Log("Skin: too many bones (${nBone}/${maxBones})")
			# upload bone info
			spa as kri.Spatial
			for i in range(nBone):
				b = tag.skel.bones[i]	# model->bind_pose
				# assuming the model is parented to skeleton
				b.genTransPose( e.node.local, spa )
				s0 = s1 = b.World
				s0.combine(spa,s1)	# ->pose->world
				s1 = e.node.World
				s1.inverse()
				spa.combine(s0,s1)	# ->model
				bones[i+1].activate(spa)
			# run the transform
			kri.Ant.Inst.params.activate(e)
			using tf.discard():
				e.mesh.render(va,bu,vDict,1,tf)
			tag.Sync = true
