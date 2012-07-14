namespace kri.meta

import OpenTK
import kri.shade
import System.Collections.Generic

#---	Unit Slave meta data	---#
public class AdUnit( ISlave, par.ValuePure[of kri.buf.Texture] ):
	public final	affects		= Dictionary[of string,single]()
	public isBump	as bool		= false
	public input	as Hermit	= null
	public final	pOffset		= par.ValuePure[of Vector4]()
	public final	pScale		= par.ValuePure[of Vector4]()
	portal Offset	as Vector4	= pOffset.Value
	portal Scale	as Vector4	= pScale.Value
	
	public def constructor():
		pOffset	.Value = Vector4.Zero
		pScale	.Value = Vector4.One
	
	def System.ICloneable.Clone() as object:
		ad = AdUnit( Value:Value, input:input, Offset:Offset, Scale:Scale )
		kri.Help.copyDic(( ad.affects, affects ))
		return ad
	
	def ISlave.link(name as string, d as par.Dict) as void:
		d.unit(name,self)
		d['offset_'	+name] = pOffset
		d['scale_'	+name] = pScale
