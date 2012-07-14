namespace kri.vb

import System.Collections.Generic
import OpenTK.Graphics.OpenGL


public struct Info:
	public name	as string
	public size	as byte		# in units
	public type	as VertexAttribPointerType
	public integer	as bool
	
	public def fullSize() as uint:
		b as uint = 0
		b = 1	if type == VertexAttribPointerType.UnsignedByte
		b = 2	if type == VertexAttribPointerType.HalfFloat
		b = 2	if type == VertexAttribPointerType.UnsignedShort
		b = 4	if type == VertexAttribPointerType.Float
		b = 4	if type == VertexAttribPointerType.UnsignedInt
		assert b and 'not a valid type'
		return size * b
	
	public static final Dummy = Info( name:'dummy', size:1,
		type:VertexAttribPointerType.UnsignedByte )


#---------

public class Storage:
	public final buffers		= List[of kri.vb.Attrib]()
	public final static	Empty	= Storage()
	public Allocated as bool:
		get:
			for b in buffers:
				if not b.Allocated:
					return false
			return true
	
	public def find(name as string) as kri.vb.Attrib:
		return buffers.Find() do(v as kri.vb.Attrib):
			s = v.Semant
			return s.Count>0 and s[0].name==name

	public def swap(x as kri.vb.Attrib, y as kri.vb.Attrib) as void:
		#vbo.Remove(x)
		#vbo.Add(y)
		buffers[ buffers.IndexOf(x) ] = y

	public def gatherAttribs() as (string):
		al = List[of string]()
		for vat in buffers:
			for ai in vat.Semant:
				al.Add( ai.name )
		return al.ToArray()
	
	public def fillEntries(d as Dict) as void:
		for vat in buffers:
			off = 0
			size = vat.countSize()
			for sem in vat.Semant:
				d.add(vat,sem,off,size)
				off += sem.fullSize()
