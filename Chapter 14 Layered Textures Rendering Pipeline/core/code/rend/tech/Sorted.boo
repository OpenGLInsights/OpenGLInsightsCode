namespace kri.rend.tech

import System.Collections.Generic

#--------- Batch ---------#

public struct Batch:	# why struct?
	public e	as kri.Entity
	public va	as kri.vb.Array
	public dict	as kri.vb.Dict
	public bu	as kri.shade.Bundle
	public up	as callable() as int
	public off	as int
	public num	as int
	
	public def constructor(ent as kri.Entity, vao as kri.vb.Array):
		e = ent
		va = vao
		dict = e.CombinedAttribs
		off = num = 0

	public def draw() as void:
		nob = up()
		kri.Ant.Inst.params.activate(e)
		e.mesh.render(va,bu,dict,off,num,nob,null)
		
	#public static cMat	= CompMat()
	public class CompMat( IComparer[of Batch] ):
		public def Compare(a as Batch, b as Batch) as int:
			r = a.bu.shader.handle - b.bu.shader.handle
			return r	if r
			r = a.va.handle - b.va.handle
			return r

	public static cMat	as IComparer[of Batch]	= CompMat()


#--------- Sorted batch technique ---------#

public abstract class Sorted(General):
	public	static	comparer	as IComparer[of Batch]	= null
	public	final	extraDict	= kri.vb.Dict()
	public	final	butch		= List[of Batch]()
	private	tempList	= List[of Batch]()
	private bat			as Batch
	
	protected def constructor(name as string):
		super(name)
	
	protected virtual def getUpdater(mat as kri.Material) as System.Func[of int]:
		return do() as int:	return 1
	
	protected override def onPass(va as kri.vb.Array, tm as kri.TagMat, bu as kri.shade.Bundle) as void:
		bat.bu = bu
		bat.va = va
		bat.up = getUpdater( tm.mat )
		bat.num = tm.num
		bat.off = tm.off
		tempList.Add(bat)
	
	protected virtual def isGood(ent as kri.Entity) as bool:
		return true
	
	public override def addObject(e as kri.Entity, vd as kri.vb.Dict) as bool:
		tempList.Clear()
		bat.dict = e.CombinedAttribs
		bat.e = e
		for de in extraDict:
			bat.dict.Add( de.Key, de.Value )
		if super(e,vd):
			butch.AddRange(tempList)
			return true
		return false
	
	protected virtual def drawScene() as void:
		scene = kri.Scene.Current
		if not scene:
			return
		butch.Clear()
		for e in scene.entities:
			if isGood(e):
				addObject(e,null)
		if comparer:
			butch.Sort(comparer)
		for b in butch:
			b.draw()
