namespace kri.data

import System.Collections.Generic


public interface ILoader:
	pass

public interface ILoaderGen[of T](ILoader):
	def read(path as string) as T


#-------------------------------------------------------#
#			RESOURCE MANAGER (itself)					#
#-------------------------------------------------------#

public class Manager:
	private final loadMap	= Dictionary[of System.Type, ILoader]()
	private final cache		= Dictionary[of string, object]()

	public static def Check(path as string) as bool:
		if System.IO.File.Exists(path):
			return true
		kri.lib.Journal.Log('Loader: Unable to locate ' + path)
		return false
	
	public def register[of T](loader as ILoaderGen[of T]) as void:
		loadMap[T] = loader
	
	public def load[of T(class)](path as string) as T:
		ob as object = null
		if cache.TryGetValue(path,ob):
			tob = ob as T
			assert tob and 'invalid cache entry type'
			return tob
		loader as ILoader = null
		if loadMap.TryGetValue( typeof(T), loader ):
			loadgen = loader as ILoaderGen[of T]
			if not loadgen:
				kri.lib.Journal.Log("Manager: invalid loader type (${loader.GetType()})")
				return null
			cache[path] = tob = loadgen.read(path)
			return tob
		return null
	
	public def loadTo[of T(class)](path as string, ref val as T) as void:
		val = load[of T](path)
	
	public def release(path as string) as object:
		ob as object = null
		cache.TryGetValue(path,ob)
		cache.Remove(path)
		return ob
	
	public def clear() as void:
		cache.Clear()
