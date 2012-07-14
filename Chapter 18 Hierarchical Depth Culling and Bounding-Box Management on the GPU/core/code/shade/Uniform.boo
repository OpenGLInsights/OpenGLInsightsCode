namespace kri.shade

import OpenTK
import OpenTK.Graphics.OpenGL


public struct Uniform:
	public name	as string
	public size	as int
	public type	as ActiveUniformType

	public def genParam(loc as int, iv as par.IBaseRoot, ref tun as int) as Parameter:
		if not iv:	return null
		it = iv.GetType().GetInterface('IBase`1')
		T = object
		if it:	T = it.GetGenericArguments()[0]
		if T == kri.buf.Texture and name.Contains( Mega.PrefixUnit ):
			tn = tun++
			return ParTexture(loc,iv,tn)
		elif T == int				and type in (ActiveUniformType.Int,ActiveUniformType.UnsignedInt):
			return ParUni_int(loc,iv)
		elif T == bool				and type == ActiveUniformType.Bool:
			return ParUni_bool(loc,iv)
		elif T == single			and type == ActiveUniformType.Float:
			return ParUni_single(loc,iv)
		elif T == Vector4			and type == ActiveUniformType.FloatVec4:
			return ParUni_Vector4(loc,iv)
		elif T == Quaternion		and type == ActiveUniformType.FloatVec4:
			return ParUni_Quaternion(loc,iv)
		elif T == Graphics.Color4	and type == ActiveUniformType.FloatVec4:
			return ParUni_Color4(loc,iv)
		kri.lib.Journal.Log("Uniform: '${name} doesn't match: ${type} for ${iv.GetType()}")
		return null
	
	public def genProxy() as par.IBaseRoot:
		if name.StartsWith( Mega.PrefixUnit ):
			return par.Proxy[of kri.buf.Texture]()
		elif type in (ActiveUniformType.Int,ActiveUniformType.UnsignedInt):
			return par.Proxy[of int]()
		elif type == ActiveUniformType.Bool:
			return par.Proxy[of bool]()
		elif type == ActiveUniformType.Float:
			return par.Proxy[of single]()
		elif type == ActiveUniformType.FloatVec4:
			return par.Proxy[of Vector4]()
		return null
