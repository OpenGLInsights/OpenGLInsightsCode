namespace kri.rend.link

import OpenTK.Graphics.OpenGL
import kri.buf


public class Buffer(Basic):
	public	final	bitColor	as byte			# color storage
	public	final	bitDepth	as byte			# depth storage
	public	final	buf			= Holder()		# intermediate FBO
	public	final	texTarget	as TextureTarget
	public	final	nSamples	as byte
	
	public static final FmColor	= (of PixelInternalFormat:
		PixelInternalFormat.Rgba,		#0 - standard
		PixelInternalFormat.Rgba8,		#8
		PixelInternalFormat.Rgba16,		#16
		PixelInternalFormat.Rgba16f,	#24
		PixelInternalFormat.Rgba32f)	#32
	public static final FmDepth	= (of PixelInternalFormat:
		PixelInternalFormat.DepthComponent,		#0 - standard
		PixelInternalFormat.Depth24Stencil8,	#8
		PixelInternalFormat.DepthComponent16,	#16
		PixelInternalFormat.DepthComponent24,	#24
		PixelInternalFormat.DepthComponent32)	#32
	
	Input as Texture:
		public override get:
			return buf.at.color[0] as Texture
	Depth as Texture:
		public override get:
			st = buf.at.stencil as Texture
			return st	if st and st.pixFormat == PixelFormat.DepthStencil
			return buf.at.depth as Texture
		private set:
			if not value:
				buf.at.depth = buf.at.stencil = null
			elif value.pixFormat == PixelFormat.DepthStencil:
				buf.at.stencil = value
			else:
				buf.at.depth = value
	Stencil as Texture:
		public override get:
			return buf.at.stencil as Texture
	Frame as Frame:
		public override  get:
			return buf
	Ready as bool:
		public get:
			return buf.at.color[0] != null
	
	public def constructor(ns as byte, bc as byte, bd as byte):
		texTarget = (TextureTarget.Texture2D, TextureTarget.Texture2DMultisample)[ns>0]
		nSamples,bitColor,bitDepth = ns,bc,bd
		b = bc | bd
		assert b<=48 and not (b&0x7)
	
	public def makeSurface() as Texture:
		t = Texture( target:texTarget, samples:nSamples,
			intFormat:FmColor[bitColor>>3] )
		t.filt(false,false)
		return t
	
	public def resize(pl as Plane) as void:
		if not Input:
			buf.at.color[0] = makeSurface()
		buf.resize( pl.wid, pl.het )
	
	public override def activate(ct as Target, offset as single, toDepth as bool) as bool:
		if not Input:	return false
		# prepare depth
		if SetDepth(offset,toDepth) and not Depth:
			pf = (PixelFormat.DepthComponent,PixelFormat.DepthStencil)[bitDepth==8]
			Depth = td = Texture( target:texTarget, samples:nSamples,
				pixFormat:pf, intFormat:FmDepth[bitDepth>>3] )
			td.init( Input.wid, Input.het )
			td.filt( false, false )
			td.shadow(false)
		# prepare color
		if ct == Target.New:
			t = buf.at.color[1]
			buf.at.color[1] = Input
			if not t:
				t = makeSurface()
				t.init( Input.wid, Input.het )
			buf.at.color[0] = t
		# bind
		buf.mask = (0,1)[ ct!=Target.None ]
		return buf.bind()

	public override def blitTo(bas as Basic) as bool:
		buf.mask = 1
		return super(bas)
