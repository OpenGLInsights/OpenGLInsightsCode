namespace support.corp

import System
import OpenTK
import OpenTK.Graphics.OpenGL


public struct SetBake:
	public pixels	as uint
	public ratio	as single
	public piPos	as PixelInternalFormat
	public piRot	as PixelInternalFormat
	public filt		as bool
	
	public def constructor(order as byte, smooth as bool):
		pixels = 1<<order
		ratio = 1f
		piPos = PixelInternalFormat.Rgba32f
		piRot = PixelInternalFormat.Rgba32f
		filt = smooth
	
	public def tag() as support.bake.surf.Tag:
		assert ratio > 0f
		side = Math.Sqrt(pixels / ratio)
		wid,het = cast(int,side*ratio),cast(int,side)
		return support.bake.surf.Tag(wid,het,(piPos,piRot),filt)


public class Extra( kri.IExtension ):
	public final con	= kri.part.Context()
	public bake			= SetBake(16,false)
	public bLoop		= false
	
	def kri.IExtension.attach(nt as kri.load.Native) as void:
		# particles
		nt.readers['part']		= f_part
		nt.readers['p_dist']	= fp_dist
		nt.readers['p_life']	= fp_life
		nt.readers['p_hair']	= fp_hair
		nt.readers['p_vel']		= fp_vel
		nt.readers['p_rot']		= fp_rot
		nt.readers['p_phys']	= fp_phys
		nt.readers['p_child']	= fp_child
		nt.readers['pr_inst']	= fpr_inst

	public def finish(pe as kri.part.Emitter, scene as kri.Scene) as void:
		pm = pe.owner
		if not pm.Ready:
			ps = pm.seBeh[of beh.Standard]()
			ph = pm.seBeh[of support.hair.Behavior]()
			if ps:
				pm.behos.Add( beh.Sys() )
				con.makePart( pm.col_init, pm.col_update )
				born = (con.sh_born_time, con.sh_born_loop)[ bLoop ]
				pm.col_update.extra.Add(born)
			elif ph:
				con.makeHair( pm.col_init, pm.col_update )
			else: return
			pm.init(con)
			if ph:
				scene.particles.Remove(pe)
				lays = ph.genLayers(pe)
				scene.particles.AddRange(lays)
		if not pe.Ready:
			pe.allocate()
	
	public def finish2(ent as kri.Entity, scene as kri.Scene) as void:
		pe = ent.seTag[of Emitter]()
		if not pe:	return
		pm = pe.owner
		if not pm.Ready:
			ps = pm.seBeh[of beh.Standard]()
			ph = pm.seBeh[of support.hair.Behavior]()
			if ps:
				pm.behos.Add( beh.Sys() )
				con.makePart( pm.col_init, pm.col_update )
				born = (con.sh_born_time, con.sh_born_loop)[ bLoop ]
				pm.col_update.extra.Add(born)
			elif ph:
				con.makeHair( pm.col_init, pm.col_update )
			else: return
			pm.init(con)
			#if ph:
			#	scene.particles.Remove(pe)
			#	lays = ph.genLayers(pe)
			#	scene.particles.AddRange(lays)
		pm.insureMesh( ent.mesh, pe.entries )
		
	
	private def upNode(e as kri.Entity):
		assert e
		kri.Ant.Inst.params.activate(e)
		return true


	#---	Parse emitter object	---#
	public def f_part(r as kri.load.Reader) as bool:
		pm = Manager( r.bin.ReadUInt32() )
		r.puData(pm)
		# create emitter
		obj = r.geData[of kri.Entity]()
		pe = Emitter( obj, pm, r.getString() )
		r.puData(pe)
		ent = kri.Entity()
		ent.tags.Add(pe)
		r.at.scene.entities.Add(ent)
		# link to material
		tm = kri.TagMat( off:0, num:pm.Total )
		ent.tags.Add(tm)
		tm.mat = r.getMaterial()
		# post-process
		r.addPostProcess() do(n as kri.Node):
			finish2( ent, r.at.scene )
		return true


	#---	Parse distribution		---#
	public def fp_dist(r as kri.load.Reader) as bool:
		source = r.getString()
		r.getString()	# type
		r.getReal()		# jitter factor
		pe = r.geData[of Emitter]()
		if not pe:	return false
		pm = pe.owner
		ent = pe.entity
		
		ph = pm.seBeh[of support.hair.Behavior]()
		edgeTag = ent.seTag[of support.bake.edge.Tag]()
		faceTag = ent.seTag[of support.bake.surf.Tag]()
		if source=='EDGE' and not edgeTag:
			ent.tags.Add( edgeTag = support.bake.edge.Tag() )
		if source=='FACE' and not faceTag:
			st = bake
			if ph:
				st.pixels = pm.Total
			ent.tags.Add( faceTag = st.tag() )
		if ph:
			assert source in ('VERT','EDGE','FACE')
			return true

		sh as kri.shade.Object	= null
		if source == '':
			sh = con.sh_surf_node
			pe.onUpdate = upNode
		elif source == 'VERT':
			for i in range(2):
				at = ('vertex','quat')[i]
				t = kri.shade.par.Value[of kri.buf.Texture](at)
				pm.dict.unit(t)
				t.Value = kri.buf.Texture()	
				t.Value.init( SizedInternalFormat.Rgba32f, ent.findAny(at) )
				pe.onUpdate = upNode
			parNumber = kri.shade.par.Value[of single]('num_vertices')
			parNumber.Value = 1f * ent.mesh.nVert
			pm.dict.var(parNumber)
			sh = con.sh_surf_vertex
		elif source == 'EDGE':
			t = kri.shade.par.Value[of kri.buf.Texture]('edge')
			pm.dict.unit(t)
			t.Value = kri.buf.Texture()
			t.Value.init( SizedInternalFormat.Rgba32f, edgeTag.data )
			sh = con.sh_surf_edge
		elif source == 'FACE':
			tVert = kri.shade.par.Value[of kri.buf.Texture]('vertex')
			tQuat = kri.shade.par.Value[of kri.buf.Texture]('quat')
			pm.dict.unit(tVert,tQuat)
			pe.onUpdate = def(e as kri.Entity):
				upNode(e)
				tag = e.seTag[of support.bake.surf.Tag]()
				if not (tag and tag.stamp>=0.0 and tag.Vert and tag.Quat):
					return false
				tVert.Value = tag.Vert
				tQuat.Value = tag.Quat
				return true
			sh = con.sh_surf_face
		else: assert not 'supported :('
		pm.col_update.extra.Add(sh)
		return true


	#---	Parse life data	(emitter)	---#
	public def fp_life(r as kri.load.Reader) as bool:
		pm = r.geData[of Manager]()
		if not pm:	return false
		bh = beh.Standard(con)
		pm.behos.Add( bh )
		data = r.getVec4()	# start,end, life time, random
		bh.parLife.Value = Vector4( data.Z, data.W, data.Y-data.X, 1f )
		return true
	
	#---	Parse hair dynamics data	---#
	public def fp_hair(r as kri.load.Reader) as bool:
		pm = r.geData[of Manager]()
		if not pm:	return false
		segs = r.getByte()
		pm.behos.Add( support.hair.Behavior(con,segs) )
		dyn = r.getVector()	# stiffness, mass, bending
		damp = r.getVec2()	# spring, air
		pm.behos.Insert(0, beh.Stiff( dyn.X ))
		pm.behos.Insert(1, beh.Damp( damp.X ))
		# standard behavior on this line
		pm.behos.Add( beh.Norm() )	# should be the last
		return true
	
	#---	Parse velocity setup		---#
	public def fp_vel(r as kri.load.Reader) as bool:
		pe = r.geData[of Emitter]()
		if not (pe and pe.owner):	return false
		objFactor	= r.getVector()	# object-aligned factor
		tanFactor	= r.getVector()	# normal, tangent, tan-phase
		add			= r.getVec2()	# object speed, random
		tan	= Vector3( tanFactor.Y, 0f, tanFactor.X )
		# get behavior
		ps = pe.owner.seBeh[of beh.Standard]()
		ph = pe.owner.seBeh[of support.hair.Behavior]()
		if ps:		# standard
			ps.parVelObj.Value = Vector4( objFactor, add.Y )
			ps.parVelTan.Value = Vector4( tan, tanFactor.Z )
			ps.parVelKeep.Value = Vector4.Zero
		elif ph:	# hair
			magic = 5f / ph.layers	# todo: find out Blender scale source
			ph.layParam = magic * Vector4(tan,add.Y)
			avgLen = tan.LengthFast + objFactor.LengthFast + 0.001f
			ph.pSystem.Value.X = ph.layers / avgLen
		else:
			kri.lib.Journal.Log("Particle loader: no behavior found for '${pe.name}'")
			return false
		return true
	
	public def fp_rot(r as kri.load.Reader) as bool:
		mode = r.getString()
		factor = r.getReal()
		if mode == 'SPIN':
			pm = r.geData[of Manager]()
			if not pm:	return false
			pm.behos.Add( beh.Rotate(factor) )
		return true
	
	public def fp_phys(r as kri.load.Reader) as bool:
		pm = r.geData[of Manager]()
		if not pm:	return false
		pg = r.at.scene.pGravity
		if pg:
			bgav = beh.Gravity(pg)
			pm.behos.Add(bgav)
		biz = beh.Physics()
		biz.pSize.Value = Vector4( r.getVec2() )
		# forces: brownian, drag, damp
		biz.pForce.Value = Vector4( r.getVector() )
		pm.behos.Add(biz)
		return true
	
	public def fp_child(r as kri.load.Reader) as bool:
		mat = r.geData[of kri.Material]()
		if not mat:	return false
		meta = child.Meta( Name:'child' )
		mat.metaList.Add(meta)
		meta.num	= r.bin.ReadUInt16()
		# X=radius, Y=roundness, Z=size, W=random
		meta.Data	= r.getVec4()
		return true
	
	public def fpr_inst(r as kri.load.Reader) as bool:
		mat = r.geData[of kri.Material]()
		if not mat:	return false
		meta = inst.Meta( Name:'inst' )
		mat.metaList.Add(meta)
		r.addResolve() do(n as kri.Node):
			meta.ent = r.at.scene.entities.Find({e| return e.node==n })
		return true
