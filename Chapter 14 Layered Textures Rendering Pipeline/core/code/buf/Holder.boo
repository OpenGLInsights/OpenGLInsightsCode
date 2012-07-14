namespace kri.buf

import System
import System.Collections.Generic
import OpenTK.Graphics.OpenGL


public struct Container:
	public stencil	as Surface
	public depth	as Surface
	public color	as (Surface)
	public All		as Surface*:
		get: return (s	for s in ((stencil,depth) + color)	if s)
	public def constructor(num as byte):
		color = array[of Surface](num)
	public def clear() as void:
		stencil = depth = null
		for i in range(color.Length):
			color[i] = null


public class Holder(Frame):
	private	old		= Container(4)
	public	at		= Container(4)
	public	forceUpdate	= false
	private oldMask	= -1
	public	mask	= 0
	
	public def dropMask() as void:
		oldMask = -1
	
	private def addSurface(fa as FramebufferAttachment, ref cur as Surface, nex as Surface) as void:
		if cur==nex and not (forceUpdate and cur):	return
		cur = nex
		(Render.Zero,nex)[nex!=null].attachTo(fa)
	
	public override def getInfo() as Plane:
		sm = -1
		pl = Plane( samples:0, wid:1<<30, het:1<<30 )
		for sf in at.All:
			pl.wid	= Math.Min( pl.wid,	sf.wid )
			pl.het	= Math.Min( pl.het,	sf.het )
			if sm<0:
				sm = pl.samples = sf.samples
			elif sm!=sf.samples:
				kri.lib.Journal.Log("FBO: inconsistent attachments (${handle})")
				return null
		return pl
	
	public def setMask(m as byte) as void:
		if (mask=m) == oldMask:	return
		oldMask = mask
		if not mask:
			GL.DrawBuffer( DrawBufferMode.None )
			return
		drawList = List[of DrawBuffersEnum](
			DrawBuffersEnum.ColorAttachment0+i
			for i in range(4)	if (mask>>i)&1)
		GL.DrawBuffers( drawList.Count, drawList.ToArray() )
	
	public def updateSurfaces() as void:
		if 'ds':
			addSurface( FramebufferAttachment.DepthStencilAttachment,	old.stencil,	at.stencil )
			addSurface( FramebufferAttachment.DepthAttachment,			old.depth,		at.depth )
		for i in range( old.color.Length ):
			surface = old.color[i]	# Boo bug workaround
			addSurface( FramebufferAttachment.ColorAttachment0+i,		surface,		at.color[i] )
			old.color[i] = surface
		forceUpdate = false
	
	public override def bind() as bool:
		if super():
			updateSurfaces()
			setMask(mask)
			return checkStatus()
		return false
	
	private override def getReadMode() as ReadBufferMode:
		if not mask:	return ReadBufferMode.None
		i = 0
		while (mask>>i)&1 == 0: i+=1
		return ReadBufferMode.ColorAttachment0+i

	public def resize(wid as uint, het as uint) as void:
		for sf in at.All:
			sf.init(wid,het)
	
	public def markDirty() as void:
		old.clear()