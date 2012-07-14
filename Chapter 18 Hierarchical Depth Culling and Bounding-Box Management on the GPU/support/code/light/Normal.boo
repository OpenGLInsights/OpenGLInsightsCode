namespace support.light.normal

#---------	LIGHT NORMALS --------#

public class Apply( kri.rend.tech.Meta ):
	private lit as kri.Light	= null
	private final texLit	as kri.shade.par.Texture
	private final defTex	as kri.buf.Texture

	public def constructor(lc as support.light.Context):
		super('lit.normal.apply', false, null, *kri.load.Meta.LightSet)
		defTex = lc.defShadow
		#shobs.Add( lc.getApplyShader() )
		shobs.Extend(( lc.sh_dummy, lc.sh_common ))
		shade('/light/normal/apply')
		dict.attach(lc.dict)
		texLit = lc.texLit
	# prepare
	protected override def getUpdater(mat as kri.Material) as System.Func[of int]:
		metaFun = super(mat)
		curLight = lit	# need current light only
		return do() as int:
			texLit.Value = d = curLight.depth
			if not d:	texLit.Value = defTex
			kri.Ant.Inst.params.activate(curLight)
			return metaFun()
	# work
	public override def process(con as kri.rend.link.Basic) as void:
		scene = kri.Scene.Current
		if not scene:	return
		butch.Clear()
		for l in scene.lights:
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

