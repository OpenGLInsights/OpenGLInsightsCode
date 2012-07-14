namespace support.light.kbuf

import OpenTK.Graphics.OpenGL
import kri.shade
import kri.buf


#---------	LIGHT INIT	--------#

public class Init( kri.rend.Basic ):
	public	final fbo	= Holder( mask:3 )
	private	final bu	= kri.shade.Bundle()

	public def constructor(nlay as byte):
		# init buffer
		tt = TextureTarget.Texture2DMultisample
		fbo.at.stencil	= Texture.Stencil(nlay)
		fbo.at.color[0]	= Texture( target:tt, samples:nlay,
			intFormat:PixelInternalFormat.Rgb16f )	# R11fG11fB10f
		fbo.at.color[1]	= Texture( target:tt, samples:nlay,
			intFormat:PixelInternalFormat.Rgb10A2 )
		# init shader
		bu.shader.add('/copy_v','/white_f') # temp
	
	public override def setup(pl as kri.buf.Plane) as bool:
		fbo.resize( pl.wid, pl.het )
		return true
	
	public override def process(con as kri.rend.link.Basic) as void:
		# depth copy
		con.Multisample = false
		fbo.mask = 0
		con.blitTo( fbo, ClearBufferMask.DepthBufferBit )
		# stencil init
		GL.StencilMask(-1)
		sm = fbo.at.stencil.samples
		if 'RectangleFill':	
			assert sm > 0
			con.DepthTest = false
			con.ClearStencil(1)
			#sb = -1; GL.GetInteger( GetPName.SampleBuffers, sb )
			#sm = -1; GL.GetInteger( GetPName.Samples, sm )
			# todo: optimize to use less passes
			using kri.Section( EnableCap.SampleMask ), kri.Section( EnableCap.StencilTest ):
				GL.StencilFunc( StencilFunction.Always, 0,0 )
				GL.StencilOp( StencilOp.Incr, StencilOp.Incr, StencilOp.Incr )
				for i in range(1,sm):
					GL.SampleMask( 0, -1<<i )
					kri.Ant.Inst.quad.draw(bu)
		else:
			using kri.Section( EnableCap.SampleMask ):
				for i in range( sm ):
					GL.SampleMask(0,1<<i)
					con.ClearStencil(i+1)
		GL.SampleMask(0,-1)
		# color clear
		fbo.mask = 3
		fbo.bind()
		GL.ColorMask(true,true,true,true)
		con.ClearColor()
		if not 'DebugColor':
			debugLayer = 1
			fbo.mask = 2
			fbo.bind()
			using kri.Section( EnableCap.StencilTest ):
				GL.StencilFunc( StencilFunction.Equal, debugLayer,-1 )
				GL.StencilOp( StencilOp.Keep, StencilOp.Keep, StencilOp.Keep )
				kri.Ant.Inst.quad.draw(bu)
		con.Multisample = true


#---------	LIGHT PRE-PASS	--------#

public class Bake( kri.rend.Basic ):
	protected final bu		= Bundle()
	protected final bv		= Bundle()
	protected final context	as support.light.Context
	protected final sphere	as kri.Mesh
	private final fbo		as Holder
	private final texDep	= par.Texture('depth')
	private final va		= kri.vb.Array()
	private final static 	geoQuality	= 1
	private final static	pif = PixelInternalFormat.Rgba

	public def constructor(init as Init, lc as support.light.Context):
		fbo = init.fbo
		context = lc
		# baking shader
		sx = bu.shader
		sx.add( '/light/kbuf/bake_v', '/light/kbuf/bake_f', '/lib/defer_f' )
		sx.add( *kri.Ant.Inst.libShaders )
		sx.fragout('rez_dir','rez_color')
		d = par.Dict()
		d.unit(texDep)
		bu.dicts.AddRange((d,lc.dict))
		# create geometry
		sphere = kri.gen.Sphere( geoQuality, OpenTK.Vector3.One )
		# create white shader
		sx = bv.shader
		sx.add('/light/kbuf/bake_v','/empty_f')
		sx.add( *kri.Ant.Inst.libShaders )

	private def drawLights(mask as byte, bx as Bundle) as void:
		fbo.mask = mask
		fbo.bind()
		for l in kri.Scene.Current.lights:
			continue	if l.fov != 0f
			kri.Ant.Inst.params.activate(l)
			sphere.render(va,bx,null)
			#break	# !debug!

	public override def process(con as kri.rend.link.Basic) as void:
		#return	# !debug!
		con.Multisample = false
		texDep.Value = con.Depth
		con.SetDepth(0f,false)
		GL.CullFace( CullFaceMode.Front )
		GL.DepthFunc( DepthFunction.Gequal )
		#todo: use stencil for front faces
		using kri.Section( EnableCap.StencilTest ):
			# write color values
			GL.StencilFunc( StencilFunction.Equal, 1,-1 )
			GL.StencilOp( StencilOp.Keep, StencilOp.Keep, StencilOp.Keep )
			drawLights(3,bu)
			# shift stencil route
			GL.StencilFunc( StencilFunction.Always, 0,0 )
			GL.StencilOp( StencilOp.Keep, StencilOp.Keep, StencilOp.Decr )
			drawLights(0,bv)
		GL.CullFace( CullFaceMode.Back )
		GL.DepthFunc( DepthFunction.Lequal )
		con.Multisample = true


#---------	LIGHT APPLICATION	--------#

public class Apply( kri.rend.tech.Meta ):
	private final fbo	as kri.buf.Holder
	private final pDir	= par.Texture('dir')
	private final pCol	= par.Texture('color')
	# init
	public def constructor(init as Init):
		super('lit.kbuf', false, null,
			'bump','emissive','diffuse','specular','glossiness')
		fbo = init.fbo
		pDir.Value = fbo.at.color[0] as kri.buf.Texture
		pCol.Value = fbo.at.color[1] as kri.buf.Texture
		dict.unit( pDir, pCol )
		shade('/light/kbuf/apply')
	# work
	public override def process(con as kri.rend.link.Basic) as void:
		con.activate( con.Target.Same, 0f, false )
		con.ClearColor()
		drawScene()
