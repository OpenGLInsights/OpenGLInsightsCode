namespace kri.rend

import System.Collections.Generic
import OpenTK.Graphics.OpenGL

#---	QUERY POOL		---#

public class QueryPool:
	private	final list	= List[of kri.Query]()
	public def push(qs as kri.Query*) as void:
		list.AddRange(qs)
	public def fetch() as kri.Query:
		if list.Count:
			q = list[0]
			list.RemoveAt(0)
			return q
		return kri.Query()


#---	PROFILE RENDER		---#

public abstract class Profile(Basic):
	public		doProfile		as bool	= false
	private		final	dpro	= Dictionary[of Basic,kri.Query]()
	private		final	qpool	= QueryPool()
	protected	final	rCopy	= Copy()
	
	public	def execute(ln as link.Basic, rlis as Basic*) as void:
		qpool.push( dpro.Values )
		dpro.Clear()
		for r in rlis:
			if not (r and r.active):	continue
			kri.vb.Array.Bind = null
			if doProfile:
				dpro[r] = q = qpool.fetch()
				using q.catch( QueryTarget.TimeElapsed ):
					r.process(ln)
			else:	r.process(ln)
	
	public def genReport() as string:
		rez = 'Profile report:'
		for p in dpro:
			r = p.Key
			if not (r and r.active):
				continue
			name = r.ToString()
			time = p.Value.result()
			rez += "\n${name}: ${time}"
		return rez



#---------	RENDER CHAIN MANAGER	--------#

public class Chain(Profile):
	public	final	renders	= List[of Basic]()		# *Render
	public	final	ln		as link.Buffer
	
	public def constructor(ns as byte, bc as byte, bd as byte):
		ln = link.Buffer(ns,bc,bd)

	public def constructor():
		self(0,0,0)

	public override def setup(pl as kri.buf.Plane) as bool:
		ln.resize(pl)
		return renders.TrueForAll() do(r as Basic):
			return r.setup(pl)
		
	public override def process(con as link.Basic) as void:
		if not ln.Ready:
			return
		execute(ln,renders)
		rCopy.process(ln,con)
