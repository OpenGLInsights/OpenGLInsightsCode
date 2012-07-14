namespace support.asm

import System.Collections.Generic
import OpenTK.Graphics.OpenGL


public struct Element:
	# internal state
	public	final	node	as kri.Node
	public	final	mat		as kri.Material
	public	final	range	as Range
	# methods
	public def constructor(n as kri.Node, tm as kri.TagMat):
		node = n
		mat = tm.mat
		range = Range(tm)



public class Scene:
	public	final	conMesh		as Mesh		= null
	public	final	conTex		as Texture	= null
	public	final	mesh	= kri.Mesh( BeginMode.Triangles )
	public	final	texMat	as kri.buf.Texture = null
	public	final	elems	as (Element)	= null
	public	final	lights	= List[of kri.Light]()
	[Getter(Current)]
	internal	static	current	as Scene	= null
	private	numEl	= 0
	
	public def enuMeshes(scene as kri.Scene) as kri.Mesh*:
		mlis = List[of kri.Mesh]()
		for e in scene.entities:
			m = e.mesh
			if m and m not in mlis:
				yield m
				mlis.Add(m)
		
	public def enuTextures(scene as kri.Scene) as kri.buf.Texture*:
		tlis = List[of kri.buf.Texture]()
		for e in scene.entities:
			for tag in e.enuTags[of kri.TagMat]():
				for un in tag.mat.unit:
					t = un.Value
					if t==null or t in tlis:
						continue
					tlis.Add(t)
					yield t
	
	public def constructor(n as int, scene as kri.Scene):
		elems = array[of Element](n)
		square = nv = np = 0
		# gather statistiscs
		for m in enuMeshes(scene):
			nv += m.nVert
			np += m.nPoly * m.polySize
		for t in enuTextures(scene):
			square += t.Area
		size = 1
		for i in range(1,14):
			if (1<<(i+i-1))>square:
				size = 1<<i
				break
		# initialize constructors
		conMesh = Mesh(nv)
		conTex = Texture(size)
		mesh.indexSize = 4
		mesh.buffers.Add( conMesh.data )
		# push data
		for e in scene.entities:
			conMesh.copyData( e.mesh )
		for t in enuTextures(scene):
			conTex.add(t)
		mesh.nVert = conMesh.nVert
		# generate element list
		acc = IndexAccum()
		mesh.ind = acc.ind
		acc.init(np)
		for e in scene.entities:
			for tm in e.enuTags[of kri.TagMat]():
				conMesh.copyIndex( e.mesh, acc, tm, numEl )
				elems[numEl] = Element( e.node, tm )
				numEl += 1
		mesh.nPoly = acc.curNumber / 3
		texMat = acc.tex
