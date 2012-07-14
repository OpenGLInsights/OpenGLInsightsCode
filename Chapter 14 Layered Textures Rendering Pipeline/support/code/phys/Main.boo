namespace support.phys

import System
import OpenTK.Graphics.OpenGL


public class Simulator( kri.ani.sim.Native ):
	public final pr	as	Core
	public def constructor( s as kri.Scene, ord as int, rz as kri.rend.EarlyZ ):
		super(s)
		pr = Core( ord, true, rz.name )
	protected override def onDelta(delta as double) as uint:
		super(delta)
		pr.tick(scene)
		return 0


#---------	RENDER PHYSICS		--------#

public class Core:
	private final fbo	= kri.buf.Holder(mask:1)
	private final cam	= kri.Camera()
	private final bu	= kri.shade.Bundle()
	private final bv	= kri.shade.Bundle()
	private final name	as string
	private final pbo	= kri.vb.Object()
	private final pId	= kri.shade.par.Value[of single]('object_id')
	private final isBig	as bool
	
	public Color	as kri.buf.Texture:
		get: return fbo.at.color[0] as kri.buf.Texture
	public Stencil	as kri.buf.Texture:
		get: return fbo.at.stencil	as kri.buf.Texture
	
	public def constructor( ord as byte, large as bool, techName as string ):
		isBig = large
		# init FBO
		pif = (PixelInternalFormat.Rg8, PixelInternalFormat.Rg16)[large]
		fbo.at.stencil = tSten		= kri.buf.Texture.Stencil(0)
		fbo.at.color[0] = tColor	= kri.buf.Texture( intFormat:pif )
		fbo.resize(1<<ord,1<<ord)
		# setup target parameters
		fbo.bind()
		for tex in (tSten,tColor):
			tex.filt(false,false)
			tex.genLevels()
		# 8 bit stencil + 2*[8,16] bit color
		pbo.init( (3,5)[large]<<(2*ord) )
		# init shader
		name = techName
		d = kri.shade.par.Dict()
		pSten	= kri.shade.par.Texture('sten')
		pColor	= kri.shade.par.Texture('color')
		pSten.Value	= tSten
		pColor.Value = tColor
		d.unit(pSten,pColor)
		d.var(pId)
		# create draw program
		bu.shader.add('/zcull_v','/physics_f')
		bu.shader.add( *kri.Ant.Inst.libShaders )
		bu.dicts.Add(d)
		# create down-sample program
		bv.shader.add('/copy_v','/filter/phys_max_f')
		bv.shader.fragout('to_sten','to_color')
		bv.dicts.Add(d)

	private def drawAll(scene as kri.Scene, bx as kri.shade.Bundle) as void:
		kid = 1f / ((1 << (8,16)[isBig]) - 1f)
		for i in range(scene.entities.Count):
			e = scene.entities[i]
			vac as kri.vb.Array = null
			if not (e.techVar.TryGetValue(name,vac) and vac):
				continue
			kri.vb.Array.Bind = vac
			pId.Value = (i+1.5f)*kid + 0.5f
			kri.Ant.Inst.params.activate(e)
			e.render(vac,bx)

	public def tick(s as kri.Scene) as void:
		# prepare the camera
		kri.Ant.Inst.params.activate( cam )
		kri.Ant.Inst.params.activate( s.cameras[0] )
		# prepare buffer
		fbo.bind()
		GL.DepthMask(true)
		GL.StencilMask(-1)
		GL.ClearColor(0f,0f,0f,1f)
		GL.ClearDepth(1f)
		GL.ClearStencil(0)
		GL.Clear(
			ClearBufferMask.ColorBufferBit |
			ClearBufferMask.DepthBufferBit |
			ClearBufferMask.StencilBufferBit )

		GL.Enable( EnableCap.DepthTest )
		GL.Disable( EnableCap.PolygonOffsetLine )
		GL.ColorMask(true,false,false,false)
		GL.DepthFunc( DepthFunction.Always )
		GL.PolygonMode( MaterialFace.FrontAndBack, PolygonMode.Line )
		drawAll(s,bu)
		GL.DepthMask(false)
		GL.ColorMask(false,true,false,false)
		GL.DepthFunc( DepthFunction.Lequal )
		GL.PolygonMode( MaterialFace.FrontAndBack, PolygonMode.Fill )
		GL.Enable( EnableCap.PolygonOffsetFill )
		GL.StencilFunc( StencilFunction.Always, 0,0 )
		using kri.Section( EnableCap.StencilTest ):
			GL.PolygonOffset(1f,1f)
			GL.CullFace( CullFaceMode.Back )
			GL.StencilOp( StencilOp.Keep, StencilOp.Keep, StencilOp.Incr )
			drawAll(s,bu)
			GL.PolygonOffset(-1f,-1f)
			GL.CullFace( CullFaceMode.Front )
			GL.StencilOp( StencilOp.Keep, StencilOp.Keep, StencilOp.Decr )
			drawAll(s,bu)
			GL.CullFace( CullFaceMode.Back )

		# resize the map
		GL.Disable( EnableCap.PolygonOffsetFill )
		GL.ColorMask(true,true,true,true)
		
		GL.Disable( EnableCap.DepthTest )
		tc = fbo.at.color[0]	as kri.buf.Texture
		ts = fbo.at.stencil		as kri.buf.Texture
		for i in range(3):
			for tex in (tc,ts):
				tex.setLevels(i,i+1)
			GL.FramebufferTexture2D( FramebufferTarget.DrawFramebuffer,
				FramebufferAttachment.ColorAttachment0, tc.target, tc.handle, i+1)
			GL.FramebufferTexture2D( FramebufferTarget.DrawFramebuffer,
				FramebufferAttachment.ColorAttachment1, ts.target, ts.handle, i+1)
			
		# read back result
		pl = fbo.at.color[0]
		size = pl.Size
		rect = Drawing.Rectangle( 0, 0, pl.wid, pl.het )
		fbo.readRaw[of byte]( PixelFormat.StencilIndex, rect, pbo, IntPtr.Zero )
		fbo.readRaw[of byte]( PixelFormat.Rg, rect, pbo, IntPtr(size) )
		# debug: extract result
		es = (1,2)[isBig]
		dar = array[of byte]( size*(1+2*es) )
		for i in range(dar.Length):
			dar[i] = 123
		pbo.read(dar,0)
		# readpixels don't work on 10.6
		for i in range(size):
			cs = dar[i]
			ca = dar[size + (i*2+0)*es]
			cb = dar[size + (i*2+1)*es]
			continue	if not cs or ca==cb
			cs = ca = cb
