namespace support.defer

import OpenTK.Graphics.OpenGL
import System.Collections.Generic
import kri.shade

#---------	DEFERRED BASE APPLY		--------#

public class ApplyBase( kri.rend.Basic ):
	protected	final	bus	= Dictionary[of support.light.ShadowType,Bundle]()
	public		initOnly	= false
	# custom activation
	private virtual def onInit() as void:
		pass
	private virtual def onDraw() as void:
		pass
	# create
	protected def constructor(con as Context, lc as support.light.Context, spath as string):
		shader = Object.Load(spath)
		for st in System.Enum.GetValues(support.light.ShadowType) as (support.light.ShadowType):
			bu = Bundle()
			bus.Add(st,bu)
			bu.dicts.Add( con.dict )
			bu.shader.add( '/lib/quat_v','/lib/tool_v','/lib/defer_f','/lib/math_f' )
			bu.shader.add( shader, con.sh_apply, con.sh_diff, con.sh_spec )
			if lc:
				bu.dicts.Add( lc.dict )
				bu.shader.add( *lc.getApplyShaders(st) )
			else:	break
	# work
	public override def process(link as kri.rend.link.Basic) as void:
		link.activate(false)
		onInit()
		if initOnly:	return
		# enable depth check
		link.activate( link.Target.Same, 0f, false )
		GL.CullFace( CullFaceMode.Front )
		GL.DepthFunc( DepthFunction.Gequal )
		link.TextureCubeMapSeamless = true
		# add lights
		using kri.Blender( kri.Blend.Add ):
			onDraw()
		GL.CullFace( CullFaceMode.Back )
		GL.DepthFunc( DepthFunction.Lequal )


#---------	DEFERRED STANDARD APPLY		--------#

public class Apply( ApplyBase ):
	private final bv		= Bundle()
	private final texShadow	as par.Texture
	private final sphere	as kri.gen.Frame
	private final cone		as kri.gen.Frame
	private final noShadow	as kri.buf.Texture
	# init
	public def constructor(lc as support.light.Context, con as Context):
		super(con,lc,'/g/apply_v')
		sphere = con.sphere
		cone = con.cone
		texShadow = lc.texLit
		noShadow = lc.defShadow
		# fill shader
		bv.shader.add( '/copy_v', '/g/init_f' )
		bv.dicts.Add( con.dict )
	# shadow
	private def bindShadow(t as kri.buf.Texture) as Bundle:
		st = support.light.ShadowType.None
		if t:
			texShadow.Value = t
			st = support.light.ShadowType.Spot
			if t.target == TextureTarget.Texture2DArray:
				st = support.light.ShadowType.Dual
			if t.target == TextureTarget.TextureCubeMap:
				st = support.light.ShadowType.Cube
		else:
			texShadow.Value = noShadow
		return bus[st]
	# work
	private override def onInit() as void:
		kri.Ant.Inst.quad.draw(bv)

	private override def onDraw() as void:
		scene = kri.Scene.Current
		if not scene:	return
		for l in scene.lights:
			frame = {
				kri.Light.Type.Omni:	sphere,
				kri.Light.Type.Spot:	cone,
			}[l.getType()] as kri.gen.Frame
			if not frame:
				kri.lib.Journal.Log("Light: no volume found for light (${l})")
				continue
			bu = bindShadow( l.depth )
			kri.Ant.Inst.params.activate(l)
			frame.draw(bu)
