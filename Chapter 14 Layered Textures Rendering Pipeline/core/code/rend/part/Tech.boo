namespace kri.rend.part

import System
import System.Collections.Generic
import kri.shade

#---------	RENDER PARTICLES: TECHNIQUE	--------#

public class Tech( Basic ):
	public final name	as	string
	public static final	Invalid	= (of int:,)
	
	protected def constructor(str as string):
		name = str
		kri.Ant.Inst.techniques.Add(name,self)
	def destructor():
		core = kri.Ant.Inst
		if core:
			core.techniques.Remove(name)
	
	public abstract def construct(pe as kri.part.Emitter) as Bundle:
		pass
	protected virtual def update(pe as kri.part.Emitter) as uint:
		return 1
	
	protected override def prepare(pe as kri.part.Emitter, ref nin as uint) as Bundle:
		m = pe.mat
		if not m:
			return null
		bu as kri.shade.Bundle = null
		if not m.tech.TryGetValue(name,bu):
			m.tech[name] = bu = construct(pe)
		if not bu:
			return null
		ready as bool
		if not pe.techReady.TryGetValue(name,ready):
			pe.techReady[name] = ready = pe.draw(bu,0)
		if not ready:
			return null
		nin = update(pe)
		return bu


#---------	RENDER PARTICLES: META TECH	--------#

public class Meta( Tech ):
	private final lMets		as (string)
	private final geom		as bool
	protected shobs			= List[of Object]()
	protected final dict	= par.Dict()
	private final factory	= Linker(setup)
	
	public def constructor(name as string, gs as bool, *mets as (string)):
		super(name)
		lMets,geom = mets,gs
	
	protected def shade(prefix as string) as void:
		shade( prefix+s	for s in('_v','_f') )
	protected def shade(slis as string*) as void:
		shobs.AddRange( Object.Load(s) for s in slis )
	
	private def setup(sa as Mega) as void:
		sa.add( *kri.Ant.Inst.libShaders )
		sa.add( *shobs.ToArray() )

	public override def construct(pe as kri.part.Emitter) as Bundle:
		assert pe.mat and pe.owner
		sl = pe.mat.collect(geom,lMets)
		if not sl:	return null
		return factory.link( sl, dict, pe.owner.dict, pe.mat.dict )
	
	public virtual def onManager(man as kri.part.Manager) as void:
		pass
