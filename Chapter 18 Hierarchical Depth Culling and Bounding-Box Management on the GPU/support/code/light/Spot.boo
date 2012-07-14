namespace support.light.spot

import System
import OpenTK.Graphics.OpenGL
import support.light

	
#---------	LIGHT MAP FILL	--------#

public class Fill( kri.rend.tech.Sorted ):
	public	final fbo		= kri.buf.Holder()
	protected final bu		= kri.shade.Bundle()
	protected final licon	as Context

	public def constructor(lc as Context):
		super('lit.bake')
		licon = lc
		# buffer init
		if lc.shadowFormat == ShadowFormat.Variance:
			fbo.mask = 1
			fbo.at.depth = kri.buf.Texture.Depth(0)
			fbo.at.color[1] = kri.buf.Texture(
				intFormat:PixelInternalFormat.Rg16 )
			fbo.resize( lc.size,lc.size )
		else: fbo.mask = 0
		# spot shader
		bu.dicts.Add( lc.dict )
		bu.shader.add( '/light/bake_v', '/lib/tool_v', '/lib/quat_v' )
		bu.shader.add( lc.getFillShader() )
		bu.link()

	public override def construct(mat as kri.Material) as kri.shade.Bundle:
		return bu

	public override def process(con as kri.rend.link.Basic) as void:
		if not kri.Scene.Current:	return
		con.SetDepth(1f, true)
		for l in kri.Scene.Current.lights:
			if l.fov == 0f: continue
			kri.Ant.Inst.params.activate(l)
			index = (-1,0)[licon.shadowFormat == ShadowFormat.Variance]
			if not l.depth:
				if not licon.size:
					raise 'Light context created without shadow support'
				ask = kri.rend.link.Buffer.FmDepth[licon.bits>>3]
				pif = ( ask, PixelInternalFormat.Rg16 )[index+1]
				pix = ( PixelFormat.DepthComponent, PixelFormat.Rgba )[index+1]
				l.depth = kri.buf.Texture( intFormat:pif, pixFormat:pix,
					wid:licon.size, het:licon.size )
			if index<0:
				fbo.at.depth = l.depth
			else:
				fbo.at.color[0] = l.depth
			fbo.mask = index+1
			fbo.bind()
			con.ClearColor( OpenTK.Graphics.Color4.White )	if not index
			con.ClearDepth( 1f )
			drawScene()
			# post-prepare texture
			kri.buf.Texture.Slot(8)
			l.depth.setState( 0, licon.smooth, licon.mipmap )
			l.depth.shadow( licon.shadowFormat == ShadowFormat.Simple )


#---------	LIGHT MAP APPLY	--------#

public class Apply( kri.rend.tech.Meta ):
	private lit as kri.Light	= null
	private final texLit	as kri.shade.par.Texture

	public def constructor(lc as Context):
		super('lit.apply', false, null, *kri.load.Meta.LightSet)
		shobs.Extend(( lc.getApplyShader(), lc.sh_common ))
		shade('/light/spot/apply')
		dict.attach(lc.dict)
		texLit = lc.texLit
	# prepare
	protected override def getUpdater(mat as kri.Material) as System.Func[of int]:
		metaFun = super(mat)
		curLight = lit	# need current light only
		return do() as int:
			texLit.Value = curLight.depth
			kri.Ant.Inst.params.activate(curLight)
			return metaFun()
	# work
	public override def process(con as kri.rend.link.Basic) as void:
		scene = kri.Scene.Current
		if not scene:	return
		butch.Clear()
		for l in scene.lights:
			if l.fov == 0f:
				continue
			lit = l
			# determine subset of affected objects
			for e in scene.entities:
				addObject(e,null)
		butch.Sort( kri.rend.tech.Batch.cMat )
		# draw
		con.activate( con.Target.Same, 0f, false )
		using kri.Blender( kri.Blend.Add ):
			for b in butch:
				b.draw()
