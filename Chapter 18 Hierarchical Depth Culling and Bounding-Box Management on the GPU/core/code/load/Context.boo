namespace kri.load

import System.Collections.Generic
import OpenTK.Graphics
import kri.shade
import kri.meta


#------		LOAD CONTEXT		------#

public static class Meta:
	private final tVert	= Template('/tmp/tc_v')
	private final tGeom	= Template('/tmp/tc_g')
	private final tFrag	= Template('/tmp/tc_f')
	public final LightSet	= ('bump','comp_diff','comp_spec',
		'diffuse','specular','glossiness')
	
	# Provide a generator+translator shader for texture coordinates on each required stage
	# link together map_input, texture_unit & target_meta
	public def MakeTexCoords( geom as bool, dict as IDictionary[of string,Hermit] ) as Object*:
		def filterInputs(tmp as Template) as string*:
			return (h.Name	for h in dict.Values	if h.Shader.type == tmp.tip)
		def ar2dict(val as string*):
			d2 = Dictionary[of string,string]()
			for v in val:
				d2[v] = string.Empty
			return d2
		# create dict
		d = Dictionary[of string,IDictionary[of string,string]]()
		d['v'] = ar2dict( filterInputs(tVert) )
		d['g'] = ar2dict( filterInputs(tGeom) )
		d['f'] = ar2dict( filterInputs(tFrag) )
		d['o'] = ar2dict(( ('mr','mv')[geom], ))
		d['p'] = ar2dict( ((of string:,),('uv0',))['uv0' in filterInputs(tVert)] )
		d['t'] = d3 = Dictionary[of string,string]()
		for v in dict:
			d3.Add( v.Key, v.Value.Name )
		# instance list
		rez = List[of Object]()
		rez.Add( tVert.instance(d) )
		rez.Add( tGeom.instance(d) )	if geom
		rez.Add( tFrag.instance(d) )
		return rez


#------		MATERIAL CONTEXT		------#

public class Shade:
	public final tc_uv0		= Object.Load("/mi/uv0_v")
	# light models
	public final lambert	= Object.Load('/mod/lambert_f')
	public final cooktorr	= Object.Load('/mod/cooktorr_f')
	public final phong		= Object.Load('/mod/phong_f')
	# meta data
	public final emissive_u		= Object.Load('/mod/emissive_u_f')
	public final emissive_t2	= Object.Load('/mod/emissive_t2d_f')
	public final diffuse_u		= Object.Load('/mod/diffuse_u_f')
	public final diffuse_t2		= Object.Load('/mod/diffuse_t2d_f')
	public final specular_u		= Object.Load('/mod/specular_u_f')
	public final specular_t2	= Object.Load('/mod/specular_t2d_f')
	public final glossiness_u	= Object.Load('/mod/glossiness_u_f')
	public final bump_c			= Object.Load('/mod/bump_c_f')
	public final bump_t2		= Object.Load('/mod/bump_t2d_f')
	# particles
	public final halo_u			= Object.Load('/mod/halo_u_f')
	public final strand_u		= Object.Load('/mod/strand_u_g')


public class Context:
	public final slib	= Shade()
	public final mDef	= kri.Material('default')

	public def fillMat(mat as kri.Material, diff as Color4, emi as single, spec as Color4, glossy as single) as void:
		ml = mat.metaList
		ml.Add( Data[of Color4]('diffuse',	slib.diffuse_u, diff ))
		ml.Add( Data[of single]('emissive',	slib.emissive_u, emi ))
		ml.Add( Data[of Color4]('specular',	slib.specular_u, spec ))
		ml.Add( Data[of single]('glossiness',	slib.glossiness_u, glossy ))
		ml.Add(Advanced	( Name:'bump', 		Shader:slib.bump_c ))
		ml.Add(Mirror())
		ml.Add(Advanced	( Name:'comp_diff',	Shader:slib.lambert ))
		ml.Add(Advanced	( Name:'comp_spec',	Shader:slib.phong ))

	public def setMatTexture(mat as kri.Material, tex as kri.buf.Texture) as void:
		me = mat.Meta['emissive']	as Advanced
		md = mat.Meta['diffuse']	as Advanced
		assert me and md
		me.Unit = md.Unit = mat.unit.Count
		un = kri.meta.AdUnit( Value:tex )
		un.input = kri.meta.Hermit( Name:'uv0', Shader:slib.tc_uv0 )
		mat.unit.Add(un)
		me.Shader = slib.emissive_t2
		md.Shader = slib.diffuse_t2

	public def constructor():
		fillMat(mDef, Color4.Gray, 0.1f, Color4.Gray, 50f)
		mDef.metaList.Add(Halo(
			Name:'halo', Shader:slib.halo_u,
			Data:OpenTK.Vector4(0.1f,50f,0f,1f)
			))
		mDef.metaList.Add(Strand(
			Name:'strand', Shader:slib.strand_u,
			Data:OpenTK.Vector4.Zero	//todo: use a meaningful value
			))
		mDef.link()
