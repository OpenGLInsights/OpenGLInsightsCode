namespace kri.lib.par

import System
import OpenTK
import OpenTK.Graphics
import kri.shade
import kri.meta


# box
public final class Box( IBase ):
	public final name	as string
	public final center	as par.Value[of Vector4]
	public final hsize	as par.Value[of Vector4]
	
	public def constructor(str as string):
		name = str
		center	= par.Value[of Vector4](name+'.center')
		hsize	= par.Value[of Vector4](name+'.hsize')
	
	public def activate(ref b as kri.Box) as void:
		center	.Value = Vector4(b.center,1f)
		hsize	.Value = Vector4(b.hsize,0f)
	
	kri.INamed.Name as string:
		get: return name
	def IBase.link(d as par.Dict) as void:
		d.var(center,hsize)


# light settings
public final class Light( IBase ):
	public final color	= par.Value[of Color4]('lit_color')
	public final attenu	= par.Value[of Vector4]('lit_attenu')
	public final data	= par.Value[of Vector4]('lit_data')

	public def activate(l as kri.Light) as void:
		color.Value		= l.Color
		kdir = (0f,1f)[l.fov>0f]
		attenu.Value	= Vector4(l.factor0, l.factor1, l.factor2, l.sphere)
		data.Value		= Vector4(l.softness, kdir, 0f, 0f)

	kri.INamed.Name as string:
		get: return 'Light'
	def IBase.link(d as par.Dict) as void:
		d.var(color)
		d.var(attenu,data)

# complete projector
public final class Project( IBase ):
	public	final	project	as proj.Shared
	public	final	spatial	as spa.Linked
	
	public def constructor(name as string):
		project = proj.Shared(name)
		spatial = spa.Linked('s_' + name)
	
	public def activate(p as kri.Projector) as void:
		project.activate( p )
		spatial.activate(( p.node if p else null ))
	
	kri.INamed.Name as string:
		get: return project.Name
	def IBase.link(d as par.Dict) as void:
		for ib in (of IBase: project,spatial):
			ib.link(d)
