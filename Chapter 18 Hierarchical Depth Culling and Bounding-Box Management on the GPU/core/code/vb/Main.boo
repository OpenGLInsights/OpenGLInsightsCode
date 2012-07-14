namespace kri.vb

import System
import OpenTK.Graphics.OpenGL


#-----------------------
#	BUFFER OBJECT
#-----------------------

public class Object:
	public	final handle	as uint
	[Getter(TimeStamp)]
	private	timeStamp		as uint	= 0
	[Getter(Allocated)]
	private	allocated		as uint = 0
	
	public	static	final	Zero = Object(0)
	
	public	static Bind[targ as BufferTarget] as Object:
		set: GL.BindBuffer( targ, (value.handle if value else cast(uint,0)) )
	public	static Index	as Object:
		set: Bind[BufferTarget.ElementArrayBuffer	] = value
	public	static Vertex	as Object:
		set: Bind[BufferTarget.ArrayBuffer			] = value
	public	static Pack		as Object:
		set: Bind[BufferTarget.PixelPackBuffer		] = value
	public	static Unpack	as Object:
		set: Bind[BufferTarget.PixelUnpackBuffer	] = value
	public	static final DefTarget	= BufferTarget.ArrayBuffer
	
	# creating
	public def constructor():
		tmp = 0
		GL.GenBuffers(1,tmp)
		handle = tmp
	private def constructor(hid as uint):
		handle = hid
		allocated = 1
	def destructor():
		tmp = handle
		kri.Help.safeKill({ GL.DeleteBuffers(1,tmp) })
	
	# binding
	public def bind() as void:
		Bind[DefTarget] = self
	public def bindAsDestination(id as uint) as void:
		GL.BindBufferBase(	BufferTarget.TransformFeedbackBuffer, id, handle )
		timeStamp += 1
	public def bindAsDestination(id as uint, off as System.IntPtr, num as System.IntPtr) as void:
		GL.BindBufferRange(	BufferTarget.TransformFeedbackBuffer, id, handle, off, num )
		timeStamp += 1
	
	# filling
	private static def GetHint(dyn as bool) as BufferUsageHint:
		return (BufferUsageHint.StreamDraw if dyn else BufferUsageHint.StaticDraw)
	public def init(size as uint) as void:
		bind()
		allocated = size
		GL.BufferData(DefTarget, IntPtr(size), IntPtr.Zero, GetHint(true))
		timeStamp += 1
	public def init[of T(struct)](ptr as (T), dyn as bool) as void:
		bind()
		allocated = ptr.Length * kri.Sizer[of T].Value
		GL.BufferData(DefTarget, IntPtr(allocated), ptr, GetHint(dyn))
		timeStamp += 1
	
	# mapping
	public def tomap(ba as BufferAccess) as IntPtr:
		bind()
		timeStamp += (0,1)[ba != BufferAccess.ReadOnly]
		return GL.MapBuffer(DefTarget,ba)
	public def unmap() as bool:
		return GL.UnmapBuffer(DefTarget)

	[ext.spec.Method(( byte,short,int,single ))]
	[ext.RemoveSource]
	public def read[of T(struct)](ar as (T), soff as int) as void:
		buf = tomap( BufferAccess.ReadOnly )
		if soff:
			size = kri.Sizer[of T].Value
			buf = IntPtr( buf.ToInt32() + soff*size )
		Marshal.Copy( buf, ar, 0, ar.Length )
		unmap()
