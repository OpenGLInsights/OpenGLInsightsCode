namespace kri.vb

import System.Collections.Generic
import OpenTK.Graphics.OpenGL


public struct Entry:
	public	final	buffer	as Object
	public	divisor			as int
	public	final	info	as Info
	public	final	offset	as uint
	public	final	stride	as uint
	
	public	final	static	Zero = Entry(null,Info(),0,0)

	public	def constructor(vat as IProvider, name as string):
		buffer = null
		stride = offset = 0
		divisor = -1
		for ai in vat.Semant:
			if ai.name == name:
				buffer = vat.Data
				info = ai
				offset = stride
			stride += ai.fullSize()
	
	public	def constructor(buf as Object, ai as Info, off as uint, size as uint):
		buffer = buf
		divisor = 0
		info = ai
		offset,stride = off,size



#-----------------------
#	VERTEX ARRAY
#-----------------------

public class Array:
	public	final	handle		as uint
	private final	slots		= array[of Entry](	kri.Ant.Inst.caps.vertexAttribs )

	private	useMask	as uint		= 0
	private index	as Object	= null
	private	static	current	as Array	= null
	
	public	static	Bind	as Array:
		get:	return current
		set:
			if current == value:	return
			current = value
			if value:
				value.useMask = 0
				GL.BindVertexArray( value.handle )
			else:
				GL.BindVertexArray(0)
	
	public def constructor():
		tmp = 0
		GL.GenVertexArrays(1,tmp)
		handle = tmp
	def destructor():
		tmp = handle
		kri.Help.safeKill() do():
			GL.DeleteVertexArrays(1,tmp)
	
	public def clean() as void:
		Bind = self
		for i in range(slots.Length):
			slots[i] = Entry.Zero
			GL.DisableVertexAttribArray(i)
	
	public def push(slot as int, ref e as Entry) as bool:
		d = e.buffer
		if slots[slot] == e:
			return true
		d.bind()
		if not d.Allocated:
			kri.lib.Journal.Log("VAO: trying to use un-allocated buffer (${d.handle}) for (${handle})")
			return false
		GL.EnableVertexAttribArray( slot )
		if slots[slot].divisor != e.divisor:
			GL.VertexAttribDivisor( slot, e.divisor )
		if e.info.integer: #TODO: use proper enum
			GL.VertexAttribIPointer( slot, e.info.size,
				cast(VertexAttribIPointerType,cast(int,e.info.type)),
				e.stride, System.IntPtr(e.offset) )
		else:
			GL.VertexAttribPointer( slot, e.info.size,
				e.info.type, false, e.stride, e.offset)
		slots[slot] = e
		return true
	
	public def pushAll(ind as Object, sat as (kri.shade.Attrib), edic as Dictionary[of string,Entry]) as bool:
		Bind = self
		for i in range(sat.Length):
			str = sat[i].name
			if not str:
				continue
			assert sat[i].size
			en as Entry
			if not edic.TryGetValue(str,en):
				return false
			ai = en.info
			if not sat[i].matches(ai):
				return false
			if not push(i,en):
				return false
			useMask |= 1<<i
		if index != ind:
			Object.Index = index = ind
		# need at least one
		if not useMask:
			for en in edic.Values:
				push(0,en)
				break
		return useMask + edic.Count > 0

	public def hasConflicts() as byte:
		lx = List[of Object]()
		if index:	lx.Add(index)
		for i in range(slots.Length):
			buf = slots[i].buffer
			if buf and ((1<<i)&useMask):
				lx.Add(buf)
		num = 0
		for bx in kri.TransFeedback.Cache:
			if bx and bx in lx:
				++num
		return num
