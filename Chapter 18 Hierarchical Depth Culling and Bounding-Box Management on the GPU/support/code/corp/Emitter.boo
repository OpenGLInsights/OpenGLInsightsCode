namespace support.corp

public enum State:
	Unknown
	Filled
	Invalid


public class Emitter( kri.ITag ):
	public	state		= State.Unknown
	public	onUpdate	as System.Func[of kri.Entity,bool]	= null

	[Getter(Dirty)]
	private dirty		as bool		= false
	private	curTime		as double	= 0.0
	public	CurTime		as double:
		get: return curTime
		set:
			dirty = true
			curTime = value
	
	public	final entity	as kri.Entity	= null
	public	final owner		as Manager		= null
	public	final name		as string		= null
	public	final entries	= kri.vb.Dict()

	public def constructor(ent as kri.Entity, pm as Manager, str as string):
		entity = ent
		owner = pm
		name = str
	public def constructor(pe as Emitter):
		entity	= pe.entity
		owner	= pe.owner
		name	= pe.name

	public def update() as bool:
		return (not onUpdate) or onUpdate(entity)
