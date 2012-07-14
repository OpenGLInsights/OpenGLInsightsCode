namespace support.asm

public class DrawSimple( kri.rend.Basic ):
	public	final	bu	= kri.shade.Bundle()
	public	final	va	= kri.vb.Array()
	public	final	q	= kri.Query()
	public	final	con	as Context	= null
	
	public def constructor(ctx as Context):
		con = ctx
		bu.dicts.Add( con.dict )
		bu.shader.add('/asm/draw/simple_v', '/asm/draw/simple_f', '/lib/quat_v','/lib/tool_v')
	
	public override def process(link as kri.rend.link.Basic) as void:
		link.activate( link.Target.Same, 0f, true )
		link.ClearDepth(1.0)
		link.ClearColor()
		sc = Scene.Current
		if not sc:	return
		con.activate(sc)
		for lit in sc.lights:
			kri.Ant.Inst.params.activate(lit)
			sc.mesh.render(va,bu,null)
