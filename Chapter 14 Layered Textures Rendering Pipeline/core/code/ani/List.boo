namespace kri.ani

import System

###		Animation list		###

public class Scheduler(IBase):
	protected final anims	= List[of XAniData]()
	public Empty as bool:
		get: return not anims.Count
		set:
			anims.Clear()	if value
	public def isPlaying(an as IBase) as bool:
		for xa in anims:
			if xa.an == an:
				return true	
		return false

	# data
	public struct XAniData:
		public an	as IBase
		public rec	as callable(int)
		public tik	as double
		public tag	as int
		public def onTime(time as double) as uint:
			if an.onTick(time-tik):
				rec(tag)	if rec
				return 1
			return 0
	# methods
	public def add(an as IBase, rec as callable(int), tag as int) as void:
		anims.Add( XAniData(an:an, rec:rec, tik:kri.Ant.Inst.Time, tag:tag) )
	public def add(an as IBase) as void:
		add(an,null,0)
	public def remove(an as IBase) as bool:
		anims.RemoveAll() do(ref xan as XAniData):
			return xan.an == an
		return true
	public def clear() as void:
		for xan in anims:
			xan.an.onKill()
		anims.Clear()

	def IBase.onTick(time as double) as uint:		#imp: IAnimation
		anims.RemoveAll() do(ref xan as XAniData):
			return xan.onTime(time) != 0
		return 0
		#return (0,1)[not anims.Count]
	
	def IBase.onKill() as void:
		clear()