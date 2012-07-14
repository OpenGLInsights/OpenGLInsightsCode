namespace support.defer

import OpenTK.Graphics.OpenGL
import kri.shade

#---------	CONTEXT	--------#

public class Context:
	public final buf		= kri.buf.Holder( mask:7 )
	public final sphere		as kri.gen.Frame
	public final cone		as kri.gen.Frame
	public final dict		= par.Dict()
	public final texDepth	= par.UnitProxy() do():
		return buf.at.depth	as kri.buf.Texture
	public final sh_diff	= Object.Load('/mod/lambert_f')
	public final sh_spec	= Object.Load('/mod/phong_f')
	public final sh_apply	= Object.Load('/g/apply_f')
	
	public Diffuse	as kri.buf.Texture:
		get: return buf.at.color[0] as kri.buf.Texture
	public Specular	as kri.buf.Texture:
		get: return buf.at.color[1] as kri.buf.Texture
	public Bump		as kri.buf.Texture:
		get: return buf.at.color[2] as kri.buf.Texture
	
	public def constructor(qord as byte, ncone as byte):
		# light volumes
		sh = kri.gen.Sphere( qord,	OpenTK.Vector3.One )
		sphere	= kri.gen.Frame('sphere',sh)
		cn = kri.gen.Cone( ncone,	OpenTK.Vector3.One )
		cone	= kri.gen.Frame('cone',cn)
		# dictionary
		dict.unit('depth',texDepth)
		# diffuse, specular, world space normal
		pif = (PixelInternalFormat.Rgba8, PixelInternalFormat.Rgba8, PixelInternalFormat.Rgba12)
		for i in range(3):
			pt = par.Texture('g'+i)
			tex = kri.buf.Texture(0, pif[i], PixelFormat.Rgba )
			buf.at.color[i] = pt.Value = tex
			pt.Value.filt(false,false)
			dict.unit(pt)


#---------	Debug Layer render	--------#

public class BugLayer( kri.rend.Basic ):
	public	final fbo	as kri.buf.Holder
	public	layer		as int	= -1
	public def constructor(con as Context):
		fbo = con.buf
	public override def process(link as kri.rend.link.Basic) as void:
		if layer<0: return
		link.activate(false)
		fbo.mask = 1<<layer
		fbo.copyTo( link.Frame, ClearBufferMask.ColorBufferBit )
