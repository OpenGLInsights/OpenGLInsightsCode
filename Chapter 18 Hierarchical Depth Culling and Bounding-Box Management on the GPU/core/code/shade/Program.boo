namespace kri.shade

import System
import OpenTK
import OpenTK.Graphics
import OpenTK.Graphics.OpenGL


#-------------------------------#
#	STANDARD SHADER	PROGRAM		#
#-------------------------------#

public class Program(ILogged):
	public final	handle	as int
	private hasGeometry		as bool	= false
	[Getter(GeometryInput)]
	private inputType		as BeginMode	= BeginMode.Polygon
	[Getter(Ready)]
	private linked	as bool = false
	private static	Current	as Program	= null
	private log		as string = ''
	private final	blocks	= List[of Object]()	# for debug
	
	public Description as string:
		get: return join( (sh.Description for sh in blocks), "\n")
	
	ILogged.Log as string:
		get: return log

	public def constructor():
		handle = GL.CreateProgram()
	
	protected def constructor(xid as int):
		handle = xid
		linked = true
	
	def destructor():
		if Current == self:
			Current = null
		kri.Help.safeKill({ GL.DeleteProgram(handle) })
	
	public static Zero	= Program(0)
	
	public def check(pp as ProgramParameter) as bool:
		GL.GetProgramInfoLog(handle,log)
		result as int
		GL.GetProgram(handle, pp, result)
		if result:	return true
		kri.lib.Journal.Log("Shader: Failed to link program (${handle}), message: ${log}")
		return false

	public def validate() as bool:
		GL.ValidateProgram(handle)
		return check( ProgramParameter.ValidateStatus )
	
	public def isCompatible(bm as BeginMode) as bool:
		#workaround: inputType reported by GL does not correspond
		#the actual value (always 0), contrary to specification
		return hasGeometry==false or inputType==bm or true
	
	# add specific objects
	public def add(*shads as (Object)) as void:
		assert not linked
		blocks.Extend(shads)
		for sh in shads:
			if sh and sh.handle:
				hasGeometry |= (sh.type == ShaderType.GeometryShader)
				GL.AttachShader(handle, sh.handle)
			else:
				str = ''
				if sh:	str = "(${sh.Description})"
				kri.lib.Journal.Log('Shader: attaching invalid object '+str)
	
	# add object from library
	public def add(names as string*) as void:
		for s in names:
			add( Object.Load(s) )
	public def add(*names as (string)) as void:
		add(names)
	
	# link program
	public virtual def link() as bool:
		#assert not linked
		GL.LinkProgram(handle)
		linked = check( ProgramParameter.LinkStatus )
		if linked and hasGeometry:
			itype = 0
			GL.GetProgram(handle, ProgramParameter.GeometryInputType, itype)
			inputType = cast(BeginMode,itype)
		return linked
	
	# activate program
	public def bind() as void:
		assert linked
		if Current == self:
			return
		GL.UseProgram(handle)
		Current = self

	# assign vertex attribute slot
	public def attrib(index as byte, name as string) as void:
		assert index < kri.Ant.Inst.caps.vertexAttribs
		GL.BindAttribLocation(handle, index, name)
	
	public def attribAll(names as (string)) as void:
		for i in range(names.Length):
			attrib(i,names[i])
	
	# assign fragment output slot
	public def fragout(*names as (string)) as void:
		assert names.Length <= kri.Ant.Inst.caps.drawBuffers
		for i in range(names.Length):
			GL.BindFragDataLocation( handle, i, names[i] )
	
	# assign transform feedback
	public def feedback(sep as bool, *names as (string)) as void:
		tm = (TransformFeedbackMode.InterleavedAttribs,TransformFeedbackMode.SeparateAttribs)[sep]
		GL.TransformFeedbackVaryings( handle, names.Length, names, tm )

	# get uniform location by name
	public def getLocation(name as string) as int:
		assert linked
		return GL.GetUniformLocation(handle,name)
	
	# get uniform value by location
	[ext.spec.Method(( int,single ))]
	[ext.RemoveSource]
	public def getValue[of T(struct)](loc as int, ref val as T) as void:
		assert loc >= 0
		GL.GetUniform(handle,loc,val)
	
	# set uniform parameter value
	private static def fun[of T(struct)](l as int, ref v as T) as void:
		assert 'Uniform type not supported'
	
	[ext.spec.ForkMethod(fun, GL.Uniform1, (int,single))]
	[ext.spec.ForkMethod(fun, GL.Uniform4, (Color4,Vector4,Quaternion))]
	public static def Param[of T(struct)](loc as int, ref val as T) as void:
		assert loc >= 0
		fun(loc,val)

	# clear everything
	public virtual def clear() as void:
		linked = hasGeometry = false
		for sh in blocks:
			GL.DetachShader( handle, sh.handle )	if sh
		blocks.Clear()
