namespace kri.ani.data

import System.Collections.Generic

#---------------------
#	KEY: single curve point

public struct Key[of T(struct)]:
	public t	as single	# time moment
	public co	as T		# actual value
	public h1	as T		# bezier handle-left
	public h2	as T		# bezier handle-right
	

public interface IPlayer:
	def touch() as void

public interface IChannel:
	def update(pl as IPlayer, time as single) as bool
	ElemId	as byte:
		get
	Valid	as bool:
		get
	Tag		as string:
		get
		set


#---------------------
#	CHANNEL[T]: generic channel data

public class Channel[of T(struct)](IChannel):
	public final kar	as (Key[of T])
	public final elid	as byte
	# it would be better to use references, but not allowed here
	public final fup	as System.Action[of IPlayer,T,byte]	= null
	public lerp			as System.Func[of T,T,single,T]		= null
	public bezier		as bool	= true
	public extrapolate	as bool	= false
	[Property(Tag)]
	private tag	as string	= ''
	
	IChannel.ElemId	as byte:
		get: return elid
	IChannel.Valid	as bool:
		get: return fup!=null and lerp!=null
		
	public def constructor(num as int, id as byte, f as System.Action[of IPlayer,T,byte]):
		kar = array[of Key[of T]](num)
		elid = id
		fup = f
	
	def IChannel.update(pl as IPlayer, time as single) as bool:
		if not Valid:
			kri.lib.Journal.Log("Animation: Failed to play channel (${tag}) of player (${pl})")
			return false
		fup(pl, moment(time), elid)
		return true
		
	public def moment(time as single) as T:
		i = kar.Length
		assert lerp and i
		while i>0 and kar[i-1].t>time:
			--i
		if i in (0,kar.Length):
			a = kar[ (0,i-1)[i>0] ]
			if extrapolate:
				if i: return lerp(a.co, a.h2, time - a.t)
				else: return lerp(a.co, a.h1, a.t - time)
			return a.co
		a,b = kar[i-1],kar[i]
		t = (time - a.t) / (b.t - a.t)
		if bezier:
			a1 = lerp(a.co, a.h2, t)
			b1 = lerp(b.h1, b.co, t)
			return lerp(a1, b1, t)
		return lerp(a.co, b.co, t)


#---------------------
#	RECORD: complete animation data
#	PLAYER: partial IPlayer implementation

public class Record:
	public final name		as string
	public final length		as single
	public final channels	= List[of IChannel]()
	public def constructor(str as string, t as single):
		name,length = str,t
	public def check() as bool:
		return channels.TrueForAll({ c| return c.Valid })


public abstract class Player(IPlayer):
	public		final anims	= List[of Record]()
	[Getter(Stamp)]
	private		stamp	as uint	= 1

	public def play(name as string) as Anim:
		rec = anims.Find({r| return r.name == name})
		if rec:
			if rec.check():
				return Anim(self,rec)
			kri.lib.Journal.Log("Acton: not supported (${name})")
		return null
	
	public def touch() as void:	#imp: IPlayer
		stamp += 1


#---------------------
#	ANIM: universal animation

public class Anim( kri.ani.Basic ):
	public final player	as IPlayer
	public final record	as Record
	
	public def constructor(pl as IPlayer, rec as Record):
		player,record = pl,rec
	
	def kri.ani.IBase.onTick(time as double) as uint:
		if not record:
			return 2
		if time > record.length:
			return 1
		for c in record.channels:
			if not c.update(player,time):
				return 3
		player.touch()
		return 0
