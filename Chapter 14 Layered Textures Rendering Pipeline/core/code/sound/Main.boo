namespace kri.sound

import OpenTK.Audio.OpenAL


public class Buffer:
	public final handle	as uint
	
	public def constructor():
		handle = AL.GenBuffer()
	def destructor():
		kri.Help.safeKill({ AL.DeleteBuffer(handle) })
	
	public def init(format as ALFormat, data as (byte), rate as int) as void:
		AL.BufferData(handle, format, data, data.Length, rate)


public static class Listener:
	public Position as OpenTK.Vector3:
		set: AL.Listener( ALListener3f.Position, value )
