namespace kri.shade.par

import System.Collections.Generic

# Standard uniform dictionary
public class Dict( SortedDictionary[of string,IBaseRoot] ):
	# copy contents of another dictionary
	public def attach(d as Dict) as void:
		for u in d:
			Item[u.Key] = u.Value
	# add standard uniform
	public def var[of T](*var as (Value[of T])) as void:
		assert T != kri.buf.Texture
		for v in var:
			Item[v.Name] = v
	# add custom unit
	public def unit(name as string, v as IBase[of kri.buf.Texture]) as void:
		Item[kri.shade.Mega.PrefixUnit + name] = v
	# add texture unit representor
	public def unit(*vat as (Value[of kri.buf.Texture])) as void:
		for v in vat:
			unit(v.Name,v)


# Proxy uniform dictionary
public class DictProxy(Dict):
	public def find(uni as kri.shade.Uniform) as IBaseRoot:
		iv as IBaseRoot = null
		if not TryGetValue(uni.name,iv):
			iv = uni.genProxy()
			Add( uni.name, iv )
		return iv
	public def link(dict as Dict) as void:
		for p in dict:
			iv as IBaseRoot = null
			TryGetValue( p.Key, iv )
			if not iv:	continue
			(iv as IProxy).Base = p.Value
