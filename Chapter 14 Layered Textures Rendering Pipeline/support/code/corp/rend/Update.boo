namespace support.corp.rend


public class Update( kri.rend.Basic ):
	public override def process(link as kri.rend.link.Basic) as void:
		scene = kri.Scene.Current
		if not scene:	return
		for ent in scene.entities:
			tag = ent.seTag[of support.corp.Emitter]()
			if not tag:	return
			tag.owner.process( tag, ent.mesh )