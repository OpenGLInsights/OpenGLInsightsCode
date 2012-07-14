namespace kri.shade

import OpenTK.Graphics.OpenGL

public interface ILogged:
	Log as string:
		get

#---------------------------#
#	STANDARD SHADER	OBJECT	#
#---------------------------#

public class Object(ILogged):
	public final handle		as int		= 0
	[Getter(Description)]
	public final tag		as string	= ''
	private log				as string	= ''
	public final type		as ShaderType
	private static NextId	as uint		= 0
	public final uniqueId	as uint		= 0

	public static def Type(name as string) as ShaderType:
		rez = cast(ShaderType,0)
		if		name.EndsWith('_v'):	rez = ShaderType.VertexShader
		elif	name.EndsWith('_f'):	rez = ShaderType.FragmentShader
		elif	name.EndsWith('_g'):	rez = ShaderType.GeometryShader
		return rez
	
	public static def Load(path as string) as Object:
		return kri.Ant.Inst.dataMan.load[of Object](path)
	
	ILogged.Log as string:
		get: return log
	
	# create from source
	public def constructor(tip as ShaderType, label as string, text as string):
		tag,type = label,tip
		handle = compose(text)
		uniqueId = ++NextId
		if handle:	check()
	
	# delete
	def destructor():
		kri.Help.safeKill({ GL.DeleteShader(handle) })

	private def compose(text as string) as int:
		if string.IsNullOrEmpty(text):
			kri.lib.Journal.Log("Shader: attempt to load an empty code (${tag})")
			return 0
		sid = GL.CreateShader(type)
		GL.ShaderSource(sid,text)
		GL.CompileShader(sid)
		return sid

	# check compilation result
	private def check() as void:
		GL.GetShaderInfoLog(handle,log)
		result as int
		GL.GetShader(handle, ShaderParameter.CompileStatus, result)
		if not result:
			kri.lib.Journal.Log("Shader: Failed to compile object (${tag}), message: ${log}")
