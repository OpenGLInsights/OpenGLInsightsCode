namespace support.stereo

import OpenTK
import kri.shade


public class Merger( kri.rend.Basic ):
	public	final pmLef		= par.Value[of Vector4]('mask_lef')
	public	final pmRit		= par.Value[of Vector4]('mask_rit')
	public	final texLef	= par.Texture('lef')
	public	final texRit	= par.Texture('rit')
	public	final bu		= Bundle()
	
	public def constructor():
		# shader
		d = par.Dict()
		d.var(pmLef,pmRit)
		d.unit(texLef,texRit)
		bu.dicts.Add(d)
		bu.shader.add('/copy_v','/stereo/merge_f')
		# masks
		pmLef.Value	= Vector4(1f,0f,0f,0.5f)
		pmRit.Value	= Vector4(0f,1f,1f,0.5f)
		
	public override def process(link as kri.rend.link.Basic) as void:
		link.activate(false)
		kri.Ant.Inst.quad.draw(bu)



public class Proxy( kri.IView ):
	public	final	rMerge	= Merger()
	public	final	view	as kri.View
	public	final	xv		as Vector3
	private	final	linkBuf		= kri.rend.link.Buffer(0,0,0)
	private	final	linkScreen	= kri.rend.link.Screen()
	private	final	nEye		= kri.Node('eye')
	
	public def constructor(v as kri.View, shift as single, focus as single):
		//reference: paulbourke.net/texture_colour/anaglyph/
		view = v
		assert v
		xv = Vector3(shift, 0f, -1f)
	
	public def setEye(eye as int, pt as par.Texture) as void:
		proPlane = 1f
		halfEye = 0.01f
		c = view.cam	# unmodified camera
		c.offset.X = 0f
		vin = xv
		#off = c.unproject(vin)
		#c.offset.X = eye * xv.X
		#nEye.local.pos.X = eye * 0.1f
		nEye.local.pos.X = eye * halfEye
		vin = Vector3( halfEye, 0f, -proPlane )
		c.offset.X = eye * c.project(vin).X
		nEye.Parent = c.node
		c.node = nEye
		kri.Ant.Inst.params.activate(c)
		# render
		linkBuf.activate(true)
		view.ren.process(linkBuf)
		pt.Value = linkBuf.Input
		# cleanup
		c.node = nEye.Parent
		c.offset = Vector3.Zero
		
	def kri.IView.update() as void:
		c = view.cam
		if not (c and view.ren and view.ren.active):
			(view as kri.IView).update()
			return
		# prepare
		kri.Scene.Current = view.scene
		view.cam.aspect = linkBuf.Frame.getInfo().Aspect
		# render eyes
		setEye(0-1, rMerge.texLef )	# left
		setEye(0+1, rMerge.texRit )	# right
		# merge
		rMerge.process(linkScreen)
		kri.Scene.Current = null
	
	def kri.IView.resize(wid as int, het as int) as bool:
		pl = linkScreen.screen.plane
		pl.wid = wid
		pl.het = het
		linkBuf.resize(pl)
		return (view as kri.IView).resize(wid,het)
