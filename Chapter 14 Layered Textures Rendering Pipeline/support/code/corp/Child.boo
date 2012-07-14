namespace support.corp.child

import OpenTK


public class Meta( kri.meta.Advanced ):
	public num	as uint	= 0
	private final data	= kri.shade.par.Value[of Vector4]('part_child')
	portal Data	as Vector4	= data.Value
	
	def System.ICloneable.Clone() as object:
		return copyTo( Meta( num:num, Data:Data ))
	def kri.meta.IBase.link(d as kri.shade.par.Dict) as void:
		d.var(data)


public class Rend( kri.rend.part.Meta ):
	public def constructor(pc as kri.part.Context):
		super('part.child', false, 'halo','diffuse','child')
		shobs.AddRange(( pc.sh_draw, pc.sh_tool, pc.sh_child ))
		shade(( '/part/draw/load_child_v', '/part/draw/load_f' ))

	protected override def update(pe as kri.part.Emitter) as uint:
		mat = pe.mat
		assert mat
		me = mat.Meta['child'] as Meta
		assert me
		return me.num

	public override def process(con as kri.rend.link.Basic) as void:
		con.activate( con.Target.Same, 0f, false )
		drawScene()
