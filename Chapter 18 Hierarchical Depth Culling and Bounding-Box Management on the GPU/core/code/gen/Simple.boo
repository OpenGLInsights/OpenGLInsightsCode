namespace kri.gen

import OpenTK
import OpenTK.Graphics.OpenGL


#---------	Wrap	---------#

public class Mesh( kri.Mesh ):
	public def constructor(mode as BeginMode):
		super(mode)
	public def constructor(mode as BeginMode, con as Constructor):
		super(mode)
		con.apply(self)
	public def wrap(mat as kri.Material) as kri.Entity:
		ent = kri.Entity(mesh:self)
		tm = kri.TagMat( num:nPoly, mat:mat )
		ent.tags.Add(tm)
		return ent

#---------	Generic Wrap	---------#

public class MeshGen[of T(struct)](Mesh):
	public def constructor(mode as BeginMode, nv as uint, np as uint, data as (T), components as byte):
		super(mode)
		.nVert = nv
		.nPoly = np
		vat = kri.vb.Attrib()
		vat.init(data,false)
		ai = kri.vb.Info( name:'vertex', size:components,
			type:VertexAttribPointerType.HalfFloat )
		vat.Semant.Add(ai)
		buffers.Add(vat)

public class MeshScreen( MeshGen[of Vector2h] ):
	public def constructor(nv as uint, data as (Vector2h)):
		super( BeginMode.TriangleStrip, nv, nv, data, 2 )
		

#---------	TRIANGLE	---------#

public class Triangle( MeshScreen ):
	public def constructor():
		super( 3, (of Vector2h:
			Vector2h(-1f,-1f),	Vector2h(3f,-1f), Vector2h(-1f,3f) 
			))

#---------	QUAD	---------#

public class Quad( MeshScreen ):
	public def constructor():
		super( 4, (of Vector2h:
			Vector2h(-1f,-1f),	Vector2h(1f,-1f),
			Vector2h(-1f,1f),	Vector2h(1f,1f),
			))


#---------	POINT	---------#

public class Point( MeshGen[of Vector4h] ):
	public def constructor():
		super( BeginMode.Points, 1, 1,
			(of Vector4h: Vector4h(0f,0f,0f,1f),),
			4)


#----	LINE OBJECT (-1,1)	----#

public class Line( MeshGen[of Vector4h] ):
	public def constructor():
		super( BeginMode.Lines, 2, 1,
			(of Vector4h: Vector4h(-1f,0f,0f,1f), Vector4h(1f,0f,0f,1f) ),
			4)


#----	PLANE OBJECT	----#
# param: half-size of sides

public class Plane( Mesh ):
	public def constructor(scale as Vector2):
		con = Constructor( v:array[of Vertex](4) )
		sar = (-1f,1f)
		for i in range(4):
			con.v[i].pos = Vector4( scale.X * sar[i&1], scale.Y * sar[i>>1], 0f,1f)
			con.v[i].rot = Quaternion.Identity
		super( BeginMode.TriangleStrip, con )

public class PlaneTex( Mesh ):
	public def constructor(scale as Vector2):
		v = array[of VertexUV](4)
		sar,str = (-1f,1f),(0f,1f)
		for i in range(4):
			v[i].uv = Vector2( str[i&1], str[i>>1] )
			v[i].pos = Vector4( scale.X * sar[i&1], scale.Y * sar[i>>1], 0f,1f)
			v[i].rot = Quaternion.Identity
		# create mesh
		super( BeginMode.TriangleStrip )
		self.nVert = 4
		self.nPoly = 4
		# fill vbo
		vat = kri.vb.Attrib()
		vat.init( v, false )
		# fill semantics
		kri.Help.enrich(vat, 4, 'vertex','quat')
		kri.Help.enrich(vat, 2, 'tex0')
		# return
		buffers.Add(vat)
