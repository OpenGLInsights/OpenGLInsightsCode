namespace kri.gen

import OpenTK.Graphics
import OpenTK.Graphics.OpenGL


public static class Texture:
	public struct Key:
		public pos	as single
		public col	as Color4
	
	public final border = Color4.Black
	
	private def make() as kri.buf.Texture:
		tex = kri.buf.Texture()
		tex.setState(0,false,false)
		return tex
	
	public def ofColor(*data as (byte)) as kri.buf.Texture:
		tex = kri.buf.Texture( wid:1, het:1 )
		tex.setState(1,false,false)
		tex.init(data,false)
		return tex
	public def ofDepth(val as single) as kri.buf.Texture:
		tex = kri.buf.Texture( wid:1, het:1,
			intFormat:PixelInternalFormat.DepthComponent,
			pixFormat:PixelFormat.DepthComponent )
		tex.setState(1,false,false)
		tex.shadow(true)
		tex.init((val,),false)
		return tex

	
	public def ofCurve(data as (Key)) as kri.buf.Texture:
		assert data.Length
		mid = 1f
		for i in range( data.Length-1 ):
			mid = System.Math.Min(mid, data[i+1].pos - data[i].pos)
		assert mid>0f
		d2 = array[of Color4](1 + cast(int, 1f / mid))
		mid = 1f / (d2.Length-1)
		for i in range(d2.Length):
			t = i * mid
			j = 0
			while(j!=data.Length and data[j].pos<t):
				j += 1
			if j==0:
				rt = t / data[j].pos
				d2[i] = kri.load.ExAnim.InterColor(
					border, data[j].col, (rt,1f)[single.IsNaN(rt)])
			elif j==data.Length:
				d2[i] = kri.load.ExAnim.InterColor(
					data[j-1].col, border, 1f - (1f-t) / (1f-data[j-1].pos))
			else:
				d2[i] = kri.load.ExAnim.InterColor(
					data[j-1].col, data[j].col,
					(t-data[j-1].pos) / (data[j].pos - data[j-1].pos))
		tex = kri.buf.Texture( target:TextureTarget.Texture1D, wid:d2.Length )
		tex.setState(1,true,true)
		tex.init(d2,false)
		return tex

	
	public final color	= ofColor(0xFF,0xFF,0xFF,0xFF)
	public final normal	= ofColor(0x80,0x80,0xFF,0x80)
	public final depth	= ofDepth(1f)
	public final noise	= ofColor(0,0,0,0)

	# generate mip map levels by either down-sampling or up-sampling
	# note that it starts with a current level and finishes on the stopLevel
	public def createMipmap(fbo as kri.buf.Holder, stopLevel as byte, bu as kri.shade.Bundle) as void:
		if fbo.mask == 0:
			t = fbo.at.depth	as kri.buf.Texture
			kri.rend.link.Help.SetDepth(0f,true)
			GL.DepthFunc( DepthFunction.Always )
		else:
			t = fbo.at.color[0]	as kri.buf.Texture
			kri.rend.link.Help.DepthTest = false
		if not t:	return
		step = (-1,1)[stopLevel > t.level]
		while true:
			if t.wid+t.het<=2 and step>0:	break
			if t.level==stopLevel:			break
			t.setLevels()
			t.switchLevel( t.level + step )
			fbo.forceUpdate = true
			fbo.bind()
			kri.Ant.Inst.quad.draw(bu)
		t.setLevels(0,20)
		GL.DepthFunc( DepthFunction.Lequal )
		
	
	# transform blender 2D environmental map to a cube map
	public def toCube(fbo as kri.buf.Holder) as kri.buf.Texture:
		tin = fbo.at.color[0]
		if not tin:	return null
		t = kri.buf.Texture( target:TextureTarget.TextureCubeMap, tag:'transformed cube' )
		t.wid = tin.wid / 3
		t.het = tin.het / 2
		t.initCube()
		t.setState(0,false,false)
		fbo.mask = 2
		fbo.bind()
		fbo.mask = 1
		fbo.bindRead(true)
		sides = (of TextureTarget:
			TextureTarget.TextureCubeMapNegativeX, TextureTarget.TextureCubeMapNegativeZ, TextureTarget.TextureCubeMapPositiveX,
			TextureTarget.TextureCubeMapNegativeY, TextureTarget.TextureCubeMapPositiveY, TextureTarget.TextureCubeMapPositiveZ)
		for la in range(6):
			x = la % 3
			y = la / 3
			GL.FramebufferTexture2D( FramebufferTarget.DrawFramebuffer,
				FramebufferAttachment.ColorAttachment1, sides[la], t.handle, 0)
			GL.DrawBuffer( DrawBufferMode.ColorAttachment1 )
			GL.BlitFramebuffer(
				x*t.wid, y*t.het, (x+1)*t.wid, (y+1)*t.het,
				0,0, t.wid, t.het, ClearBufferMask.ColorBufferBit,
				BlitFramebufferFilter.Nearest )
		t.layer = -1
		fbo.markDirty()
		return t

	
	# lay out mip levels into a single layer
	public def flatMipmap(fi as kri.buf.Holder, fo as kri.buf.Holder, depth as bool) as void:
		t = (fi.at.color[0],fi.at.depth)[depth] as kri.buf.Texture
		if not t:	return
		t.switchLevel(0)
		fo.mask = fi.mask = (1,0)[depth]
		fi.bind()
		fo.bind()
		fi.bindRead(not depth)
		clearMask = (ClearBufferMask.ColorBufferBit,ClearBufferMask.DepthBufferBit)[depth]
		attachment = (FramebufferAttachment.ColorAttachment0,FramebufferAttachment.DepthAttachment)[depth]
		ofx = ofy = 0
		for i in range( 1, t.MipsNumber ):
			t.switchLevel(i)
			GL.FramebufferTexture( FramebufferTarget.ReadFramebuffer, attachment, t.handle, i )
			GL.BlitFramebuffer(	0,0, t.wid, t.het,
				ofx, ofy, ofx+t.wid, ofy+t.het,
				clearMask, BlitFramebufferFilter.Nearest )
			ofx += t.wid	# horisontal offset
		t.switchLevel(0)
