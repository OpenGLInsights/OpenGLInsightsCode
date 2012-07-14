namespace kri.load.sound

import OpenTK.Audio.OpenAL

#------		BASIC WAVE 		------#

public class Basic( kri.data.IGenerator[of kri.sound.Buffer] ):
	public final channels	as int
	public final bits		as int
	public final rate		as int
	public final scan		as (byte)
	
	public def constructor(c as int, b as int, r as int, data as (byte)):
		channels,bits,rate = c,b,r
		scan = data
	
	public def getFormat() as ALFormat:
		assert bits in (8,16) and 'unknow bits number'
		assert channels in (1,2) and 'unknow channels number'
		return ((ALFormat.Mono8,ALFormat.Mono16),
			(ALFormat.Stereo8,ALFormat.Stereo16))[channels>1][bits>8]

	public def generate() as kri.sound.Buffer:	#imp: kri.res.IGenerator
		buf = kri.sound.Buffer()
		buf.init( getFormat(), scan,rate )
		return buf