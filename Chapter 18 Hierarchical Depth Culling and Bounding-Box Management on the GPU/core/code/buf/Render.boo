namespace kri.buf

import OpenTK.Graphics.OpenGL


public class Render(Surface):
	private final	hardId	as uint
	private			ready	= false
	public			format	as RenderbufferStorage

	public def constructor():
		id = -1
		GL.GenRenderbuffers(1,id)
		hardId = id
	private def constructor(manId as uint):
		ready = true
		hardId = manId
	public static final	Zero	= Render(0)

	def destructor():
		return	if not hardId
		kri.Help.safeKill() do:
			tmp as int = hardId
			GL.DeleteRenderbuffers(1,tmp)
	
	public override def attachTo(fa as FramebufferAttachment) as void:
		init()	if not ready
		assert ready
		GL.FramebufferRenderbuffer( FramebufferTarget.Framebuffer, fa, RenderbufferTarget.Renderbuffer, hardId )
	public override def bind() as void:
		GL.BindRenderbuffer( RenderbufferTarget.Renderbuffer, hardId )
	
	public override def init() as void:
		bind()
		caps = kri.Ant.Inst.caps
		assert wid <= caps.renderBufSize
		assert het <= caps.renderBufSize
		ready = true
		if samples:
			assert samples <= caps.multiSamples
			GL.RenderbufferStorageMultisample( RenderbufferTarget.Renderbuffer, samples, format, wid, het )
		else:
			GL.RenderbufferStorage( RenderbufferTarget.Renderbuffer, format, wid, het )

	public override def syncBack() as void:
		bind()
		vals = (of int:0,0,0,0)
		pars = (
			RenderbufferParameterName.RenderbufferWidth,
			RenderbufferParameterName.RenderbufferHeight,
			RenderbufferParameterName.RenderbufferSamples,
			RenderbufferParameterName.RenderbufferInternalFormat)
		for i in range(vals.Length):
			GL.GetRenderbufferParameter( RenderbufferTarget.Renderbuffer, pars[i], vals[i] )
		wid,het,samples = vals[0:3]
		format = cast(RenderbufferStorage,vals[3])

