namespace kri.buf


public class Plane:
	public samples	as byte	= 0
	public wid		as uint	= 0
	public het		as uint	= 0
	# attributes
	public Aspect	as single:
		get: return wid*1f / het
	public Area		as uint:
		get: return wid * het
	public Size		as uint:
		get: return wid * System.Math.Max(het,cast(uint,1)) * System.Math.Max(samples,1)
	# methods
	public def isCompatible(pl as Plane) as bool:
		if samples!=pl.samples:
			# only MS resolution is possible
			return false	if samples*pl.samples
			return false	if wid!=pl.wid or het!=pl.het
		return true


public class Surface(Plane):
	public name		as string	= ''
	# attache to a framebuffer
	public abstract def attachTo(fa as OpenTK.Graphics.OpenGL.FramebufferAttachment) as void:
		pass
	# bind on its own
	public abstract def bind() as void:
		pass
	# allocate contents
	public abstract def init() as void:
		pass
	public def init(w as uint, h as uint) as void:
		if wid==w and het==h: return
		wid,het = w,h
		init()
	# retrieve GL state
	public abstract def syncBack() as void:
		pass
