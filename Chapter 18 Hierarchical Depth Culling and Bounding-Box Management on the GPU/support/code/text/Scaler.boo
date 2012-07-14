namespace support.text

import System.Drawing

public class Scaler:
	public	final	sizeBig		= 1024
	public	final	sizeSmall	= 64
	public	final	spread		= 0.1
	public	final	bitmap		= Bitmap( sizeBig, sizeBig, Imaging.PixelFormat.Alpha )
	public	final	graphics	= Graphics.FromImage(bitmap)
	public	font 	= Font( FontFamily.GenericMonospace, 10 )
	public	brush	= SolidBrush( Color.White )
	
	public def constructor():
		pass
		
	public def create(text as string) as kri.buf.Texture:
		graphics.Clear( Color.Black )
		graphics.DrawString( text, font, brush, 0, 0 )
		return null
