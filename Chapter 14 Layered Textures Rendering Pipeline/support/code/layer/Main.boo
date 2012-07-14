namespace support.layer

import OpenTK.Graphics

#----------------------------------------
#	Layer pass information

public class Pass( kri.meta.Advanced ):
	public prog		as kri.shade.Bundle	= null
	public enable		= true
	public blend		= ''
	public bumpSpace	= ''
	public color		= Color4.White
	public defValue		= 1f
	public doIntensity	= false
	public doInvert		= false
	public doStencil	= false
	
	def System.ICloneable.Clone() as object:
		return copyTo(Pass( prog:prog, enable:enable, blend:blend, bumpSpace:bumpSpace, color:color,
			defValue:defValue, doIntensity:doIntensity, doInvert:doInvert, doStencil:doStencil ))

	def kri.meta.IBase.link(d as kri.shade.par.Dict) as void:
		pass


#----------------------------------------
#	Engine extension - loader

public class Extra( kri.IExtension ):
	def kri.IExtension.attach(nt as kri.load.Native) as void:
		nt.readers['u_apply']	= pu_apply

	#---	Parse unit application	---#
	public def pu_apply(r as kri.load.Reader) as bool:
		m = r.geData[of kri.Material]()
		u = r.geData[of kri.meta.AdUnit]()
		if not (m and u):	return false
		id = m.unit.IndexOf(u)
		pa = Pass( Name:'pass-'+id, Unit:id )
		m.metaList.Add(pa)
		pa.enable		= r.getByte()>0
		pa.blend		= r.getString()
		pa.bumpSpace	= r.getString()
		pa.doIntensity	= r.getByte()>0
		pa.doInvert		= r.getByte()>0
		pa.doStencil	= r.getByte()>0
		pa.color		= r.getColorByte()
		pa.defValue		= r.getReal()
		return true
