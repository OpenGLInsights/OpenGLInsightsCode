namespace kri.gen

import System.Math
import OpenTK
import OpenTK.Graphics.OpenGL


#----	CUBE OBJECT	----#
# param: half-size of sides

public class Cube(Mesh):
	public def constructor(scale as Vector3):
		con = Constructor()
		sar = (-1f,1f)
		verts = List[of Vector4]( Vector4(
			scale.X * sar[i&1],
			scale.Y * sar[(i>>1)&1],
			scale.Z * sar[i>>2], 1f)
			for i in range(8)).ToArray()
		#vi = (0,1,4,5,7,1,3,0,2,4,6,7,2,3)	# tri-strip version
		vi = (0,4,5,1, 4,6,7,5, 6,2,3,7, 2,0,1,3, 2,6,4,0, 1,5,7,3)
		ang = 0.5f * PI
		quats = (of Quaternion:
			Quaternion.FromAxisAngle( Vector3.UnitX, ang ),		#-Y
			Quaternion.Identity,								#+Z
			Quaternion.FromAxisAngle( Vector3.UnitX, -ang ),	#+Y
			Quaternion.FromAxisAngle( Vector3.UnitX, ang+ang ),	#-Z
			Quaternion.FromAxisAngle( Vector3.UnitY, -ang ),	#-X
			Quaternion.FromAxisAngle( Vector3.UnitY, ang )		#+X
			)
		con.v = List[of Vertex]( Vertex(verts[vi[i]], quats[i>>2]) for i in range(24)).ToArray()
		offsets = (of ushort: 0,3,2,0,2,1)
		con.i = List[of ushort]( cast(ushort, (i / 6)*4 + offsets[i%6]) for i in range(36)).ToArray()
		super( BeginMode.Triangles, con )



#----	SPHERE OBJECT	----#
# param: radius

public class Sphere(Mesh):	
	private static def Octahedron(scale as Vector3) as Constructor:
		ar = (of Vector3:
			-Vector3.UnitZ, Vector3.UnitX,
			Vector3.UnitY, -Vector3.UnitX,
			-Vector3.UnitY, Vector3.UnitZ)
		vert = List[of Vector4](Vector4( Vector3.Multiply(scale,x),1f ) for x in ar).ToArray()
		# no quaternions needed at this stage
		con = Constructor()
		con.v = List[of Vertex](Vertex(v,Quaternion.Identity) for v in vert).ToArray()
		con.i = (of ushort: 0,1,4, 0,2,1, 0,3,2, 0,4,3, 5,4,1, 5,1,2, 5,2,3, 5,3,4)
		return con
	
	public def constructor(stage as uint, scale as Vector3):
		con = Octahedron(scale)
		# subdivide iterations
		for sub in range(stage):
			con.subDivide()
			# renormalize
			for i in range( con.v.Length ):
				v = con.v[i].pos.Xyz
				v.NormalizeFast()
				con.v[i].pos.Xyz = Vector3.Multiply(scale,v)
		# calculate smooth quaternions
		for i in range( con.v.Length ):
			rv = con.v[i].pos.Xyz
			xyz = rv.LengthFast
			alpha = 0.5f*PI - Asin(rv.Z / xyz)
			xy = rv.Xy.LengthFast
			if xy > 1e-10f:
				beta = Asin(rv.Y / xy)
				if rv.X < 0f: beta = PI-beta
			else: beta = 0f
			con.v[i].rot =\
				Quaternion.FromAxisAngle( Vector3.UnitZ, beta )*\
				Quaternion.FromAxisAngle( Vector3.UnitY, alpha )	
		# finish
		super( BeginMode.Triangles, con )


#----	CONE OBJECT	----#
# param: radius, length
public class Cone(Mesh):	
	public def constructor(num as uint, scale as Vector3):
		super( BeginMode.Triangles )
		.nVert = 2+num
		.nPoly = num*2
		v = array[of VertexNormal](nVert)
		v[0] = VertexNormal( pos:Vector4(Vector3.Multiply(scale,Vector3.Zero),1f), nor:Vector3.UnitZ )
		v[1] = VertexNormal( pos:Vector4(Vector3.Multiply(scale,-Vector3.UnitZ),1f), nor:-Vector3.UnitZ )
		ix = array[of ushort]( 6*num )
		for i in range(num):
			angle = i * 2f * PI / num
			p = Vector3( Sin(angle), Cos(angle), 1f )
			n = Vector3.UnitZ
			v[i+2] = VertexNormal( pos:Vector4(Vector3.Multiply(scale,p),1f), nor:n )
			ix[i*6+0] = ix[i*6+4] = 2 + i
			ix[i*6+2] = ix[i*6+5] = 2 + (i+1)%num
			ix[i*6+1] = 0
			ix[i*6+3] = 1
		vo = kri.vb.Attrib()
		vo.init( v, false )
		vo.Semant.AddRange(( Constructor.InfoVertex, Constructor.InfoNormal ))
		buffers.Add(vo)
		.ind = kri.vb.Object()
		ind.init( ix, false )
