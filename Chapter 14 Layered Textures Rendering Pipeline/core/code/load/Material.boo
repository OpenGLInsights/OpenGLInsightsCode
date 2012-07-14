namespace kri.load

import System.Collections.Generic
import kri.meta
import OpenTK
import OpenTK.Graphics


public class ExMaterial( kri.IExtension ):
	public final limDict	= Dictionary[of string,callable(Reader) as Hermit]()
	public final con		= Context()
	public prefix	as string	= 'res'
	# texture target name -> (meta name, shader)
	public final tarDict = Dictionary[of string,MapTarget]()

	public struct MapTarget:
		public final name	as string
		public final prog	as kri.shade.Object
		public def constructor(s as string, p as kri.shade.Object):
			name,prog = s,p
	
	def kri.IExtension.attach(nt as Native) as void:
		init()
		# fill targets
		tarDict['color_diffuse']	= MapTarget('diffuse',	con.slib.diffuse_t2 )
		tarDict['color_emission']	= MapTarget('emissive',	con.slib.emissive_t2 )
		tarDict['color_specular']	= MapTarget('specular',	con.slib.specular_t2 )
		tarDict['normal']			= MapTarget('bump',		con.slib.bump_t2 )
		tarDict['mirror']			= MapTarget('mirror',	null )
		for s in ('diffuse','emission','specular'):
			tarDict[s] = tarDict['color_'+s]
		# material
		nt.readers['mat']		= p_mat
		nt.readers['m_hair']	= pm_hair
		nt.readers['m_halo']	= pm_halo
		nt.readers['m_surf']	= pm_surf
		nt.readers['m_diff']	= pm_diff
		nt.readers['m_spec']	= pm_spec
		nt.readers['unit']		= pm_unit
		nt.readers['t_map']		= pt_map
		nt.readers['t_samp']	= pt_samp
		nt.readers['t_path']	= pt_path
		nt.readers['t_seq']		= pt_seq
		nt.readers['t_color']	= pt_color
		nt.readers['t_ramp']	= pt_ramp
		nt.readers['t_zero']	= pt_zero
		nt.readers['t_noise']	= pt_noise
		nt.readers['t_blend']	= pt_blend
		nt.readers['t_cube']	= pt_cube
	
	
	private def init() as void:
		uvShaders = List[of kri.shade.Object](
			kri.shade.Object.Load("/mi/uv${i}_v") for i in range(4)
			).ToArray()
		orcoVert =	kri.shade.Object.Load('/mi/orco_v')
		orcoHalo =	kri.shade.Object.Load('/mi/orco_halo_f')
		objectShader = 	kri.shade.Object.Load('/mi/object_v')
		# todo?: normal & reflection in fragment
		# trivial sources
		def genFun(x as Hermit): return {return x}
		for s in ('GLOBAL','WINDOW','NORMAL','REFLECTION','TANGENT','STRAND'):
			slow = s.ToLower()
			suf = 'v'
			suf = 'f'	if s == 'WINDOW'
			suf = 'g'	if s == 'STRAND'
			sh = kri.shade.Object.Load( "/mi/${slow}_${suf}" )
			mt = Hermit( Shader:sh, Name:slow )	# careful!
			limDict[s] = genFun(mt)
		# non-trivial sources
		limDict['UV']		= do(r as Reader):
			lid = r.getByte()
			if lid>=uvShaders.Length:	return null
			return Hermit( Shader:uvShaders[lid],	Name:'uv'+lid )
		limDict['ORCO']		= do(r as Reader):
			mat = r.geData[of kri.Material]()
			if not mat:	return null
			sh = (orcoVert,orcoHalo)[ mat.Meta['halo'] != null ]
			return Hermit( Shader:sh, Name:'orco' )
		limDict['OBJECT']	= do(r as Reader):
			mio = InputObject( Shader:objectShader,	Name:'object' )
			r.addResolve( mio.pNode.activate )
			return mio
	

	#---	Parse texture unit	---#
	
	public def pm_unit(r as Reader) as bool:
		m = r.geData[of kri.Material]()
		if not m:	return false
		# map targets
		u = AdUnit()
		m.unit.Add(u)
		r.puData(u)
		while (name = r.getString()) != '':
			factor = r.getReal()
			u.affects[name] = factor
			targ as MapTarget
			if not tarDict.TryGetValue(name,targ):
				kri.lib.Journal.Log("Loader: unknown map target: ${name}")
				continue
			me = m.Meta[targ.name] as Advanced
			if not me:
				continue
			me.Unit = m.unit.IndexOf(u)
			me.Shader = targ.prog
		# map inputs
		name = r.getString()
		proj = r.getString()
		if proj != 'FLAT':
			kri.lib.Journal.Log("Loader: projection (${proj}) not supported in material '${m.name}'")
		fun as callable(Reader) as Hermit = null
		if limDict.TryGetValue(name,fun):
			u.input = fun(r)
			return true
		else:
			kri.lib.Journal.Log("Loader: mapping (${name}) not supported in material '${m.name}'")
			return false
	
	#---	Parse material	---#
	public def p_mat(r as Reader) as bool:
		m = kri.Material( r.getString() )
		r.getByte()	# shadeless
		r.getByte()	# tangent shading
		r.at.mats[m.name] = m
		r.puData(m)
		r.addPostProcess() do(n as kri.Node):
			m.link()
		return true

#---	Strand properties	---#
	public def pm_hair(r as Reader) as bool:
		m = r.geData[of kri.Material]()
		if not m:	return false
		ms = Strand( Name:'strand', Data:r.getVec4() )
		r.getByte()	# tangent shading
		r.getReal()	# surface diffuse distance
		r.getString()	# UV layer to override
		ms.Shader = con.slib.strand_u
		m.metaList.Add(ms)
		return true
	
	#---	Halo properties		---#
	public def pm_halo(r as Reader) as bool:
		m = r.geData[of kri.Material]()
		if not m:	return false
		mh = Halo( Name:'halo', Data:Vector4(r.getVector()) )
		r.getByte()	# use texture - ignored
		mh.Shader = con.slib.halo_u
		m.metaList.Add(mh)
		return true
	
	#---	Surface properties	---#
	public def pm_surf(r as Reader) as bool:
		m = r.geData[of kri.Material]()
		if not m:	return false
		r.getByte()	# shadeless
		r.getReal()	# parallax
		m.metaList.Add( Advanced( Name:'bump', Shader:con.slib.bump_c ))
		r.getReal()	# ambient
		r.getReal()	# translucency
		return true
	
	#---	Meta: diffuse	---#
	public def pm_diff(r as Reader) as bool:
		m = r.geData[of kri.Material]()
		if not m:	return false
		m.metaList.Add( Data[of Color4]('diffuse',
			con.slib.diffuse_u,		r.getColorFull() ))
		m.metaList.Add( Data[of single]('emissive',
			con.slib.emissive_u,	r.getReal() ))
		model = r.getString()
		sh = { '':		con.slib.lambert,
			'LAMBERT':	con.slib.lambert
			}[model] as kri.shade.Object
		if not sh:
			kri.lib.Journal.Log("Loader: Unknown diffuse lighting model (${model})")
			return false
		m.metaList.Add(Advanced( Name:'comp_diff', Shader:sh ))
		return true

	#---	Meta: specular	---#
	public def pm_spec(r as Reader) as bool:
		m = r.geData[of kri.Material]()
		if not m:	return false
		m.metaList.Add( Data[of Color4]('specular',
			con.slib.specular_u,	r.getColorFull() ))
		m.metaList.Add( Data[of single]('glossiness',
			con.slib.glossiness_u,	r.getReal() ))
		m.metaList.Add(Mirror())
		model = r.getString()
		sh = {
			'COOKTORR':	con.slib.cooktorr,
			'PHONG':	con.slib.phong,
			'BLINN':	con.slib.phong	#fake
			}[model] as kri.shade.Object
		if not sh:
			kri.lib.Journal.Log("Loader: Unknown specular lighting model (${model})")
			return false
		m.metaList.Add( Advanced( Name:'comp_spec', Shader:sh ))
		return true

	#---	Texture: mapping	---#
	public def pt_map(r as Reader) as bool:
		u = r.geData[of AdUnit]()
		if not u:	return false
		# tex-coords
		u.pOffset.Value	= Vector4(r.getVector(), 0.0)
		u.pScale.Value	= Vector4(r.getVector(), 1.0)
		return true

	#---	Texture: sampling	---#
	public def pt_samp(r as Reader) as bool:
		u = r.geData[of AdUnit]()
		if not (u and u.Value):	return false
		sRepeat	= r.getString()	# extend by repeat
		iRepeat = (0,1)[sRepeat=='REPEAT']
		bMipMap	= r.getByte()>0	# generate mip-maps
		bFilter	= r.getByte()>0	# linear filtering
		# init sampler parameters, todo: use sampler object
		u.Value.setState( iRepeat, bFilter, bMipMap )
		return true

	#---	Texture: file path	---#
	public def pt_path(r as Reader) as bool:
		u = r.geData[of AdUnit]()
		if not u:	return false
		path = prefix + r.getString()
		# set gamma correction
		u.isBump = r.getByte()>0
		image.Basic.GammaCorrected = not u.isBump
		# load image
		u.Value = r.data.load[of kri.buf.Texture](path)
		return u.Value != null

	#---	Texture: sequence	---#
	public def pt_seq(r as Reader) as bool:
		return false

	#---	Texture: color		---#
	public def pt_color(r as Reader) as bool:
		r.getColor()	# factor
		r.getReal()	# brightness
		r.getReal()	# contrast
		r.getReal()	# saturation
		return true
	
	#---	Texture: color ramp		---#
	public def pt_ramp(r as Reader) as bool:
		u = r.geData[of AdUnit]()
		if not u:	return false
		r.getString()	# interpolator
		num = r.getByte()
		data = array[of kri.gen.Texture.Key](num)
		for i in range(num):
			data[i].pos		= r.getReal()
			data[i].col		= r.getColor()
			data[i].col.A	= r.getReal()
		u.Value = kri.gen.Texture.ofCurve(data)
		return u.Value != null
	
	#---	Texture: zero		---#
	public def pt_zero(r as Reader) as bool:
		u = r.geData[of AdUnit]()
		if not u:	return false
		u.Value = null
		return true

	#---	Texture: noise		---#
	public def pt_noise(r as Reader) as bool:
		u = r.geData[of AdUnit]()
		if not u:	return false
		u.Value = kri.gen.Texture.noise
		return true

	#---	Texture: blend		---#
	public def pt_blend(r as Reader) as bool:
		r.getString()	# interpolator
		r.getByte()		# flip_axis
		return true
	
	#---	Texture: environment	---#
	public def pt_cube(r as Reader) as bool:
		effect = 'mirror'
		m = r.geData[of kri.Material]()
		u = r.geData[of AdUnit]()
		x = m.Meta[effect] as Mirror
		if not (m and u and x) :
			return false
		if effect not in u.affects:
			kri.lib.Journal.Log("Loader: environment unit has to affect ${effect}")
			return false
		# prepare for sampling after loading
		r.addPostProcess() do(n as kri.Node):
			u.Value.setState(0,true,true)	if u.Value
		amount = u.affects[effect]
		x.Value = Vector4.One * amount
		return true
