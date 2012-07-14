namespace kri.rend


#---------	BASIC RENDER	--------#

public class Basic:
	public	active	as bool = true
	public virtual def setup(pl as kri.buf.Plane) as bool:
		return true
	public virtual def process(con as link.Basic) as void:
		pass


#---------	GENERAL FILTER	--------#

public class Filter(Basic):
	protected	final bu	= kri.shade.Bundle()
	protected	final texIn	= kri.shade.par.Texture('input')
	protected	final dict	= kri.shade.par.Dict()
	protected	linear		= false
	
	public def constructor():
		dict.unit(texIn)
		bu.dicts.Add(dict)
	public override def process(con as link.Basic) as void:
		texIn.Value = con.Input
		con.Input.filt(linear,false)
		con.activate(true)
		kri.Ant.Inst.quad.draw(bu)

public class FilterCopy(Filter):
	public def constructor():
		bu.shader.add('/copy_v','/copy_f')


#---------	RENDER GROUP	--------#

public class Group(Basic):
	public	final	renders	as (Basic)	= null
	public def constructor(*ren as (Basic)):
		renders = ren
	
	public override def setup(pl as kri.buf.Plane) as bool:
		return System.Array.TrueForAll(renders) do(r as Basic):
			return r.setup(pl)
	
	public override def process(con as link.Basic) as void:
		for r in renders:
			if r.active:
				r.process(con)
