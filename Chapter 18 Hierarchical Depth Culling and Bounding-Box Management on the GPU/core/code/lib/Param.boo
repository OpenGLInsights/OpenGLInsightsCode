namespace kri.lib

import OpenTK
import kri.shade


# Shader Parameter Library
public final class Param:
	public	final	spaModel	= par.spa.Shared('s_model')	# object->world()
	public	final	spaView		= par.spa.Shared('s_view')	# object->camera()
	public	final	modelBox	= par.Box('bb_model')
	public	final	light		= par.Light()
	public	final	pLit		= par.Project('lit')	# light->world, projection
	public	final	pCam		= par.Project('cam')	# camera->world, projection
	public	final	parSize		= par.Value[of Vector4]('screen_size')	# viewport size
	public	final	parTime		= par.Value[of Vector4]('cur_time')		# task time & delta
	
	public def activate(e as kri.Entity) as void:
		sm = kri.Node.SafeWorld( e.node )
		sc = kri.Node.SafeWorld( pCam.spatial.extract() )
		sc.inverse()
		sv = kri.Spatial.Combine(sm,sc)
		spaModel	.activate(sm)
		spaView		.activate(sv)
		modelBox	.activate( e.localBox )

	public def activate(l as kri.Light) as void:
		light	.activate(l)
		pLit	.activate(l)
	public def activate(c as kri.Camera) as void:
		kri.Camera.Current = c
		pCam	.activate(c)
	public def activate(pl as kri.buf.Plane) as void:
		parSize.Value = Vector4( 1f*pl.wid, 1f*pl.het, 0.5f*(pl.wid+pl.het), 0f)

	public def constructor(d as par.Dict):
		for me in (of kri.meta.IBase: spaModel,spaView,modelBox,light,pLit,pCam):
			me.link(d)
		d.var(parSize,parTime)
