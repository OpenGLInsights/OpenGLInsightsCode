namespace support.corp.rend

import OpenTK.Graphics.OpenGL

#---------	RENDER PARTICLES BASE		--------#

public abstract class Basic( kri.rend.tech.Sorted ):
	protected override def isGood(ent as kri.Entity) as bool:
		tag = ent.seTag[of support.corp.Emitter]()
		return false	if not tag
		return false	if tag.state != support.corp.State.Filled
		return super(ent)
	
	public def constructor(name as string):
		super(name)
	public override def drawScene() as void:
		using kri.Blender( kri.Blend.Add ),	\
		kri.Section( EnableCap.ClipPlane0 ),\
		kri.Section( EnableCap.VertexProgramPointSize ):
			super()	
