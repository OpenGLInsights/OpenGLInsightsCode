namespace kri

import System
import System.Collections.Generic
import OpenTK.Graphics.OpenGL


#--------- Mesh ---------#

public class Mesh( vb.Storage ):
	public	nVert	as uint	= 0
	public	nPoly	as uint	= 0
	public	ind		as vb.Object	= null
	public	indexSize	= 2
	public	final drawMode	as BeginMode
	public	final polySize	as uint
	public	final blockList	= List[of shade.Bundle]()
	public	NumElements as uint:
		get: return (nVert,nPoly)[ind!=null]
	
	public def constructor():
		self( BeginMode.Points )
	
	public def constructor(dmode as BeginMode):
		drawMode,polySize = dmode,0
		polySize = 1	if dmode in (BeginMode.Points, BeginMode.LineStrip, BeginMode.LineLoop,
			BeginMode.TriangleStrip, BeginMode.TriangleFan, BeginMode.QuadStrip)
		polySize = 2	if dmode == BeginMode.Lines
		polySize = 3	if dmode == BeginMode.Triangles
		polySize = 4	if dmode == BeginMode.Quads
	
	public def constructor(m as Mesh):
		nVert	= m.nVert
		nPoly	= m.nPoly
		ind		= m.ind
		drawMode	= m.drawMode
		polySize	= m.polySize
		buffers.AddRange( m.buffers )
	
	public def allocate() as void:
		for v in buffers:
			v.initUnit(nVert)
		assert not ind
	
	public def getTotalSize() as uint:
		rez = 0
		for v in buffers:
			rez += v.countSize()
		return rez * nVert
	
	#---	render functions	---#
	
	public def render(vao as vb.Array, bu as shade.Bundle, dict as vb.Dict, off as uint, num as uint, nob as uint, tf as TransFeedback) as bool:
		if bu in blockList:	return
		sa = bu.shader
		if not bu.pushAttribs(ind,vao,dict):
			kri.lib.Journal.Log("Render: failed to load mesh (v=${nVert},p=${nPoly}) with shader ${sa.handle}")
			blockList.Add(bu)
			return false
		if not bu.activate():
			kri.lib.Journal.Log("Render: failed to activate shader (${sa.handle})")
			blockList.Add(bu)
			return false
		actualMode = (drawMode,BeginMode.Points)[tf!=null]
		if not sa.isCompatible(actualMode):
			kri.lib.Journal.Log("Render: incompatible primitive type (${actualMode}, expected ${sa.GeometryInput} of ${sa.handle})")
			blockList.Add(bu)
			return false
		rez = true
		if tf == TransFeedback.Dummy:
			rez = draw(null)
		elif tf:
			if kri.Ant.Inst.debug and vao.hasConflicts():
				kri.lib.Journal.Log("Transform Feedback: loop detected on shader ${sa.handle}")
				blockList.Add(bu)
				return false
			rez = draw(tf)
		elif nob>0:
			rez = draw(off,num,nob)
		if not rez:
			kri.lib.Journal.Log("Render: mesh limitations check failed (v=${nVert},p=${nPoly})")
			return false
		return true
	
	public def render(vao as vb.Array, bu as shade.Bundle, dict as vb.Dict, nob as uint, tf as TransFeedback) as bool:
		return render(vao,bu,dict,0,NumElements,nob,tf)
	
	public def render(vao as vb.Array, bu as shade.Bundle, tf as TransFeedback) as bool:
		return render( vao, bu, vb.Dict(self), 1, tf)
	
	#---	internal drawing functions	---#
	
	public def getIndexType() as DrawElementsType:
		return (
			DrawElementsType.UnsignedByte,
			DrawElementsType.UnsignedShort,
			cast(DrawElementsType,0),
			DrawElementsType.UnsignedInt
			)[indexSize-1]
	
	protected def draw(off as uint, num as uint, nob as uint) as bool:
		caps = kri.Ant.Inst.caps
		if ind:
			if num+off>nPoly or num*polySize > caps.elemIndices:
				return false
		elif num+off>nVert:
			return false
		if nVert>caps.elemVertices:
			return false
		elType = getIndexType()
		ptr = IntPtr(polySize*indexSize*off)
		if ind and nob != 1:
			GL.DrawElementsInstanced(	drawMode, polySize*num, elType, ptr, nob)
		elif ind:	GL.DrawElements (	drawMode, polySize*num, elType, ptr)
		elif nob != 1:
			GL.DrawArraysInstanced(		drawMode, polySize*off, polySize*num, nob)
		else:	GL.DrawArrays(			drawMode, polySize*off, polySize*num )
		kri.Ant.Inst.stats.draw( polySize*num*nob )
		return true

	# draw all polygons once
	protected def draw(nob as uint) as bool:
		return draw(0,NumElements,nob)
	# draw all points
	private def draw() as bool:
		if nVert>kri.Ant.Inst.caps.elemVertices:
			return false
		GL.DrawArrays( BeginMode.Points, 0, nVert )
		kri.Ant.Inst.stats.draw(nVert)
		return true
	# transform points with feedback
	protected def draw(tf as TransFeedback) as bool:
		if not tf:
			return draw()
		using tf.catch():
			draw()
		nr = tf.result()
		if nr>=0 and nr != nVert:
			kri.lib.Journal.Log("TransFeedback: bad result size (${nr}) for input (${nVert})")
			return false
		return true


#--------- Tag ---------#

public interface ITag:
	pass

public interface ITagData(ITag):
	Data as object:
		get
	
public class TagMat(ITagData):
	public off	as uint	= 0
	public num	as uint	= 0
	public mat	as Material	= null
	
	ITagData.Data as object:
		get: return mat


#--------- Entity ---------#

public interface IMeshed:
	Mesh as Mesh:
		get

public interface INamed:
	Name as string:
		get

public class Entity( kri.ani.data.Player, INoded, IMeshed, INamed ):
	public node		as Node	= null
	public mesh		as Mesh	= null
	public visible	as bool	= true
	public final frameVisible	= Dictionary[of Projector,bool]()
	public localBox			= Box(1f)
	public final store		= vb.Storage()
	public final techVar	= Dictionary[of string,vb.Array]()
	public final tags		= List[of ITag]()
	
	INoded.Node as Node:
		get: return node
	IMeshed.Mesh as Mesh:
		get: return mesh
	INamed.Name as string:
		get: return (node.name	if node else	'')
	
	public Visible[p as Projector] as bool:
		get:
			rez = false
			return rez	if not p
			tr = not frameVisible.TryGetValue(p,rez)
			return visible and (tr or rez)
	public VisibleCam as bool:
		get: return Visible[Camera.Current]
	public CombinedAttribs as vb.Dict:
		get: return vb.Dict(mesh,store)
	
	public def constructor():
		pass
	public def constructor(e as Entity):
		mesh = e.mesh
		visible = e.visible
		tags.AddRange( e.tags )
	
	public def seTag[of T(ITag)]() as T:
		for it in tags:
			t = it as T
			return t	if t
		return null as T
		#return tags.Find( {t| return t isa T} ) as T
	
	public def enuTags[of T(ITag)]() as (T):
		tlis = List[of T]()
		for it in tags:
			t = it as T
			tlis.Add(t)	if t
		return tlis.ToArray()
	
	public def findAny(name as string) as kri.vb.Attrib:
		at = store.find(name)
		return (at	if at else	mesh.find(name))
	
	# returns null if entity does't have all attributes requested
	# otherwise - a list of rejected materials
	public def check(name as string) as kri.Material*:
		vac as vb.Array = null
		if not (techVar.TryGetValue(name,vac) and vac):
			return null
		ml = List[of kri.Material]()
		for t in tags:
			tm = t as TagMat
			if not tm:
				continue	
			m = tm.mat
			bu as shade.Bundle = null
			m.tech.TryGetValue(name,bu)
			if not bu:	ml.Add(m)
		return ml
	
	public def render(vao as vb.Array, bu as shade.Bundle, tf as kri.TransFeedback) as bool:
		assert mesh and store
		return mesh.render( vao,bu, CombinedAttribs, 1,tf )
	public def render(vao as vb.Array, bu as shade.Bundle) as bool:
		return render(vao,bu,null)

	# removes animated vertex buffers that present in a mesh
	public def deleteOwnData() as void:
		store.buffers.RemoveAll() do(vat as kri.vb.Attrib):
			for ai in vat.Semant:
				if not mesh.find(ai.name):
					return false
			return true
