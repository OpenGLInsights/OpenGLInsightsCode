namespace support.hair

import OpenTK

#-----------------------------------#
#		Main Hair Behavior			#
#-----------------------------------#

public class Behavior( kri.part.Behavior ):
	# Y = cur seg ID, Z = 1 / segments
	public final pSegment	= kri.shade.par.Value[of Vector4]('fur_segment')
	# X = 1 / avg_len
	public final pSystem	= kri.shade.par.Value[of Vector4]('fur_system')
	# number of layers
	public final layers		as byte
	private final posName	as string
	public layParam		as Vector4
	# fun
	public def constructor(pc as kri.part.Context, segs as byte):
		super('/part/beh/fur_main')
		enrich(3,'pos','speed')
		layers = segs
		kd = 1f / segs
		pSegment.Value	= Vector4( 0f, 0f, kd, 0f )
		pSystem.Value	= Vector4.Zero
	public override def link(d as kri.shade.par.Dict) as void:
		d.var(pSegment,pSystem)

	# generate fur layers
	public def genLayers(em as kri.part.Emitter) as (kri.part.Emitter):
		lar = List[of kri.part.Emitter](
			kri.part.Emitter(em.owner,"${em.name}-${i}")
			for i in range(layers) ).ToArray()
		assert not em.obj.seTag[of Tag]()
		tag = Tag( em.owner.Total )
		tag.param = layParam
		em.obj.tags.Add(tag)
		# localize id in a function
		def genFunc(id as int):
			return do(e as kri.Entity) as bool:
				pSegment.Value.Y = 1f*id
				return tag.Ready
		# external attribs setup
		ex0 as kri.vb.Entry
		ex1 as kri.vb.Entry
		for i in range(lar.Length):
			pe = lar[i]
			pe.allocate()
			pe.obj = em.obj
			pe.mat = em.mat
			pe.onUpdate = genFunc(i)
			if i == 0:
				ex0 = kri.vb.Entry( tag.data, 'root_prev' )
				ex1 = kri.vb.Entry( tag.data, 'root_base' )
			else:
				if i == 1:
					ex0 = kri.vb.Entry( tag.data, 'root_base' )
				else:
					ex0 = kri.vb.Entry( lar[i-2], 'pos' )
				ex1 = kri.vb.Entry( lar[i-1], 'pos' )
			pe.entries['prev'] = ex0
			pe.entries['base'] = ex1
		return lar
	
	# add children dependencies
	public static def prepareChildren(scene as kri.Scene, man as kri.part.Manager) as void:
		root as kri.vb.Attrib = null
		for pe in scene.particles:
			if pe.owner != man:
				continue
			assert pe.obj
			tag = pe.obj.seTag[of Tag]()
			assert tag
			if not root:
				root = tag.data
			assert not 'ready'
			#pe.exData.vbo.Add(root)
