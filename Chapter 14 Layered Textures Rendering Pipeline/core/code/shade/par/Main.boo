namespace kri.shade.par

#-----------------------#
#	AUTO PARAMETERS 	#
#-----------------------#

public interface IBaseRoot:
	pass

public interface IBase[of T](IBaseRoot):
	Value	as T:
		get

public class ValuePure[of T]( IBase[of T] ):
	[Property(Value)]
	private val	as T


public abstract class ValueBase[of T]( IBase[of T], kri.INamed ):
	[Getter(Name)]
	private final name	as string
	public def constructor(s as string):
		name = s


public class Value[of T]( IBase[of T], kri.INamed ):
	[Property(Value)]
	private val	as T
	[Getter(Name)]
	private final name	as string
	public def constructor(s as string):
		name = s


public class Texture(Value[of kri.buf.Texture]):
	public def constructor(s as string):
		super(s)


public interface IProxy:
	Base	as IBaseRoot:
		set

public class Proxy[of T](IBase[of T],IProxy):
	public base	as IBase[of T]	= null
	IProxy.Base as IBaseRoot:
		set: base = value as IBase[of T]
	public Value as T:
		get: return base.Value


/*	gives Failed to create 'kri.shade.par.Value2[of T]' type.. (BCE0055)
public class Value2[of T](ValueBase[of T]):
	[property(Value)]
	private val	as T
	public def constructor(s as string):
		super(s)
*/

public class UnitProxy( IBase[of kri.buf.Texture] ):
	private final fun as callable() as kri.buf.Texture
	public def constructor(f as callable() as kri.buf.Texture):
		fun = f
	public override Value as kri.buf.Texture:
		get: return fun()
