namespace kri.lib

public class StatBase:
	public	virtual	def frame() as void:
		pass
	public	virtual	def target() as void:
		pass
	public	virtual	def draw(num as uint) as void:
		pass


public class Statistic(StatBase):
	public		nFrame	as uint
	public		nDraw	as uint
	public		nInd	as uint
	protected	tStart	as double
	protected	tPrev	as double
	public		tMax	as double
	public		nTarget	as uint
	
	public def reset() as void:
		nFrame = nDraw = nInd = nTarget = 0
		tStart = tPrev = kri.Ant.Inst.Time
		tMax = 0.0
	
	public virtual def gather() as string:
		time	= kri.Ant.Inst.Time - tStart
		fps		= cast(int, nFrame / time)
		fmax	= cast(int, tMax*1000.0)
		if nFrame:
			cDraw	= nDraw		/ nFrame
			cInd	= nInd		/ nFrame
			cTarg	= nTarget	/ nFrame
		else:
			cDraw = cInd = cTarg = 0
		return "${fps} fps, ${fmax}ms max, ${cDraw} calls, ${cInd} indices, ${cTarg} targets"

	public override	def frame() as void:
		++nFrame
		lag = kri.Ant.Inst.Time - tPrev
		tPrev += lag
		if lag>tMax:	tMax = lag
	
	public override	def target() as void:
		++nTarget
	
	public override	def draw(num as uint) as void:
		++nDraw
		nInd += num


public class StatPeriod(Statistic):
	public	final	title	as string
	public	final	period	as double
	
	public def constructor(name as string, per as double):
		title = name
		period = per
	
	public override def gather() as string:
		if kri.Ant.Inst.Time < tStart+period:
			return null
		rez = super()
		reset()
		return "${title}: ${rez}"
