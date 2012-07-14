namespace support.cull.hier

import OpenTK


public class Fill( kri.rend.Basic ):
	public	final	fbo		= kri.buf.Holder(mask:0)
	public	final	buDown	= kri.shade.Bundle()
	private	final	pTex	as kri.shade.par.Texture
	
	public def constructor(con as support.cull.Context):
		pTex = con.pTex
		buDown.dicts.Add( con.dict )
		buDown.shader.add('/copy_v','/cull/down_f')
	public override def process(link as kri.rend.link.Basic) as void:
		fbo.at.depth = t = pTex.Value = link.Depth
		t.setBorder( Graphics.Color4.Black )
		t.shadow(false)
		t.setState(0,false,false)
		kri.gen.Texture.createMipmap(fbo,10,buDown)
		t.switchLevel(0)



public class Apply( kri.rend.Basic ):
	public	final	bu		= kri.shade.Bundle()
	private	final	tf		= kri.TransFeedback(1)
	private	final	frame	as kri.gen.Frame	= null
	private	final	dest	= kri.vb.Object()
	private final	rez		as (int)
	
	public def constructor(con as support.cull.Context):
		bu.dicts.Add( con.dict )
		bu.shader.add( '/cull/check_v', '/lib/quat_v', '/lib/tool_v' )
		bu.shader.feedback(true,'to_visible')
		frame = con.frame
		rez = array[of int]( con.maxn )
		dest.init( con.maxn * 4 )
	
	public override def process(link as kri.rend.link.Basic) as void:
		link.DepthTest = false
		tex = link.Depth
		if not (tex and tex.MipMapped):
			kri.lib.Journal.Log('HierZ: mip chain has not been constructed')
			return
		scene = kri.Scene.Current
		if not scene:	return
		# perform culling
		tex.setBorder( Graphics.Color4.White )
		tf.Bind(dest)
		using tf.discard():
			frame.draw(bu,tf)
		# store the result
		cam = kri.Camera.Current
		dest.read(rez,0)
		for ent in scene.entities:
			tag = ent.seTag[of support.cull.box.Tag]()
			if not (tag and tag.Index>=0):
				continue
			vis = rez[tag.Index] != 0
			ent.frameVisible[cam] = vis
