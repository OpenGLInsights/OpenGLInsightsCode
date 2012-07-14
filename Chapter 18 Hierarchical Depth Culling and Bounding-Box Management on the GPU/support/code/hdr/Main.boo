namespace support.hdr

import System

public class Context:
	public avg	as single	= 1f
	

public class Render( kri.rend.Basic ):
	private final fbo	= kri.buf.Holder(mask:1)
	private final b2	= kri.buf.Holder(mask:1)
	private final pbo	= kri.vb.Object()
	private final context	as Context
	private final bu_bright	= kri.shade.Bundle()
	private final bu_scale	= kri.shade.Bundle()
	private final bu_tone	= kri.shade.Bundle()
	private final tInput	= kri.shade.par.Texture('input')
	private final pExpo		= kri.shade.par.Value[of single]('exposure')
	
	public def constructor(ctx as Context):
		context = ctx
		pExpo.Value = 1f
		d = kri.shade.par.Dict()
		d.unit(tInput)
		d.var(pExpo)
		for i in range(3):
			bu = (bu_bright,bu_scale,bu_tone)[i]
			name = ('/hdr/bright_f','/copy_f','/hdr/tone_f')[i]
			bu.shader.add('/copy_v',name)
			bu.dicts.Add(d)
	
	public virtual def setup(pl as kri.buf.Plane) as bool:
		fbo.resize( pl.wid, pl.het )
		pbo.init( sizeof(single) )
		return true
	
	public virtual def process(con as kri.rend.link.Basic) as void:
		# update context
		data = (of single:-1f,)
		pbo.read(data,0)
		context.avg = data[0]
		# init texture
		assert not con.Input
		con.DepthTest = false
		tInput.Value = t = con.Input
		fbo.at.color[0] = b2.at.color[0] = t
		t.filt(true,false)
		# bright filter
		t.switchLevel(0)
		kri.gen.Texture.createMipmap(b2,1,bu_bright)
		# down-sample
		kri.gen.Texture.createMipmap(b2,0,bu_scale)
		# read back
		pbo.bind()
		#b2.read(PixelFormat.Alpha, PixelType.Float)
		# up-sample
		using blend = kri.Blender():
			blend.Alpha = 0.1f
			blend.skipAlpha()
			kri.gen.Texture.createMipmap(b2,0,bu_scale)
		# bring to screen
		t.filt(false,false)
		con.activate(true)
		kri.Ant.Inst.quad.draw(bu_tone)
