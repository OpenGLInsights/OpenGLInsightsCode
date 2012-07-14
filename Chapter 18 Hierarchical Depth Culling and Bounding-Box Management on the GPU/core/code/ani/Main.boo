namespace kri.ani

# Animation Interface
public interface IBase:
	def onTick(time as double) as uint
	def onKill() as void

public class Basic(IBase):
	def IBase.onKill() as void:
		pass
	def IBase.onTick(time as double) as uint:
		return 0


public class Action(Basic):
	protected virtual def execute() as void:
		pass
	def IBase.onTick(time as double) as uint:
		execute()
		return 1


public class Delta(Basic):
	private last	= 0.0
	protected abstract def onDelta(delta as double) as uint:
		pass
	def IBase.onTick(time as double) as uint:
		d = time - last
		last += d
		return onDelta(d)


public class Loop(Basic):
	public lTime	as double = 1.0
	private start	= double.MinValue
	[getter(Loops)]
	private loops	as uint = 1
	protected virtual def onLoop() as void:
		pass
	protected virtual def onRate(rate as double) as void:
		pass
	def IBase.onTick(time as double) as uint:
		if time >= start+lTime:
			if not loops:
				onRate(1.0)
				return 1
			--loops
			start = time
			onLoop()
		else: onRate((time-start) / lTime)
		return 0
