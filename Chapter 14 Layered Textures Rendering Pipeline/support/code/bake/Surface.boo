namespace support.bake.surf

import OpenTK.Graphics


public class Tag( kri.ITag ):
	public worldSpace	as bool = true		# in world space
	public clearTarget	as bool = true		# clear textures
	public uvChannel	as byte = 0			# tex-coord channel
	public stamp		as double	= -1f	# last update
	public final buf	= kri.buf.Holder( mask:0 )
	public final allowFilter	as bool		# allow results filtering
	
	public Size as uint:
		get: return buf.getInfo().Size * sizeof(single)*4
	public Vert as kri.buf.Texture:
		get: return buf.at.color[0] as kri.buf.Texture
	public Quat as kri.buf.Texture:
		get: return buf.at.color[1] as kri.buf.Texture
	
	public def constructor(w as uint, h as uint, pif as (OpenGL.PixelInternalFormat), filt as bool):
		allowFilter = filt
		for i in range(2):
			buf.mask |= 1<<i
			buf.at.color[i] = t = kri.buf.Texture( intFormat:pif[i] )
			t.filt( allowFilter and not i, false )
		buf.resize(w,h)


#---------	RENDER VERTEX SPATIAL TO UV		--------#

public class Update( kri.rend.tech.Basic ):
	private final bu		= kri.shade.Bundle()
	public	final va		= kri.vb.Array()
	public	final channel	as byte
	
	public def constructor(texId as byte, putId as bool):
		super('bake.surf')
		channel = texId
		# surface shader
		sa = bu.shader
		sa.add( '/uv/bake_v', '/lib/quat_v', '/uv/bake_f' )
		if putId:
			sa.add( '/uv/set/geom_v', '/uv/bake_g' )
		else:	sa.add('/uv/set/norm_v')
		sa.fragout('re_vertex','re_quat')

	public override def process(con as kri.rend.link.Basic) as void:
		scene = kri.Scene.Current
		if not scene:	return
		con.DepthTest = false
		for e in scene.entities:
			tag = e.seTag[of Tag]()
			if not e.visible or not tag:
				continue
			assert tag.uvChannel == 0
			tag.stamp = kri.Ant.Inst.Time
			n = (null,e.node)[tag.worldSpace]
			kri.Ant.Inst.params.spaModel.activate(n)
			tag.buf.bind()
			if tag.clearTarget:
				con.ClearColor( Color4(0f,0f,0f,0f) )
				tag.clearTarget = false
			if not e.render(va,bu):
				kri.lib.Journal.Log("Bake: surface update failed for ${e.node.name}")
				e.tags.Remove(tag)
