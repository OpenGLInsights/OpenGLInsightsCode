namespace support.hair

import OpenTK
import OpenTK.Graphics.OpenGL
import kri.shade

#-----------------------------------------------#
#		Hair base attributes baking Render		#
#-----------------------------------------------#


public class Bake( kri.rend.Basic ):
	# face section
	public	final bu_face	= Bundle()
	private	final pWid	= par.Value[of int]('width')
	private	final pVert	= par.Texture('vert')
	private	final pQuat	= par.Texture('quat')
	private	final pInit	= par.Value[of Vector4]('fur_init')
	# vertex section
	public	final bu_vert	= Bundle()
	private	final pStretch	= par.Value[of single]('vertex_ratio')
	private	final tbuf = ( kri.buf.Texture(), kri.buf.Texture() )

	public def constructor(pc as kri.part.Context):
		# init dictionary
		d = par.Dict()
		d.var(pWid)
		d.var(pInit)
		d.var(pStretch)
		d.unit(pVert,pQuat)
		# init shader
		com = Object.Load('/part/fur/base/main_v')
		bu_face.shader.add('/lib/quat_v','/part/fur/base/face_v')
		bu_vert.shader.add('/lib/quat_v','/part/fur/base/vert_v')
		for bu in (bu_face,bu_vert):
			bu.shader.add( com, pc.sh_tool )
			bu.dicts.Add(d)
			bu.shader.feedback(false, 'to_prev','to_base')

	public override def process(con as kri.rend.link.Basic) as void:
		for e in kri.Scene.Current.entities:
			tCur	= e.seTag[of Tag]()
			if not tCur: continue
			assert tCur.Pixels
			pInit.Value = tCur.param
			kri.Ant.Inst.params.activate(e)
			tBake	= e.seTag[of support.bake.surf.Tag]()
			bu as Bundle	= null
			if tBake:	# emit from face
				if tBake.stamp<0f:
					continue
				pWid.Value	= tBake.buf.getInfo().wid
				pVert.Value	= tBake.Vert
				pQuat.Value	= tBake.Quat
				bu = bu_face
			else:		# from vertices
				pStretch.Value = e.mesh.nVert * 1f / tCur.Pixels
				(pVert.Value = tbuf[0]).init( SizedInternalFormat.Rgba32f,	e.findAny('vertex') )
				(pQuat.Value = tbuf[1]).init( SizedInternalFormat.Rgba32f,	e.findAny('quat') )
				bu = bu_vert
			tCur.bake(bu)
