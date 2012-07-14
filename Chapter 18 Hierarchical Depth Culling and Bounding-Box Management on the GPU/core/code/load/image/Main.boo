namespace kri.load.image

import OpenTK.Graphics.OpenGL

#------		DUMMY GENERATOR		------#

public class Dummy( kri.data.IGenerator[of kri.buf.Texture] ):
	public tex	as kri.buf.Texture	= null
	public def generate() as kri.buf.Texture:	#imp: kri.res.IGenerator
		return tex


#------		BASIC RGBA IMAGE		------#

public class Basic( kri.data.IGenerator[of kri.buf.Texture] ):
	public final name	as string
	public final width	as uint
	public final height	as uint
	public final scan 	as (byte)
	public final bits	as byte
	public static	GammaCorrected	= false
	public static	Compressed		= true
	
	public def constructor(s as string, w as uint, h as uint, d as byte):
		name = s
		width,height = w,h
		scan = array[of byte](w*h*d)
		bits = d<<3
	
	public def constructor(s as string, w as uint, h as uint, ar as (byte), d as byte):
		name = s
		width,height = w,h
		scan = ar
		bits = d<<3
	
	private def getIntFormat() as PixelInternalFormat:
		if GammaCorrected and kri.Ant.Inst.gamma:
			if Compressed:	return PixelInternalFormat.CompressedSrgbAlpha
			else:			return PixelInternalFormat.SrgbAlpha
		else:
			if Compressed:	return PixelInternalFormat.CompressedRgba
			else:			return PixelInternalFormat.Rgba
	
	public def getFormat() as PixelFormat:
		pa = PixelFormat.Alpha
		return (pa,pa,pa, PixelFormat.Bgr, PixelFormat.Bgra)[bits>>3]
	
	public def generate() as kri.buf.Texture:	#imp: kri.res.IGenerator
		tex = kri.buf.Texture( wid:width, het:height, tag:name,
			name:name, pixFormat:getFormat(), intFormat:getIntFormat() )
		tex.init(scan,false)
		return tex
