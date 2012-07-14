namespace support.reflect

import OpenTK.Graphics.OpenGL
import kri.buf

public class Update( kri.rend.Basic ):
	private final buPlane	= Holder()
	private final buCube	= Holder()
	
	public def constructor():
		buPlane.at.depth	= Texture.Depth(0)
		buCube.at.depth	= t	= Texture.Depth(0)
		t.target = TextureTarget.TextureCubeMap
	
	private def drawScene() as void:
		pass	# draw everything!
	
	public override def process(con as kri.rend.link.Basic) as void:
		for ent in kri.Scene.Current.entities:
			tag = ent.seTag[of Tag]()
			continue	if not tag or not tag.counter
			kri.Ant.Inst.params.pCam.spatial.activate( ent.node )
			buf = (buPlane,buCube)[tag.cubic]
			tag.counter -= 1
			if not tag.pTex.Value:
				tag.pTex.Value = Texture()
			buf.mask = 1
			buf.at.color[0] = tag.pTex.Value
			buf.bind()
			con.ClearDepth(1f)
			con.ClearColor()
			drawScene()
