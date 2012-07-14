namespace support.stereo

import kri.shade
import OpenTK


public class Split( kri.rend.Basic ):
	public	final	pColor		= par.Texture('color')
	public	final	pDepth		= par.Texture('depth')
	public	focus	= 0.5
	public	final	dict		= par.Dict()
	public	final	pHalfEye	= par.Value[of single]('half_eye')
	public	final	pFocus		= par.Value[of single]('focus_dist')
	public	final	pmLef		= par.Value[of Vector4]('mask_lef')
	public	final	pmRit		= par.Value[of Vector4]('mask_rit')
	public	final	bu			= Bundle()
	private	final	vao			= kri.vb.Array()
	
	public def constructor(halfEye as single):
		pmLef.Value = Vector4(1f,0f,0f,0.5f)
		pmRit.Value = Vector4(0f,1f,1f,0.5f)
		dict.var(pHalfEye,pFocus)
		dict.var(pmLef,pmRit)
		dict.unit(pDepth,pColor)
		pHalfEye.Value = halfEye
		bu.dicts.Add(dict)
		bu.shader.add('/stereo/split_'+s	for s in ('v','g','f'))
		bu.shader.add('/lib/quat_v','/lib/tool_v')
		bu.link()
	
	public def getFocus(cam as kri.Camera) as single:
		return (1f-focus)*cam.rangeIn + focus*cam.rangeOut
	
	public override def process(link as kri.rend.link.Basic) as void:
		pDepth.Value = link.Depth
		pColor.Value = link.Input
		link.activate(true)	# manual depth check in a pixel shader
		link.ClearColor()
		scene = kri.Scene.Current
		if not scene:	return
		pFocus.Value = getFocus( kri.Camera.Current )
		using kri.Blender( kri.Blend.Add ):
			for ent in scene.entities:
				kri.Ant.Inst.params.activate(ent)
				ent.render(vao,bu)
