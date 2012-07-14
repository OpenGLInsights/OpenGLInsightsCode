namespace kri

import System
import OpenTK.Graphics.OpenGL

# Generic structures size calculator

public static class Sizer[of T(struct)]:
	public final Value = System.Runtime.InteropServices.Marshal.SizeOf(T)


#-----------#
#	HELP	#
#-----------#

public static class Help:
	# swap two abstract elements
	public def swap[of T](ref a as T, ref b as T) as void:
		a,b = b,a
	# provides skipping of resource unloading errors on exit
	public def safeKill(fun as callable() as void) as void:
		if OpenTK.Graphics.GraphicsContext.CurrentContext:
			fun()
	# semantics fill helper
	public def enrich(ob as vb.ISemanted, size as byte, *names as (string)) as void:
		for str in names:
			ob.Semant.Add( vb.Info(
				name:str, size:size, integer:false,
				type:VertexAttribPointerType.Float ))
	# smart logical shift
	public def shiftInt(val as int, shift as int) as int:
		return (val<<shift	if shift>0 else val>>-shift)
	# copy dictionary
	public def copyDic[of K,V](dv as (Collections.Generic.Dictionary[of K,V])) as void:
		for x in dv[1]:
			dv[0].Add( x.Key, x.Value )


#---------------#
#	GL Caps		#
#---------------#

public class State:
	public final	cap as EnableCap
	public def constructor(state as EnableCap):
		cap = state
	public On as bool:
		get: return GL.IsEnabled(cap)
		set: 
			if value:	GL.Enable(cap)
			else:		GL.Disable(cap)
	

# Provides GL state on/off mechanics
public class Section(State,IDisposable):
	public static final DummyCap	= EnableCap.Fog
	
	public def switch(val as bool) as void:
		if cap == DummyCap:	return
		if On == val:
			lib.Journal.Log("GL: unexpected state (${cap} == ${not val})")
		On = val
			
	public def constructor(state as EnableCap):
		super(state)
		switch(true)
	
	public def constructor():
		self(DummyCap)
		
	def IDisposable.Dispose() as void:
		switch(false)


# Provide standard blending options
public class Blender(Section):
	public def constructor():
		super( EnableCap.Blend )
	public def constructor(method as Blend):
		if method == Blend.None:
			super()
			return
		self()
		if method == Blend.Sub:
			GL.BlendEquation( BlendEquationMode.FuncSubtract )
			add()
		elif method == Blend.Add:
			GL.BlendEquation( BlendEquationMode.FuncAdd )
			add()
		elif method == Blend.Min:
			GL.BlendEquation( BlendEquationMode.Min )
			add()
		elif method == Blend.Max:
			GL.BlendEquation( BlendEquationMode.Max )
			add()
		elif method == Blend.Alpha:
			GL.BlendEquation( BlendEquationMode.FuncAdd )
			alpha()
		else:
			lib.Journal.Log("Blend: unknown method (${method})")
			return

	public Alpha as single:
		set: GL.BlendColor(0f,0f,0f, value)
	public static def alpha() as void:
		GL.BlendFunc( BlendingFactorSrc.SrcAlpha,		BlendingFactorDest.OneMinusSrcAlpha )
	public static def add() as void:
		GL.BlendFunc( BlendingFactorSrc.One,			BlendingFactorDest.One )
	public static def over() as void:
		GL.BlendFunc( BlendingFactorSrc.One,			BlendingFactorDest.Zero )
	public static def skip() as void:
		GL.BlendFunc( BlendingFactorSrc.Zero,			BlendingFactorDest.One )
	public static def multiply() as void:
		GL.BlendFunc( BlendingFactorSrc.DstColor,		BlendingFactorDest.Zero )
	public static def overAlpha() as void:
		GL.BlendFunc( BlendingFactorSrc.One,			BlendingFactorDest.ConstantAlpha )
	public static def skipAlpha() as void:
		GL.BlendFunc( BlendingFactorSrc.ConstantAlpha,	BlendingFactorDest.One )
