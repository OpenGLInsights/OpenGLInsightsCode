namespace support.cull.box

import OpenTK.Graphics.OpenGL


###	Draw BBoxes on the screen	###

public class Draw( kri.rend.Basic ):
	public	final	bu		= kri.shade.Bundle()
	private	final	frame	as kri.gen.Frame	= null
	
	public def constructor(con as support.cull.Context):
		sa = bu.shader
		for name in ('quat','tool'):
			text = kri.shade.Code.Read("/lib/${name}_v")
			sa.add( kri.shade.Object( ShaderType.GeometryShader, name, text ))
		sa.add( '/cull/draw_v', '/cull/draw_g', '/white_f' )
		frame = con.frame
	
	public override def process(link as kri.rend.link.Basic) as void:
		if not frame.mesh.Allocated:
			return
		link.activate(false)
		link.SetDepth(0f,false)	#todo: add line offset
		frame.draw(bu)


###	Read the GPU object and update local bounding boxes	###

public class Update( kri.rend.Basic ):
	private final rez		as (single)
	private final data		as kri.vb.Object
	
	public def constructor(ct as support.cull.Context):
		rez = array[of single]( ct.maxn*4*2 )
		data = ct.bound
		
	public override def process(link as kri.rend.link.Basic) as void:
		scene = kri.Scene.Current
		if not scene:	return
		data.read(rez,0)
		# update local boxes
		for e in scene.entities:
			tag = e.seTag[of Tag]()
			if not (tag and tag.fresh):
				continue
			tag.fresh = false
			i = 2*4 * tag.Index
			if i<0: continue
			v0 = OpenTK.Vector3(rez[i+0],rez[i+1],rez[i+2])
			v1 = OpenTK.Vector3(rez[i+4],rez[i+5],rez[i+6])
			e.localBox.center = 0.5f*(v0-v1)
			e.localBox.hsize = -0.5f*(v0+v1)
