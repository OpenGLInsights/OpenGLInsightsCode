namespace support.corp.inst


public class Meta( kri.meta.Advanced ):
	public ent	as kri.Entity	= null
	def System.ICloneable.Clone() as object:
		return copyTo( Meta( ent:ent ))
	def kri.meta.IBase.link(d as kri.shade.par.Dict) as void:
		pass


public class Rend( kri.rend.tech.Meta ):
	private	cur		as kri.part.Emitter	= null
	private	final	pBase	= kri.shade.par.Value[of OpenTK.Vector4]('base_color')
	
	public def constructor(pc as kri.part.Context):
		super('part.object', false, null, 'emissive')
		# shade
		pBase.Value = OpenTK.Vector4.UnitX
		dict.var(pBase)
		shade(( '/part/draw/obj_v', '/mat_base_f' ))
	
	public override def process(con as kri.rend.link.Basic) as void:
		con.activate( con.Target.Same, 0f, true )
		butch.Clear()
		for pe in kri.Scene.Current.particles:
			if not pe.mat:	continue	
			inst = pe.mat.Meta['inst'] as Meta
			if not inst:	continue	
			ent = inst.ent
			if not ent:		continue
			for s in ('pos','rot','sys','sub'):
				en = kri.vb.Entry( pe.mesh.buffers[0], s )
				if not en.buffer:
					ent = null
					break
				en.divisor = 1
				extraDict['ghost_'+s] = en
			if ent:
				cur = pe
				addObject(ent,null)
		for b in butch:
			b.draw()
