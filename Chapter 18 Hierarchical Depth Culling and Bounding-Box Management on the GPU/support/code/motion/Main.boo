namespace support.motion

import System.Collections.Generic
import OpenTK.Graphics.OpenGL


public class Context:
	public final buf		= kri.buf.Holder()
	public final pRadius	= kri.shade.par.Value[of single]('radius')
	public def constructor():
		buf.at.color[0] = kri.buf.Texture(
			intFormat:PixelInternalFormat.Rg16f )


public class Bake( kri.rend.tech.Basic ):
	private final pu		= kri.shade.Bundle()
	private final pv		= kri.shade.Bundle()
	public	final va		= kri.vb.Array()
	private final buf		as kri.buf.Holder
	private final diModel	= Dictionary[of kri.Entity,kri.Spatial]()
	private final diCamera	= Dictionary[of kri.Camera,kri.Spatial]()
	private final pModel	= kri.lib.par.spa.Shared('s_old_mod')
	private final pCamera	= kri.lib.par.spa.Shared('s_old_cam')
	private final pOffset	= kri.lib.par.spa.Shared('s_offset')
	private final bones		= List[of kri.lib.par.spa.Shared]( kri.lib.par.spa.Shared("bone[${i}]")\
		for i in range(kri.Ant.Inst.caps.bones) ).ToArray()
	
	public def constructor(con as Context):
		super('motion')
		buf = con.buf
		# parameters
		d = kri.shade.par.Dict()
		for par as kri.meta.IBase in (pModel,pCamera,pOffset):
			par.link(d)
		for par as kri.meta.IBase in bones:
			par.link(d)
		# simple shader
		sa = pu.shader
		sa.add( '/lib/quat_v', '/lib/tool_v' )
		sa.add( '/motion/bake_v', '/motion/bake_f' )
		pu.dicts.Add(d)
		pu.link()
		# skin shader
		sa = pv.shader
		sa.add( '/lib/quat_v', '/lib/tool_v' )
		sa.add( '/skin/skin_v', '/skin/dual_v' )
		sa.add( '/motion/skin_v', '/motion/bake_f' )
		pv.dicts.Add(d)
		pv.link()
	
	public override def setup(pl as kri.buf.Plane) as bool:
		buf.at.stencil = null
		buf.resize( pl.wid, pl.het )
		return true

	public override def process(con as kri.rend.link.Basic) as void:
		#todo: check samples?
		buf.at.stencil = con.Depth
		assert con.Depth
		con.SetDepth(-1f,false)
		buf.mask = 1
		buf.bind()
		con.ClearColor()
		# set camera
		cam = kri.Camera.Current
		scene = kri.Scene.Current
		s_old = kri.Spatial.Identity
		if not scene:	return
		diCamera.TryGetValue(cam,s_old)
		pCamera.activate(s_old)
		s_old.inverse()
		s_new = kri.Node.SafeWorld(cam.node)
		diCamera[cam] = s_new
		sc = kri.Spatial.Combine(s_old,s_new)	# cam offset
		# iterate meshes
		for e in scene.entities:
			continue	if not e.Visible[cam]
			# find previous transform
			s_old = kri.Spatial.Identity
			diModel.TryGetValue(e,s_old)
			pModel.activate(s_old)
			s_new = kri.Node.SafeWorld( e.node )
			diModel[e] = s_new
			s_old.inverse()
			se = kri.Spatial.Combine(s_old,s_new)	# entity offset
			# final steps
			sc.inverse()
			sp = kri.Spatial.Combine(se,sc)
			pOffset.activate(sp)
			kri.Ant.Inst.params.spaModel.activate(s_new)
			#draw
			e.render(va,pu)


public class Apply( kri.rend.Filter ):
	public def constructor(con as Context):
		linear = true
		bu.shader.add( '/copy_v', '/filter/blur_vector_f', '/motion/apply_f' )
		pTex = kri.shade.par.Texture('velocity')
		pTex.Value = con.buf.at.color[0] as kri.buf.Texture
		dict.var(con.pRadius)
		dict.unit(pTex)
		bu.dicts.Add(dict)
		bu.link()
