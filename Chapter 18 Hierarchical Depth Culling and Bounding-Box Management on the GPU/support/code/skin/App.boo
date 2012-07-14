namespace support.skin

#---------	RENDER SKELETON SYNC		--------#

public class Update( kri.rend.tech.Basic ):
	private final va	= kri.vb.Array()
	private final tf	= kri.TransFeedback(1)
	private final bu1	= kri.shade.Bundle()
	private final bu2	= kri.shade.Bundle()
	private final par	= List[of kri.lib.par.spa.Shared](
		kri.lib.par.spa.Shared("bone[${i}]")
		for i in range(kri.Ant.Inst.caps.bones)
		).ToArray()
	public final at_all	as (int)

	public def constructor(dq as bool):
		super('skin')
		dict = kri.shade.par.Dict()
		for p as kri.meta.IBase in par:
			p.link(dict)
		# prepare quat-based shader
		sa = bu1.shader
		sa.add( '/lib/quat_v', '/skin/skin_v', '/skin/main_v' )
		sa.add( ('/skin/simple_v','/skin/dual_v')[dq] )
		#old: sa.add( '/skin/zcull_v', '/lib/tool_v', '/empty_f' )
		sa.add( '/skin/empty_v' )
		sa.feedback(true, 'to_vertex', 'to_quat')
		bu1.dicts.Add(dict)
		# prepare normal-based shader
		sa = bu2.shader
		sa.add( '/lib/quat_v', '/skin/skin_v', '/skin/normal_v' )
		sa.feedback(true, 'to_vertex', 'to_normal')
		bu2.dicts.Add(dict)
		# finish
		spat = kri.Spatial.Identity
		par[0].activate(spat)

	public override def process(con as kri.rend.link.Basic) as void:
		scene = kri.Scene.Current
		if not scene:	return
		buDic = { 'quat':bu1, 'normal':bu2 }
		for e in scene.entities:
			tag = e.seTag[of Tag]()
			if not e.visible or not tag or tag.Sync:
				continue
			# collect or create destination buffers
			bu as kri.shade.Bundle = null
			at_mod = ('vertex','')
			for bx in buDic:
				str = bx.Key as string
				if e.mesh.find(str):
					bu = bx.Value as kri.shade.Bundle
					at_mod[1] = str
					break
			if not (bu and e.mesh.find(at_mod[0])):
				str = (e as kri.INamed).Name
				kri.lib.Journal.Log("Skin: insufficient input data for '${str}'")
				e.tags.Remove(tag)
				continue
			# bind outputs
			vos = array[of kri.vb.Attrib]( at_mod.Length )
			for i in range( vos.Length ):
				vos[i] = e.store.find(at_mod[i])
				if vos[i]:	continue
				orig = e.mesh.find(at_mod[i])
				if not orig:	break
				ai = orig.Semant[0]
				vos[i] = v = kri.vb.Attrib()
				v.Semant.Add(ai)
				v.initUnit( e.mesh.nVert )
				e.store.buffers.Add(v)
			tf.Bind( *vos )
			# upload bone info
			spa as kri.Spatial
			for i in range( tag.skel.bones.Length ):
				b = tag.skel.bones[i]	# model->pose
				b.genTransPose( e.node.local, spa )
				s0 = s1 = b.World
				s0.combine(spa,s1)	# ->world
				s1 = e.node.World
				s1.inverse()
				spa.combine(s0,s1)	# ->model
				par[i+1].activate(spa)
			# run the transform
			kri.Ant.Inst.params.activate(e)
			using tf.discard():
				e.mesh.render(va,bu,tf)
			tag.Sync = true
