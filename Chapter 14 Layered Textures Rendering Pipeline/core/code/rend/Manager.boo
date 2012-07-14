namespace kri.rend

import System
import System.Collections.Generic


private class Job:
	private final	difficulty	as int		= 1
	public	final	onGPU		as bool		= true
	public	final	rend		as Basic	= null
	public	final	deps		= List[of Job]()
	
	public	Active	as bool:
		get:	return rend!=null and rend.active
	public	Diff	as int:
		get:	return (0,difficulty)[Active]
		
	public def constructor(d as int, r as Basic):
		difficulty = Math.Max(0,d)
		rend = r


public class Manager(Profile):
	private	final	jall	= Dictionary[of string,Job]()
	private final	ln		= link.Buffer(0,0,0)
	private	reverse			= false
	private final	static MAX	= 100
	
	public	Renders	as Basic*:
		get:
			for job in jall.Values:
				if job.rend:	yield job.rend
	
	public def put(name as string, dif as int, r as Basic, *deps as (string)) as void:
		assert not name in jall	# ensures no cycles in the dependency graph
		jall[name] = j = Job(dif,r)
		for d in deps:
			dj as Job = null
			if jall.TryGetValue(d,dj):
				j.deps.Add(dj)
			else:
				kri.lib.Journal.Log("RMan: dependency (${d}) not found")
	
	public override def setup(pl as kri.buf.Plane) as bool:
		ln.resize(pl)
		return List[of Job](jall.Values).TrueForAll() do(j as Job):
			return j.rend==null or j.rend.setup(pl)
	
	public override def process(con as link.Basic) as void:
		jord = array[of Job](jall.Count)
		jall.Values.CopyTo(jord,0)
		total = 0
		for j in jord:
			total += j.Diff
		assert jord.Length <= MAX
		# aux func
		def getScore() as int:	# O(n^3)
			mint,sum = total,0
			for i in range(jord.Length):
				rd = -1
				for d in jord[i].deps:
					ind = Array.IndexOf(jord,d)
					assert ind >= 0 and ind != i
					rd = Math.Max(ind,rd)
				if rd<0:	continue
				cur,step = 0,(-1,1)[rd<i]
				rd += (step+1)>>1
				for k in range(rd,i,step):
					cur += step*jord[k].Diff
				if cur<mint:	mint = cur
				sum += cur
			return mint*MAX*MAX + sum
		def swap(i as int) as void:	# O(1)
			j = jord[i-1]
			jord[i-1] = jord[i]
			jord[i] = j
		# main proc
		score = array[of int]( jord.Length )
		score[0] = getScore()
		while true:	# O(n^6) !!!
			md = 0
			for i in range(1, jord.Length):
				swap(i)
				score[i] = getScore()
				if score[i]>score[md]:	md=i
				swap(i)
			if not md:	break
			swap(md)
			score[0] = score[md]
		assert score[0] >= 0
		# run
		if reverse:
			Array.Reverse(jord)
		execute(ln, (j.rend for j in jord))
		rCopy.process(ln,con)
