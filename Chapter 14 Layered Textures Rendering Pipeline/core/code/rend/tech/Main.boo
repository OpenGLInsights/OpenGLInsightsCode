namespace kri.rend.tech


public class Basic( kri.rend.Basic ):
	public	final name	as string		# technique name
	protected def constructor(str as string):
		name = str
		kri.Ant.Inst.techniques.Add(name,self)
	def destructor():
		core = kri.Ant.Inst
		if core:
			core.techniques.Remove(name)


#---------	META TECHNIQUE	--------#

public class Meta(Sorted):
	private final lMets	as (string)
	private final lOuts	as (string)
	private final geom	as bool
	protected shobs			= List[of kri.shade.Object]()
	protected final dict	= kri.shade.par.Dict()
	private final factory	= kri.shade.Linker(onLink)
	
	protected def constructor(name as string, gs as bool, outs as (string), *mets as (string)):
		super(name)
		lMets,lOuts = mets,outs
		geom = gs
	
	protected def shade(prefix as string) as void:
		shade( prefix+s for s in ('_v','_f') )
	protected def shade(slis as string*) as void:
		shobs.Extend( kri.shade.Object.Load(s) for s in slis )
	
	private virtual def onLink(sa as kri.shade.Mega) as void:
		if lOuts:
			sa.fragout( *lOuts )
		sa.add( *kri.Ant.Inst.libShaders )
		sa.add( *shobs.ToArray() )

	public override def construct(mat as kri.Material) as kri.shade.Bundle:
		sl = mat.collect(geom,lMets)
		if not sl:	return null
		return factory.link( sl, dict, mat.dict )
