namespace support.asm

import System.Collections.Generic
import OpenTK
import OpenTK.Graphics.OpenGL


public class Texture:
	public	final	data	= kri.buf.Texture(0, PixelInternalFormat.Rgba, PixelFormat.Rgba)
	public	final	cMap	= Dictionary[of kri.buf.Texture,Box2]()
	private	final	fbo		= kri.buf.Holder(mask:2)
	private	cx		as uint	= 0
	private	cy		as uint	= 0
	private maxy	as uint	= 0
	
	public def constructor(size as uint):
		fbo.at.color[1] = data
		data.setState(1,true,false)
		data.setBorder( Graphics.Color4.Black )
		data.init(size,size)
		clear()
	
	public def clear() as void:
		fbo.bind()
		kri.rend.link.Help.ClearColor()
		cMap.Clear()
		cx = cy = maxy = 0
	
	public def add(tex as kri.buf.Texture) as Box2:
		b = Box2()
		if cMap.TryGetValue(tex,b):
			return b
		if tex==null:
			kri.lib.Journal.Log('Asm: null texture added')
			return b
		elif tex.wid > data.wid:
			kri.lib.Journal.Log("Asm: texture is too X wide (${tex.handle})")
			return b
		elif cx+tex.wid > data.wid:
			cy += maxy+1
			cx = maxy = 0
		if cy+tex.het > data.het:
			kri.lib.Journal.Log("Asm: texture is too Y wide (${tex.handle})")
			return b
		fbo.at.color[0] = tex
		fbo.bind()
		fbo.bindRead(true)
		dx = cx + tex.wid
		dy = cy + tex.het
		GL.BlitFramebuffer(
			0,0, tex.wid,tex.het, cx,cy,dx,dy,
			ClearBufferMask.ColorBufferBit,
			BlitFramebufferFilter.Nearest )
		v = Vector2(1f / (data.wid-1), 1f / (data.het-1))
		b = Box2( Vector2.Multiply(v,Vector2(cx,cy)), Vector2.Multiply(v,Vector2(dx,dy)) )
		cMap.Add(tex,b)
		cx += tex.wid+1
		if tex.het>maxy:	maxy = tex.het
		return b
