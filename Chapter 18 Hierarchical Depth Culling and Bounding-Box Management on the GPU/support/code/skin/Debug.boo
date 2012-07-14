namespace support.skin

public class Debug( kri.rend.Basic ):
	public	final	vbo		= kri.vb.Attrib()
	public	final	frame	as kri.gen.Frame
	public	final	bu		= kri.shade.Bundle()
	
	public def constructor():
		# create mesh
		kri.Help.enrich(vbo,4,'pos','rot','par')
		m = kri.Mesh()
		m.buffers.Add(vbo)
		frame = kri.gen.Frame('skeleton',m)
		# create shader
		bu.shader.add('/skin/debug_v','/skin/debug_g','/color_f','/lib/quat_v','/lib/tool_v')
	
	public override def process(link as kri.rend.link.Basic) as void:
		link.activate(false)
		scene = kri.Scene.Current
		if not scene:	return
		visited = List[of kri.Skeleton]()
		for ent in scene.entities:
			tag = ent.seTag[of Tag]()
			if tag==null or tag.skel==null or tag.skel in visited:
				continue
			visited.Add( tag.skel )
			bones = tag.skel.bones
			frame.mesh.nVert = bones.Length
			# fill data array
			data = array[of OpenTK.Vector4]( bones.Length*3 )
			for i in range(bones.Length):
				bw = bones[i].World
				data[3*i+0] = bw.PackPos
				data[3*i+1] = bw.PackRot
				bw = bones[i].Parent.World
				data[3*i+2] = bw.PackPos
			# upload to mesh
			kri.vb.Array.Bind = frame.va
			vbo.init(data,true)
			frame.draw(bu)
