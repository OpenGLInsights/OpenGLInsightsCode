namespace support.light

import kri.shade

public enum ShadowFormat:
	Simple
	Exponent
	Variance
public enum ShadowType:
	None
	Spot
	Dual
	Cube

#---------	LIGHT CONTEXT	--------#

public class Context:
	public final size	as uint	= 0
	public final layers	as uint	= 0
	public final texLit	= par.Texture('light')
	public final pDark	= par.Value[of single]('k_dark')
	public final pX		= par.Value[of OpenTK.Vector4]('dark')
	public final pOff	= par.Value[of single]('texel_offset')
	public final pHemi	= par.Value[of single]('hemi')
	public final dict	= par.Dict()
	public bits		as uint	= 0
	public mipmap	as bool = false
	public smooth	as bool	= true
	public shadowFormat		= ShadowFormat.Simple
	public final defShadow	= kri.gen.Texture.depth
	public final sh_dummy	= Object.Load('/light/shadow/dummy_f')
	public final sh_common	= Object.Load('/light/common_f')

	# init
	public def constructor():
		dict.var(pDark,pOff,pHemi)
		dict.var(pX)
		dict.unit(texLit)
	public def constructor(nlay as uint, qlog as uint):
		self()
		layers,size	= nlay,1<<qlog

	# exponential
	public def setExpo(darkness as single, kernel as single) as void:
		shadowFormat = ShadowFormat.Exponent
		bits = 32
		pDark.Value	= darkness
		pOff.Value	= kernel / size
		pX.Value = OpenTK.Vector4(5f, 5f, 4f, kernel / size)
	# variance
	public def setVariance() as void:
		shadowFormat = ShadowFormat.Variance
		bits = 0

	# shadow shaders
	public def getFillShader() as Object:
		name = '/empty_f'
		name = '/light/bake_exp_f'	if shadowFormat == ShadowFormat.Exponent
		name = '/light/bake_var_f'	if shadowFormat == ShadowFormat.Variance
		return Object.Load(name)
	
	public def getApplyShaders(ot as ShadowType) as (Object):
		if ot == ShadowType.None:
			return (sh_dummy,)
		path = '/light/shadow' + {
			ShadowType.Spot:	'/spot',
			ShadowType.Cube:	'/omni/cube',
			ShadowType.Dual:	'/omni/dual',
		}[ot]
		name = {
			ShadowFormat.Simple:	'simple',
			ShadowFormat.Exponent:	'exponent2',
			ShadowFormat.Variance:	'variance',
		}[shadowFormat]
		x0 = Object.Load("${path}/base_f")
		x1 = Object.Load("${path}/${name}_f")
		return (of Object: x0,x1)
	
	public def getApplyShader() as Object:
		return getApplyShaders( ShadowType.Spot )[1]
