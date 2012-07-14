namespace support.light.defer

import OpenTK
import OpenTK.Graphics.OpenGL
import kri.shade
import kri.buf


public class Context:
	public final buf		= Holder()
	public final tool		= kri.shade.Object.Load('/light/defer/sh_f')

	public def constructor():
		#buf.emitArray(3)
		for i in range(3):
			buf.at.color[i] = Texture()


#---------	LIGHT PRE-PASS	--------#

public class Bake( kri.rend.Basic ):
	protected final bu		= Bundle()
	protected final context	as support.light.Context
	protected final sphere	as kri.Mesh
	private final fbo		as Holder
	private final texDep	= par.Value[of kri.buf.Texture]('depth')
	private final va		= kri.vb.Array()
	private final static 	geoQuality	= 1
	private final static	pif = PixelInternalFormat.Rgba16f

	public def constructor(dc as Context, lc as support.light.Context):
		fbo = dc.buf
		context = lc
		# baking shader
		sa = bu.shader
		sa.add( '/light/defer/bake_v', '/light/defer/bake_f', '/lib/defer_f' )
		sa.add( dc.tool )
		sa.add( *kri.Ant.Inst.libShaders )
		sa.fragout('ca','cb','cc')
		d = par.Dict()
		d.unit(texDep)
		bu.dicts.AddRange(( d, lc.dict ))
		# create geometry
		sphere = kri.gen.Sphere( geoQuality, OpenTK.Vector3.One )

	public override def setup(pl as kri.buf.Plane) as bool:
		fbo.at.color[0].samples = 3
		fbo.resize( pl.wid, pl.het )
		return true
		
	public override def process(con as kri.rend.link.Basic) as void:
		fbo.at.depth = texDep.Value = con.Depth
		fbo.bind()
		con.SetDepth(0f,false)
		con.ClearColor( Graphics.Color4(0f,0f,0f,0f) )
		GL.CullFace( CullFaceMode.Front )
		GL.DepthFunc( DepthFunction.Gequal )
		using kri.Blender( kri.Blend.Add ):
			for l in kri.Scene.Current.lights:
				continue	if l.fov != 0f
				kri.Ant.Inst.params.activate(l)
				sphere.render(va,bu,null)
		GL.CullFace( CullFaceMode.Back )
		GL.DepthFunc( DepthFunction.Lequal )


#---------	LIGHT APPLICATION	--------#

public class Apply( kri.rend.tech.Meta ):
	private final buf	as Holder
	private final pTex	= par.Texture('light')
	# init
	public def constructor(dc as Context):
		super('lit.defer', false, null,
			'bump','emissive','diffuse','specular','glossiness')
		buf = dc.buf
		pTex.Value = buf.at.color[0] as Texture
		dict.unit( pTex )
		shobs.Add( dc.tool )
		shade('/light/defer/apply')
	# work
	public override def process(con as kri.rend.link.Basic) as void:
		con.activate( con.Target.Same, 0f, false )
		drawScene()
