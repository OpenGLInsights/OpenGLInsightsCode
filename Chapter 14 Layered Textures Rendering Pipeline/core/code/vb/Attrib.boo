namespace kri.vb

import System.Collections.Generic


public interface ISemanted:
	Semant	as List[of Info]:
		get

public interface IProvider(ISemanted):
	Data	as Object:
		get



public class Dict( Dictionary[of string,Entry] ):
	public def constructor(*stores as (Storage)):
		super()
		for st in stores:
			if st: st.fillEntries(self)
	
	public def fake(*names as (string)) as byte:
		mask = 0
		for i in range(names.Length):
			if names[i]	in Keys:
				mask |= 1<<i
			else:
				if not i:	return mask
				self[names[i]] = self[names[0]]
		return mask
	
	public def add(buf as Object, ai as Info, offset as uint, stride as uint) as void:
		self[ai.name] = kri.vb.Entry(buf,ai,offset,stride)


public class Attrib( IProvider, Object ):
	[Getter(Semant)]
	private final semantics	as List[of Info]	= List[of Info]()
	IProvider.Data		as Object:
		get: return self

	public def countSize() as uint:
		rez as uint = 0
		for a in semantics:
			rez += a.fullSize()
		return rez
	
	public def initUnit(num as uint) as void:
		init( num * countSize() )

	public def bindAsDestination(id as uint, off as uint, num as uint) as void:
		size = countSize()
		bindAsDestination( id, System.IntPtr(off*size), System.IntPtr(num*size) )
