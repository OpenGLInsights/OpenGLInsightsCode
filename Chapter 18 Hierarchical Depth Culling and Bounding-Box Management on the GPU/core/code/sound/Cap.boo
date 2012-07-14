namespace kri.sound

import OpenTK.Audio.OpenAL

#-----------------------------------#
#	SOUND SYSTEM CAPABILITIES		#
#-----------------------------------#

public final class Capabilities:
	public final contextVersion	as string
	public final rendererName	as string
	public final vendorName		as string
	public final extensions		as string
	
	public def constructor():
		contextVersion	= AL.Get( ALGetString.Version )
		rendererName	= AL.Get( ALGetString.Renderer )
		vendorName		= AL.Get( ALGetString.Vendor )
		extensions		= AL.Get( ALGetString.Extensions )
	
	public def getInfo() as string:
		str = ''
		str += "\nContext: ${contextVersion}"
		str += "\nVendor: ${vendorName}"
		str += "\nRenderer: ${rendererName}"
		return str
