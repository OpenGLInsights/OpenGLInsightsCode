namespace kri.lib.par.spa

import System
import OpenTK
import kri.shade
import kri.meta


#---	Shared spatial, used for the camera,model,light & bones	---#
public class Shared(IBase):
	[Getter(Name)]
	private final name	as string
	public final position	as par.Value[of Vector4]
	public final rotation 	as par.Value[of Vector4]
	
	public def constructor(s as string):
		name = s
		position = par.Value[of Vector4](s+'.pos')
		rotation = par.Value[of Vector4](s+'.rot')
	public def activate(ref sp as kri.Spatial) as void:
		position.Value = sp.PackPos
		rotation.Value = sp.PackRot
	public def activate(n as kri.Node) as void:
		sp = kri.Node.SafeWorld(n)
		activate(sp)

	def IBase.link(d as par.Dict) as void:
		d.var(position,rotation)


#---	Linked value	---#
public class TransVal( par.IBase[of Vector4] ):
	public node			as kri.Node = null
	public final fun	as callable(ref kri.Spatial) as Vector4
	public override Value	as Vector4:
		get:
			sp = kri.Node.SafeWorld(node)
			return fun(sp)
	public def constructor(f as callable(ref kri.Spatial) as Vector4):
		assert f
		fun = f


#---	Linked spatial to a concrete node, doesn't interact online	---#
public class Linked(IBase):
	private static def GetPos(ref s as kri.Spatial):
		return s.PackPos
	private static def GetRot(ref s as kri.Spatial):
		return s.PackRot
	
	[Getter(Name)]
	private final name	as string
	public final position	= TransVal(GetPos)
	public final rotation 	= TransVal(GetRot)

	public def constructor(s as string):
		name = s
	public def activate(n as kri.Node) as void:
		position.node = rotation.node = n
	public def extract() as kri.Node:
		return position.node

	def IBase.link(d as par.Dict) as void:
		d[Name+'.pos']	= position
		d[Name+'.rot']	= rotation
