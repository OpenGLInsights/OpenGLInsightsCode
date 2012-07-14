namespace kri.rend.link

import OpenTK.Graphics.OpenGL


public class Help:
	state DepthTest
	state Multisample
	state TextureCubeMapSeamless
	
	public static def Init() as void:
		GL.Enable( EnableCap.CullFace )
		GL.Disable( EnableCap.VertexProgramPointSize )
		GL.CullFace( CullFaceMode.Back )
		GL.ClearDepth(1.0)
		GL.DepthRange(0.0,1.0)
		GL.DepthFunc( DepthFunction.Lequal )
	
	public static def SetDepth(offset as single, write as bool) as bool:
		DepthTest = on = (not System.Single.IsNaN(offset))
		# set polygon offset
		return false	if not on
		GL.DepthMask(write)
		cap = EnableCap.PolygonOffsetFill
		if offset:
			GL.PolygonOffset(offset,offset)
			GL.Enable(cap)
		else:	GL.Disable(cap)
		return true
	
	public static def ClearDepth(val as double) as void:
		GL.ClearDepth(val)
		GL.Clear( ClearBufferMask.DepthBufferBit )
	public static def ClearStencil(val as int) as void:
		GL.ClearStencil(val)
		GL.Clear( ClearBufferMask.StencilBufferBit )
	public static def ClearColor(val as OpenTK.Graphics.Color4) as void:
		GL.ClearColor(val)
		GL.Clear( ClearBufferMask.ColorBufferBit )
	public static def ClearColor() as void:
		ClearColor( OpenTK.Graphics.Color4.Black )
