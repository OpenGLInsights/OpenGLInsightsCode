namespace kri.gen

import OpenTK
import OpenTK.Graphics.OpenGL


#----	LANDSCAPE	----#
# param: height map

public class Landscape(Mesh):
	public def constructor(hm as (single,2), scale as Vector3):
		con = Constructor()
		con.v = array[of Vertex]( len(hm) )
		dx = len(hm,0)
		dy = len(hm,1)
		assert dx and dy
		
		def hv(x as int,y as int):
			z = 0f
			if x>=0 and x<dx and y>=0 and y<dy:
				z = hm[x,y]
			return Vector3(x,y,z)
		
		center = Vector3( (dx-1)*0.5f, (dy-1)*0.5f, 0f )
		for y in range(dy):
			for x in range(dx):
				pos = hv(x,y) - center
				id = y*dx + x
				con.v[id].pos = Vector4( Vector3.Multiply(scale,pos), 1f)
				normal = Vector3.Divide( Vector3.Cross(
					hv(x+1,y) - hv(x-1,y),
					hv(x,y+1) - hv(x,y-1)),
					scale)
				normal.Normalize()
				nz = Vector3.Cross( Vector3.UnitZ, normal )
				con.v[id].rot = Quaternion.Normalize( Quaternion(
					nz* (1f - Vector3.Dot(normal,Vector3.UnitZ)),
					nz.LengthFast ))
		
		con.i = array[of ushort]( (dy-1) * (2*dx+1) )
		for y in range( dy-1 ):
			id = y*( 2*dx+1 )
			for x in range(dx):
				x2 = ( x, dx-1-x )[y&1]
				for d in range(2):
					con.i[id + x+x+d] = (y+1-d)*dx + x2
			id += 2*dx
			con.i[id] = con.i[id-1]
		
		super( BeginMode.TriangleStrip, con )
