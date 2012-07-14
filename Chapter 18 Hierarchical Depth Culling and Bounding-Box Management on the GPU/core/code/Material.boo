namespace kri

import System.Collections.Generic
import OpenTK.Graphics.OpenGL

public enum Blend:
	None
	Add
	Sub
	Min
	Max
	Alpha


public class Material( ani.data.Player ):
	public final	name	as string
	public			blend	= Blend.None
	public final	dict	= shade.par.Dict()
	public final	unit	= List[of meta.AdUnit]()
	public final	tech	= Dictionary[of string,shade.Bundle]()
	public final	metaList = List[of meta.IBaseMat]()
	
	public Meta[str as string] as meta.IBaseMat:
		get: return metaList.Find({m| return m.Name == str})
	public def getData[of T(struct)](str as string) as meta.Data[of T]:
		return Meta[str] as meta.Data[of T]
	
	public def constructor(str as string):
		name = str
	private def clone[of T(System.ICloneable)](me as T) as T:
			return me.Clone() as T

	# clone with all 1-st level metas
	public def constructor(mat as Material):
		name = mat.name
		for u in mat.unit:
			unit.Add( clone(u) )
		for me in mat.metaList:
			metaList.Add( clone(me) )

	# update dictionary
	public def link() as void:
		dict.Clear()
		lis = List[of meta.IBase]()
		lis.Add(null)
		def push(h as meta.IBase):
			if h in lis:	return
			h.link(dict)
			lis.Add(h)
		for m in metaList:
			push(m)
			ma = m as meta.Advanced
			if not (ma and ma.Unit>=0):
				continue
			u = unit[ma.Unit]
			if not u.Value:
				kri.lib.Journal.Log("Material: texture is missing for ${name}:${m.Name}")
			(u as meta.ISlave).link( m.Name, dict )
		for u in unit:
			push( u.input )
	
	# collect shaders for meta data
	public def collect(geom as bool, melist as (string)) as shade.Object*:
		dd = Dictionary[of shade.Object,meta.IShaded]()
		def push(m as meta.IShaded):
			if (m and m.Shader):
				dd[m.Shader] = m
		# collect mets shaders & map inputs
		din = Dictionary[of string,meta.Hermit]()
		for str in melist:
			reject = str.StartsWith('!')
			if reject:
				str = str.Substring(1)
			m = Meta[str] as meta.Advanced
			if reject != (not m):
				return null
			if not m:	continue
			push(m)
			ud = m.Unit
			if ud<0:	continue
			u = unit[ud]
			if not u.input:	continue
			push( u.input )
			din.Add( m.Name, u.input )
		# check geometry shaders
		if not geom:
			for dk in dd.Keys:
				if dk.type == ShaderType.GeometryShader:
					#dd.Remove(dk)
					return null
		# generate coords
		mapins = load.Meta.MakeTexCoords(geom,din)
		if not mapins:	return null	
		for sh in mapins:
			dd[sh] = null
		return dd.Keys
