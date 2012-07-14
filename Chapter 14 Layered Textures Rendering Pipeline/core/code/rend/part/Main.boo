namespace kri.rend.part

import OpenTK.Graphics.OpenGL


#---------	RENDER PARTICLES BASE		--------#

public class Basic( kri.rend.Basic ):
	protected abstract def prepare(pe as kri.part.Emitter, ref nin as uint) as kri.shade.Bundle:
		pass
	public def drawScene() as void:
		scene = kri.Scene.Current
		if not scene:	return
		using kri.Blender( kri.Blend.Add ),\
		kri.Section( EnableCap.ClipPlane0 ),\
		kri.Section( EnableCap.VertexProgramPointSize ):
			for pe in scene.particles:
				if not (pe.visible and pe.filled):
					continue
				nInst as uint = 1
				bu = prepare(pe,nInst)
				pe.draw(bu,nInst)


#---------	RENDER PARTICLES: SINGLE SHADER		--------#

public abstract class Simple( Basic ):
	protected final bu		= kri.shade.Bundle()
	public dTest	as bool	= true
	protected override def prepare(pe as kri.part.Emitter, ref nin as uint) as kri.shade.Bundle:
		return bu
	public override def process(con as kri.rend.link.Basic) as void:
		off = (single.NaN,0f)[dTest]
		con.activate( con.Target.Same, off, false )
		drawScene()
