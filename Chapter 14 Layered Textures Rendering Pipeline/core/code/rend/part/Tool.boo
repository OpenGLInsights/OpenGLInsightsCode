namespace kri.rend.part

public class Standard( Meta ):
	public	depthTest	= true
	public def constructor(pc as kri.part.Context):
		super('part.std', false, 'halo','diffuse')
		shobs.Add( pc.sh_draw )
		shade('/part/draw/load')
	public override def process(con as kri.rend.link.Basic) as void:
		if depthTest:
			con.activate( con.Target.Same, 0f, false )
		else:	con.activate(false)
		drawScene()


public class Dummy( Simple ):
	public def constructor(pc as kri.part.Context):
		bu.shader.add( pc.sh_draw )
		bu.shader.add( *kri.Ant.Inst.libShaders )
		bu.shader.add('/part/draw/simple_v','/part/draw/simple_f')
