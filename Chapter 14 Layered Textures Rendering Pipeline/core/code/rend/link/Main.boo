namespace kri.rend.link

import OpenTK.Graphics.OpenGL


public class Basic(Help):
	Input	as kri.buf.Texture:
		public virtual get:
			return null
	Depth	as kri.buf.Texture:
		public virtual get:
			return null
	Stencil	as kri.buf.Texture:
		public virtual get:
			return null
	Frame	as kri.buf.Frame:
		public virtual get:
			return null

	public enum Target:
		None
		Same
		New
	public abstract def activate(ct as Target, offset as single, toDepth as bool) as bool:
		pass
	public def activate(toNew as bool) as bool:
		ct = (Target.Same,Target.New)[toNew]
		return activate( ct, System.Single.NaN, true )

	public def blitTo(dest as kri.buf.Frame, what as ClearBufferMask) as bool:
		if Frame.getInfo():
			Frame.copyTo(dest,what)
			return true
		return false
	public virtual def blitTo(bas as Basic) as bool:
		return blitTo( bas.Frame, ClearBufferMask.ColorBufferBit )


public class Screen(Basic):
	public final screen	= kri.buf.Screen()
	Frame	as kri.buf.Frame:
		public override get:
			return screen
	public override def activate(ct as Target, offset as single, toDepth as bool) as bool:
		SetDepth(offset,toDepth)
		return screen.bind()
