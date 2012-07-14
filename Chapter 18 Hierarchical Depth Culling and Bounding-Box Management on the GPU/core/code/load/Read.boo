namespace kri.load

import System
import OpenTK
import OpenTK.Graphics


public class Reader:
	public final at		as Atom
	public final bin	as IO.BinaryReader
	public final data	as kri.data.Manager


	#---------	NODE RESOLVING IN A POST-PROCESS	---------#
	public struct ResNode:
		public name as string
		public fun	as Action[of kri.Node]

	public final nodeResolve	= List[of ResNode]()
	public def addResolve(fun as Action[of kri.Node]) as void:
		nodeResolve.Add( ResNode( name:getString(), fun:fun ))
	public def addPostProcess(fun as Action[of kri.Node]) as void:
		nodeResolve.Add( ResNode( name:null, fun:fun ))
	

	#---------	CREATION	---------#
	
	public def constructor(path as string, dataMan as kri.data.Manager):
		.at = Atom(path)
		.bin = IO.BinaryReader( IO.File.OpenRead(path) )
		.data = dataMan
	
	public def finish() as Atom:
		for nr in nodeResolve:
			n as kri.Node = null
			str = nr.name
			if string.IsNullOrEmpty(str) or at.nodes.TryGetValue(str,n):
				nr.fun(n)
			else:
				kri.lib.Journal.Log("Loader: node (${str}) is not resolved")
		nodeResolve.Clear()
		bin.Close()
		return at


	#---------	CONTEXT REPRESENTORS	---------#

	private final rep	= []
	public Clear as bool:
		get: return not rep.Count
		set: rep.Clear()
	
	public def geData[of T]() as T:
		#return rep.Find(predicate) as T
		for ob in rep:
			t = ob as T
			return t	if t
		return null as T

	public def puData[of T](r as T) as void:
		#rep.RemoveAll(predicate)
		rep.Remove( geData[of T]() )
		rep.Insert(0,r)	if r
	
	
	#---------	HELPER METHODS	---------#
	
	public def getByte() as byte:
		return bin.ReadByte()
	public def getReal() as single:
		return bin.ReadSingle()
	public def getScale() as single:
		return getVector().LengthSquared / 3f
	public def getColor() as Color4:
		return Color4( getReal(), getReal(), getReal(), 1f )
	public def getColorByte() as Color4:
		c = bin.ReadBytes(3)	#rbg
		a as byte = 0xFF
		return Color4(c[0],c[1],c[2],a)
	public def getColorRaw() as ColorRaw:
		return ColorRaw(
			red		:bin.ReadByte(),
			green	:bin.ReadByte(),
			blue	:bin.ReadByte())
	public def getColorFull() as Color4:
		c = getColorByte()
		c.A = getReal()	# alpha
		v = getReal()	# intensity
		#v *= v			# squared
		c.R *= v; c.G *= v; c.B *= v
		return c

	public def getString(size as byte) as string:
		return string( bin.ReadChars(size) ).TrimEnd( char(0) )
	public def getString() as string:
		return getString( bin.ReadByte() )
	public def getVector() as Vector3:
		return Vector3( X:getReal(), Y:getReal(), Z:getReal() )
	public def getVec2() as Vector2:
		return Vector2( X:getReal(), Y:getReal() )
	public def getVec4() as Vector4:
		return Vector4( Xyz:getVector(), W:getReal() )
	public def getQuat() as Quaternion:
		return Quaternion( Xyz:getVector(), W:getReal() )
	public def getQuatRev() as Quaternion:
		return Quaternion( W:getReal(), Xyz:getVector() )
	public def getQuatEuler() as Quaternion:
		return kri.Spatial.EulerQuat( getVector() )
	public def getSpatial() as kri.Spatial:
		return kri.Spatial( pos:getVector(), scale:getReal(), rot:getQuat() )
	
	public def getMaterial() as kri.Material:
		name = getString()
		rez as kri.Material = null
		if not at.mats.TryGetValue(name,rez):
			kri.lib.Journal.Log("Loader: material not found (${name})")
			rez = kri.Ant.Inst.loaders.materials.con.mDef
			puData[of kri.Material](null)
		else:
			puData(rez)
		return rez
