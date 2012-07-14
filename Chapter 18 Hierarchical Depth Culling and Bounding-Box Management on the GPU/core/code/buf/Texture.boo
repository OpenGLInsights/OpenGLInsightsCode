namespace kri.buf

import System
import OpenTK
import OpenTK.Graphics.OpenGL


public class Texture(Surface):
	public	final	handle	as uint	= 0
	[Getter(Description)]
	public	tag		as string = ''
	private static	bound	as uint	= 0
	private	final	allocated	= array[of bool](16)
	private			filtered	= false
	private			mipmapped	= false
	private			fullWidth	as uint	= 0
	private			fullHeight	as uint	= 0
	public			target		= TextureTarget.Texture2D
	public			intFormat	= PixelInternalFormat.Rgba
	public			pixFormat	= PixelFormat.Rgba
	public			dep		as uint	= 0		# depth (z-dimension)
	public			level	as byte = 0		# active mip level
	public 			layer	as int	= -1	# active layer for Cube/Array textures
	
	public	MipMapped	as bool:
		get: return mipmapped
	public	MipsNumber	as byte:
		get: return Array.IndexOf(allocated,false)
	private Allocated	as bool:
		get: return allocated[level]
	public	CanSample	as bool:
		get: return allocated[0] and filtered

	public def constructor():
		handle = GL.GenTexture()
		for i in range(allocated.Length):
			allocated[i] = false
	
	private def constructor(manId as uint):
		handle = manId
		allocated[0] = filtered = true

	public def constructor(sm as byte, ifm as PixelInternalFormat, pf as PixelFormat):
		self()
		target = (TextureTarget.Texture2D,TextureTarget.Texture2DMultisample)[sm>0]
		samples = sm
		intFormat = ifm
		pixFormat = pf
	
	public static def Color(sm as byte) as Texture:
		return Texture( sm, PixelInternalFormat.Rgba,			PixelFormat.Rgba )
	public static def Depth(sm as byte) as Texture:
		return Texture( sm, PixelInternalFormat.DepthComponent,	PixelFormat.DepthComponent )
	public static def Stencil(sm as byte) as Texture:
		return Texture( sm, PixelInternalFormat.DepthStencil,	PixelFormat.DepthStencil )

	public static final	Zero	= Texture(0)
	def destructor():
		if not handle:
			return
		if handle==bound:
			ResetCache()
		kri.Help.safeKill() do():
			GL.DeleteTexture(handle)
	
	public static def Slot(tun as byte) as void:
		GL.ActiveTexture( TextureUnit.Texture0 + tun )
	public static def ResetCache() as void:
		bound = 0
		
	# virtual routines

	public override def attachTo(fa as FramebufferAttachment) as void:
		if not Allocated:	init()
		assert CanSample	# some GL implementations require filtering to be set as well
		if layer>=0:
			GL.FramebufferTextureLayer(	FramebufferTarget.Framebuffer, fa, handle, level, layer)
		else:	# let GL decide
			GL.FramebufferTexture(		FramebufferTarget.Framebuffer, fa, handle, level )

	public override def bind() as void:
		# can't use cache as we have different bounds
		# per texture slots and targets
		#return	if boundId == hardId
		bound = handle
		GL.BindTexture( target, handle )
	
	public override def syncBack() as void:
		bind()
		vals = (of int:0,0,0,0)
		pars = (
			GetTextureParameter.TextureWidth,
			GetTextureParameter.TextureHeight,
			GetTextureParameter.TextureSamples,
			GetTextureParameter.TextureInternalFormat)
		for i in range(vals.Length):
			GL.GetTexParameterI( target, pars[i], vals[i] )
		wid,het,samples = vals[0:3]
		intFormat = cast(PixelInternalFormat,vals[3])

	# initialization routines
	
	public def init(sif as SizedInternalFormat, buf as kri.vb.Object) as void:
		assert not level
		target = TextureTarget.TextureBuffer
		bind()
		allocated[level] = filtered = true
		GL.TexBuffer( TextureBufferTarget.TextureBuffer, sif, buf.handle )
		#syncBack()	# produces InvalidEnum when asking for width
	
	public override def init() as void:
		if target == TextureTarget.TextureCubeMap:
			initCube()
		elif samples:
			initMulti(level>0)
		elif pixFormat == PixelFormat.DepthStencil:
			init[of double](null,false)
		else:
			init[of byte](null,false)
	
	public def initMulti(fixedLoc as bool) as void:
		bind()
		caps = kri.Ant.Inst.caps
		fullWidth,fullHeight = wid,het
		assert samples and samples <= caps.multiSamples
		assert wid <= caps.textureSize
		assert het <= caps.textureSize
		assert dep <= caps.textureSize
		tams = cast( TextureTargetMultisample, cast(int,target) )
		allocated[level] = true
		if dep:
			assert target == TextureTarget.Texture2DMultisampleArray	
			GL.TexImage3DMultisample( tams, samples, intFormat, wid, het, dep, fixedLoc )
		else:
			assert target == TextureTarget.Texture2DMultisample
			GL.TexImage2DMultisample( tams, samples, intFormat, wid, het, fixedLoc )
	
	public static def GetPixelType(t as Type) as PixelType:
		return PixelType.UnsignedByte	if t==byte
		return PixelType.UnsignedShort	if t==ushort
		return PixelType.UnsignedInt	if t==uint
		return PixelType.HalfFloat		if t in (Vector2h,Vector3h,Vector4h)
		return PixelType.Float			if t in (single,Vector2,Vector3,Vector4,Graphics.Color4)
		return PixelType.UnsignedInt248	if t==double	# depth_stencil
		assert not 'good type'
		return PixelType.Bitmap

	private def setImage[of T(struct)](tg as TextureTarget, data as (T), compressed as bool) as void:
		# update stats
		if level: mipmapped = true
		else: fullWidth,fullHeight = wid,het
		allocated[level] = true
		# check
		assert not samples
		caps = kri.Ant.Inst.caps
		assert wid <= caps.textureSize
		assert het <= caps.textureSize
		assert dep <= caps.textureSize
		# upload
		if compressed:
			if dep:		GL.CompressedTexImage3D( tg, level, intFormat, wid, het, dep, 	0, data.Length, data )
			elif het:	GL.CompressedTexImage2D( tg, level, intFormat, wid, het,		0, data.Length, data )
			else:		GL.CompressedTexImage1D( tg, level, intFormat, wid,				0, data.Length, data )
		else:
			pt = GetPixelType(T)
			if dep:		GL.TexImage3D( tg, level, intFormat, wid, het, dep,	0, pixFormat, pt, data )
			elif het:	GL.TexImage2D( tg, level, intFormat, wid, het, 		0, pixFormat, pt, data )
			else:		GL.TexImage1D( tg, level, intFormat, wid, 	 		0, pixFormat, pt, data )
	
	public def init[of T(struct)](data as (T), compressed as bool) as void:
		bind()
		setImage(target,data,compressed)
	
	public def initCube[of T(struct)](side as int, data as (T), compressed as bool) as void:
		bind()
		tArray = (
			TextureTarget.TextureCubeMapNegativeX, TextureTarget.TextureCubeMapNegativeY, TextureTarget.TextureCubeMapNegativeZ,
			target,	# dummy corresponding side==0
			TextureTarget.TextureCubeMapPositiveX, TextureTarget.TextureCubeMapPositiveY, TextureTarget.TextureCubeMapPositiveZ)
		assert side>=-3 and side<=3
		setImage( tArray[side+3], data, compressed )
	
	public def initCube() as void:
		bind()
		for t in (
			TextureTarget.TextureCubeMapNegativeX,	TextureTarget.TextureCubeMapPositiveX,
			TextureTarget.TextureCubeMapNegativeY,	TextureTarget.TextureCubeMapPositiveY,
			TextureTarget.TextureCubeMapNegativeZ,	TextureTarget.TextureCubeMapPositiveZ):
			setImage[of byte]( t, null, false )
	
	# read back
	
	public def switchLevel(val as byte) as void:
		if level==val:	return
		assert fullWidth
		level = val
		wid = ((fullWidth-1)>>val)+1
		het = ((fullHeight-1)>>val)+1
	
	public def read[of T(struct)]() as (T):
		assert Allocated
		bind()
		pt = GetPixelType(T)
		data = array[of T](wid*het)
		GL.GetTexImage( target,level,pixFormat,pt, data )
		return data
	
	public def read(pt as PixelType) as void:
		assert Allocated
		bind()
		GL.GetTexImage( target,level,pixFormat,pt, IntPtr.Zero )

	# state routines
	
	# set filtering mode: point/linear
	public def filt(mode as bool, mips as bool) as void:
		if samples: return	# ignore multi-sampled
		vMin as TextureMinFilter
		vMag = (TextureMagFilter.Nearest,TextureMagFilter.Linear)[mode]
		vmi0 = (TextureMinFilter.Nearest,TextureMinFilter.Linear)
		vmi1 = (TextureMinFilter.NearestMipmapNearest,TextureMinFilter.LinearMipmapLinear)
		vMin = (vmi0,vmi1)[mips][mode]
		val = (of int: cast(int,vMin), cast(int,vMag))
		bind()
		GL.TexParameter( target, TextureParameterName.TextureMinFilter, val[0] )
		GL.TexParameter( target, TextureParameterName.TextureMagFilter, val[1] )
		filtered = true
	
	# set wrapping mode: clamp/repeat
	public def wrap(mode as TextureWrapMode, dim as int) as void:
		val = cast(int,mode)
		wraps = (TextureParameterName.TextureWrapS, TextureParameterName.TextureWrapT, TextureParameterName.TextureWrapR)
		assert dim>=0 and dim<wraps.Length
		bind()
		for wp in wraps[0:dim]:
			GL.TexParameter(target, wp, val)

	# set shadow mode: on/off
	public def shadow(en as bool) as void:
		param = 0
		bind()
		if en:
			param = cast(int, TextureCompareMode.CompareRefToTexture)
			func = cast(int, DepthFunction.Lequal)
			GL.TexParameter( target, TextureParameterName.TextureCompareFunc, func )
		if 'always':
			GL.TexParameter( target, TextureParameterName.TextureCompareMode, param )
	
	# set anisotropy
	public def anisotropy(lev as byte) as void:
		bind()
		
	# generate mipmaps
	public def genLevels() as byte:
		assert target != TextureTarget.TextureRectangle
		assert not samples
		ti = cast(GenerateMipmapTarget, cast(int,target))
		bind()
		GL.GenerateMipmap(ti)
		num as int = 0
		GL.GetTexParameterI( target, GetTextureParameter.TextureMaxLod, num )
		mipmapped = true
		return System.Math.Min(0xFF,num+1)
	
	# init all state
	public def setState(wrap as int, fl as bool, mips as bool) as void:
		wm = (TextureWrapMode.MirroredRepeat, TextureWrapMode.ClampToBorder, TextureWrapMode.Repeat)[wrap+1]
		bind()
		wrap(wm,2)
		filt(fl,mips)
		if mips and not mipmapped:
			genLevels()
	
	# select a range of LODs to sample from
	public def setLevels(a as int, b as int) as void:
		bind()
		GL.TexParameter( target, TextureParameterName.TextureBaseLevel, a )	if a>=0
		GL.TexParameter( target, TextureParameterName.TextureMaxLevel, b )	if b>=0
	public def setLevels() as void:
		setLevels(level,level)
	
	# set border color
	public def setBorder(v as Graphics.Color4) as void:
		bind()
		GL.TexParameter( target, TextureParameterName.TextureBorderColor, (v.R,v.G,v.B,v.A) )
