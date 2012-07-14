namespace support.asm


import OpenTK

public class Segment( kri.meta.IBase ):
	public	final	name	as string
	private	final	pNode	as kri.lib.par.spa.Linked
	private	final	pArea	as kri.shade.par.Value[of Vector4]
	private	final	pChan	as kri.shade.par.Value[of int]
	
	public def constructor(str as string):
		name = str
		pNode = kri.lib.par.spa.Linked(str+'spa')
		pArea = kri.shade.par.Value[of Vector4](str+'area')
		pChan = kri.shade.par.Value[of int](str+'channel')
	
	public def activate(el as Element) as void:
		pNode.activate( el.node )
	
	kri.INamed.Name as string:
		get: return name
	def kri.meta.IBase.link(d as kri.shade.par.Dict) as void:
		(pNode as kri.meta.IBase).link(d)
		d.var(pArea)
		d.var(pChan)



public class Context:
	public	final	seg		= List[of Segment](
		Segment("el[${i}].") for i in range(200)
		).ToArray()
	public	final	dict	= kri.shade.par.Dict()
	public	final	pMatId	= kri.shade.par.Texture('mat_id')
	
	public def constructor():
		for s in seg:
			(s as kri.meta.IBase).link(dict)
		dict.unit(pMatId)
	
	public def activate(sc as Scene) as void:
		for i in range( sc.elems.Length ):
			seg[i].activate(sc.elems[i])
		pMatId.Value = sc.texMat
