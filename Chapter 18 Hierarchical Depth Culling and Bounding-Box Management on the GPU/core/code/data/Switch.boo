namespace kri.data

import System.Collections.Generic


public interface IGenerator[of T]:
	def generate() as T


public class Switch[of T(class)]( ILoaderGen[of T] ):
	public final ext	= Dictionary[of string,ILoaderGen[of IGenerator[of T]]]()
	
	public def read(path as string) as T:	#imp: ILoaderGen
		if not Manager.Check(path):
			return null
		for dd in ext:
			if path.EndsWith(dd.Key):
				raw = dd.Value.read(path)
				if not raw:
					return null
				return raw.generate()
		kri.lib.Journal.Log("Switch: extension (${path}) is not recognized")
		return null
