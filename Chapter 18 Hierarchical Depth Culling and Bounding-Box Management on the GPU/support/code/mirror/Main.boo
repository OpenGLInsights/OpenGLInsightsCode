namespace support.mirror

import OpenTK.Graphics
import kri.shade


#----------------------------------------
#	Reflection data, stored as an entity tag

public class Meta( kri.meta.Advanced ):
	public final pColor	= par.Value[of Color4]('mirror_color')
	portal Color	as Color4	= pColor.Value
	
	def System.ICloneable.Clone() as object:
		return copyTo( Meta( Color:Color ))
	def kri.meta.IBase.link(d as par.Dict) as void:
		d.var(pColor)


#----------------------------------------
#	Engine extension - loader

public class Extra( kri.IExtension ):
	public final sh_uni	= Object.Load('/mod/mirror_u_f')
	public final sh_tex	= Object.Load('/mod/mirror_t2d_f')
	public final name	= 'mirror'

	def kri.IExtension.attach(nt as kri.load.Native) as void:
		nt.readers['m_mirr']	= pm_mirr
		kri.Ant.Inst.loaders.materials.tarDict['mirror'] =\
			kri.load.ExMaterial.MapTarget(name,sh_tex)

	#---	Material: mirror	---#
	public def pm_mirr(r as kri.load.Reader) as bool:
		mat = r.geData[of kri.Material]()
		return false	if not mat
		col = r.getColorFull()
		mat.metaList.Add(Meta( Name:name, Color:col, Shader:sh_uni ))
		return true


#----------------------------------------
#	Application render

public class Render( kri.rend.tech.Meta ):
	public def constructor():
		super('mirror', false, null, 'mirror')
		shade(('/mat_base_v','/mirror_f'))
	public override def process(con as kri.rend.link.Basic) as void:
		con.activate( con.Target.Same, 0f, false )
		using kri.Blender( kri.Blend.Add ):
			drawScene()
