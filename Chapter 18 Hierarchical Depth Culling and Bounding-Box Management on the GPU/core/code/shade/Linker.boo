namespace kri.shade

import System.Collections.Generic

#-------------------------------#
#	SHADER LINKER CHACHE		#
#-------------------------------#

public class Linker:
	public final	samap = Dictionary[of string,Bundle]()
	public final	onLink	as callable(Mega)	= null
	
	public def constructor(fun as callable(Mega)):
		onLink = fun
	
	public def link(sl as Object*, *dc as (par.Dict)) as Bundle:
		key = join(x.uniqueId.ToString() for x in sl, ',')
		bu as Bundle = null
		if samap.TryGetValue(key,bu):
			bu = Bundle(bu)
			bu.dicts.AddRange(dc)
			bu.fillParams()
			# yes, we will just fill the parameters for this program ID again
			# it's not obvious, but texture units will be assigned to the old values,
			# because the meta-data sets already matched (kri.load.meta.MakeTexCoords)
		else:
			bu = Bundle()
			bu.shader.add( *List[of Object](sl).ToArray() )
			bu.dicts.AddRange(dc)
			if onLink:
				onLink( bu.shader )
			samap.Add(key,bu)
		return bu
