namespace kri.meta

import System
import OpenTK
import kri.shade


#---	Map Input : OBJECT		---#
public class InputObject(Hermit):
	public final pNode	= kri.lib.par.spa.Linked('s_target')
	#don't inherit as the name is different
	def ICloneable.Clone() as object:
		ib = InputObject()
		ib.pNode.activate( pNode.extract() )
		return copyTo(ib)
	def IBase.link(d as par.Dict) as void:
		(pNode as IBase).link(d)


#---	real value meta-data	---#
public class Data[of T(struct)]( Advanced ):
	private final data	as par.Value[of T]
	portal Value	as T	= data.Value
	
	public def constructor(s as string, sh as Object, val as T):
		data = par.Value[of T]( 'mat_'+s )
		Name = s
		Shader = sh
		Value = val
	def ICloneable.Clone() as object:
		d2 = Data[of T](Name,Shader,Value)
		d2.Unit = Unit
		return d2
	def IBase.link(d as par.Dict) as void:
		d.var(data)


#---	strand		---#
public class Strand(Advanced):
	# X = base thickness: [0,], Y = tip thickness: [0,], Z = shape: (-1,1)
	private final pData	= par.Value[of Vector4]('strand_data')
	portal Data		as Vector4	= pData.Value
	
	def ICloneable.Clone() as object:
		return copyTo( Strand( Data:Data ))
	def IBase.link(d as par.Dict) as void:
		d.var(pData)


#---	halo		---#
public class Halo(Advanced):
	# X = size, Y = hardness, Z = add
	private final pData	= par.Value[of Vector4]('halo_data')
	portal Data		as Vector4	= pData.Value
	
	def ICloneable.Clone() as object:
		return copyTo( Halo( Data:Data ))
	def IBase.link(d as par.Dict) as void:
		d.var(pData)


#---	environment mirror		---#
public class Mirror( Data[of Vector4] ):
	public cube	as kri.buf.Texture	= null
	public def constructor():
		super( 'mirror', null, Vector4.Zero )
