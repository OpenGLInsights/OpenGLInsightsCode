namespace support.bake.edge


public class Tag( kri.ITag ):
	public worldSpace	as bool		= true	# in world space
	public stamp		as double	= -1f	# last update
	public final		data	= kri.vb.Object()
	public size			as uint		= 0
	public step			as single	= 1f


#---------	RENDER VERTEX SPATIAL TO EDGE BUFFER		--------#

public class Update( kri.rend.tech.Basic ):
	private final tf		= kri.TransFeedback(1)
	private final bu		= kri.shade.Bundle()
	public	final va		= kri.vb.Array()
	public	final channel	as byte
	
	public def constructor(texId as byte, putId as bool):
		super('bake.edge')
		channel = texId
		# surface shader
		sa = bu.shader
		sa.add( '/uv/edge/bake_v', '/lib/quat_v', '/uv/edge/bake_g' )
		sa.add('/uv/set/norm_v')
		sa.feedback(false, 'rez_vertex', 'rez_quat')

	public override def process(con as kri.rend.link.Basic) as void:
		scene = kri.Scene.Current
		if not scene:	return
		con.DepthTest = false
		for e in scene.entities:
			tag = e.seTag[of Tag]()
			if not e.visible or not tag:
				continue
			tag.stamp = kri.Ant.Inst.Time
			n = (null,e.node)[tag.worldSpace]
			kri.Ant.Inst.params.spaModel.activate(n)
			tf.Bind( tag.data )
			using tf.discard():
				if not e.render(va,bu):
					kri.lib.Journal.Log("Bake: edge update failed for ${e.node.name}")
					e.tags.Remove(tag)
