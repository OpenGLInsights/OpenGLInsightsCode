namespace kri.sound

import OpenTK.Audio.OpenAL


public class Source:
	public final handle		as uint
	public	node	as kri.Node	= null
	
	public def constructor():
		handle = AL.GenSource()
	def destructor():
		kri.Help.safeKill({ AL.DeleteSource(handle) })
	
	public Relative as bool:
		set: AL.Source( handle, ALSourceb.SourceRelative, value )
	public Looping as bool:
		set: AL.Source( handle, ALSourceb.Looping, value )
	public Streaming as bool:
		set:
			type = (ALSourceType.Static, ALSourceType.Streaming)[value]
			AL.Source( handle, ALSourcei.SourceType, cast(int,type) )
	
	public Position as OpenTK.Vector3:
		set: AL.Source( handle, ALSource3f.Position, value )
	public Velocity as OpenTK.Vector3:
		set: AL.Source( handle, ALSource3f.Velocity, value )
	public Direction as OpenTK.Vector3:
		set: AL.Source( handle, ALSource3f.Direction, value )
	public SecOffset as single:
		set: AL.Source( handle, ALSourcef.SecOffset, value )

	public def init(buf as Buffer) as void:
		AL.Source( handle, ALSourcei.Buffer, buf.handle )
	public def play() as void:
		AL.SourcePlay(handle)
	public def stop() as void:
		AL.SourceStop(handle)
