namespace kri.ani

import System
import System.Collections.Generic

#-----------------------#
#	Animation Graph		#
#-----------------------#

public class Graph(IBase):
	public class AgNode:
		internal tik	as double	= -1.0
		public anim		as IBase	= null
		public bWaitAll	as bool		= true
		public bEmitAll	as bool		= true
		public final to	= List[of AgNode]()
		public onStart	as callable	= null
		public onFinish	as callable	= null
		public Running as bool:
			get: return tik >= 0.0
		public def append(an as IBase) as AgNode:
			nod = AgNode(anim: an)
			to.Add(nod)
			return nod
	
	public final init	= AgNode()
	private final alist	= List[of AgNode]()
	
	public def constructor():
		alist.Add(init)
	public def stop() as void:
		for a in alist:
			a.to.Clear()
		alist.Clear()
	public def restart() as void:
		stop()
		alist.Add(init)

	def IBase.onTick(time as double) as uint:
		return 1	if alist.Count == 0
		tmp = List[of AgNode]()
		def add(t as AgNode) as void:
			tmp.Add(t)	if not t in tmp
		# separate candidates
		alist.RemoveAll() do(a as AgNode):
			id = (a.anim.onTick(time - a.tik) if a.anim else 1)
			return false	if not id
			a.onFinish()	if a.onFinish
			a.tik = -1.0
			if a.bEmitAll:
				for t in a.to: add(t)
			else: add(a.to[id-1])
			return true
		return 0	if not tmp.Count
		# remove those can't be started
		for an in alist:
			for d in an.to:
				if an.Running and d.bWaitAll:
					tmp.Remove(d)
		# start others	
		for t in tmp:
			t.tik = kri.Ant.Inst.Time
			alist.Add(t)
			t.onStart()	if t.onStart
		return 0
	
	def IBase.onKill() as void:
		for an in alist:
			if an.anim:
				an.anim.onKill()
		alist.Clear()
