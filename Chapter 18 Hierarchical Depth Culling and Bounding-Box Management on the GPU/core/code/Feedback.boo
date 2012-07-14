namespace kri

import System
import System.Collections.Generic
import OpenTK.Graphics.OpenGL


#-----------#
#	QUERY	#
#-----------#

public class Catcher( IDisposable ):
	public final t	as QueryTarget
	public def constructor(target as QueryTarget, q as Query):
		t = target
		Query.Assign(t,q)
	def IDisposable.Dispose() as void:
		Query.Assign(t,null)


public class Query:
	public	final handle	as int
	private	static	final	State	= Dictionary[of QueryTarget,Query]()
	public	static	def Assign(tg as QueryTarget, val as Query) as void:
		q as Query = null
		State.TryGetValue(tg,q)
		if val:	
			assert not q
			State[tg] = val
			GL.BeginQuery( tg, val.handle )
		elif q:
			State[tg] = null
			GL.EndQuery(tg)
	
	public def constructor():
		tmp = 0
		GL.GenQueries(1,tmp)
		handle = tmp
	def destructor():
		tmp = handle
		Help.safeKill({ GL.DeleteQueries(1,tmp) })
	public def catch(tg as QueryTarget) as IDisposable:
		return Catcher(tg,self)
	public virtual def result() as int:
		rez = 0
		GL.GetQueryObject(handle, GetQueryObjectParam.QueryResult, rez)
		return rez


#-----------------------#
#	TRANSFORM FEEDBACK	#
#-----------------------#

public class CatcherFeed(IDisposable):
	public	static final	target = QueryTarget.TransformFeedbackPrimitivesWritten
	public def constructor(m as BeginFeedbackMode, q as Query):
		GL.BeginTransformFeedback(m)
		if q: Query.Assign(target,q)
	def IDisposable.Dispose() as void:
		Query.Assign(target,null)
		GL.EndTransformFeedback()


public class TransFeedback(Query):
	public			final	mode	as BeginFeedbackMode
	public	static	final	Cache	= array[of vb.Object](8)
	public	static	final	Dummy	= TransFeedback(1)
	public	static	CountPrimitives = false

	public def constructor(nv as byte):
		mode = (BeginFeedbackMode.Points, BeginFeedbackMode.Lines, BeginFeedbackMode.Triangles)[nv-1]
	
	public def catch() as IDisposable:
		return CatcherFeed( mode, (null,self)[CountPrimitives] )
	
	public def discard() as IDisposable:
		return Section( EnableCap.RasterizerDiscard )
	
	public static def Bind(buffers as (vb.Object), offsets as (uint), sizes as (uint)) as bool:
		for i in range( buffers.Length ):
			bf = Cache[i] = buffers[i]
			if not bf:	bf = vb.Object.Zero
			if not bf.Allocated:
				kri.lib.Journal.Log("Feedback: destination buffer is not ready (${bf.handle})")
				return false
			if offsets and sizes:
				bf.bindAsDestination(i, System.IntPtr(offsets[i]), System.IntPtr(sizes[i]) )
			else:
				bf.bindAsDestination(i)
		for i in range( buffers.Length, Cache.Length ):
			Cache[i] = null
		return true
	
	public static def Bind(*buffers as (vb.Object)) as bool:
		return Bind(buffers,null,null)
	
	public override def result() as int:
		return -1	if not CountPrimitives
		return super()
