namespace support.light.omni

import OpenTK.Graphics.OpenGL


#---------	LIGHT OMNI FILL	--------#

public class Fill( kri.rend.tech.Sorted ):
	protected final fbo		= kri.buf.Holder(mask:0)
	protected final bu		= kri.shade.Bundle()
	protected final context	as support.light.Context

	public def constructor(lc as support.light.Context):
		super('lit.omni.bake')
		context = lc
		# omni shader
		bu.shader.add( '/light/omni/bake_v', '/light/omni/bake_g', '/empty_f' )
		bu.shader.add( *kri.Ant.Inst.libShaders )
		bu.dicts.Add( lc.dict )
		bu.link()

	public override def construct(mat as kri.Material) as kri.shade.Bundle:
		return bu

	public override def process(con as kri.rend.link.Basic) as void:
		scene = kri.Scene.Current
		if scene: return	# force exit
		if not scene:	return
		con.SetDepth(1f, true)	# offset for HW filtering
		for l in scene.lights:
			if l.fov != 0f:	continue
			kri.Ant.Inst.params.activate(l)
			if not l.depth:
				if not context.size:
					raise 'Light context created without shadow support'
				l.depth = t = kri.buf.Texture.Depth(0)
				t.target = TextureTarget.TextureCubeMap
				t.wid = t.het = context.size
				t.initCube()
				t.shadow(false)
				t.setState(0,false,false)
			elif not 'Debug':
				data = fbo.readAll[of single]( PixelFormat.DepthComponent )
				data[0] = 0f
			fbo.at.depth = l.depth
			fbo.bind()
			con.ClearDepth(0.5)
			#drawScene()


#---------	LIGHT OMNI APPLY	--------#

public class Apply( kri.rend.tech.Meta ):
	private lit as kri.Light	= null
	private final smooth	as bool
	private final texLit	as kri.shade.par.Texture
	
	public def constructor(lc as support.light.Context, bSmooth as bool):
		super('lit.omni.apply', false, null, *kri.load.Meta.LightSet)
		shobs.Add( lc.sh_common )
		shade('/light/omni/apply')
		smooth = bSmooth
		texLit = lc.texLit
	protected override def getUpdater(mat as kri.Material) as System.Func[of int]:
		metaFun = super(mat)
		curLight = lit
		return do() as int:
			#texLit.Value = curLight.depth
			kri.Ant.Inst.params.activate(curLight)
			return metaFun()
	
	public override def process(con as kri.rend.link.Basic) as void:
		scene = kri.Scene.Current
		if not scene:	return
		con.activate( con.Target.Same, 0f, false )
		butch.Clear()
		for l in scene.lights:
			if l.fov != 0f:	continue
			d = (lit=l).depth
			d = null
			#if not (d and d.target==TextureTarget.TextureCubeMap):
			#	continue
			# determine subset of affected objects
			for e in scene.entities:
				addObject(e,null)
		using kri.Blender( kri.Blend.Add ):
			butch.Sort( kri.rend.tech.Batch.cMat )
			for b in butch:
				b.draw()
