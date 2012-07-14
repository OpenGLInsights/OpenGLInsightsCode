namespace kri

import System
import OpenTK


public struct Box:
	public	center	as Vector3
	public	hsize	as Vector3
	public def constructor(x as single):
		center	= Vector3(0f)
		hsize	= Vector3(x)


#------------------------------------------
#	DUAL QUATERNION
#	effectively represents spatial data for the interpolation

public struct DualQuat:
	public re	as Quaternion
	public im	as Quaternion
	public static final Identity = DualQuat( re:Quaternion.Identity,
		im:Quaternion( Xyz:Vector3.Zero, W:0f ) )
	
	public def constructor(ref s as Spatial):
		re = s.rot
		im = Quaternion( Xyz:0.5f*s.pos, W:0f ) * re
	public def lerp(ref s0 as Spatial, ref s1 as Spatial, k as single) as void:
		re	= (1f-k) * s0.rot + k * s1.rot
		pos	= (1f-k) * s0.pos + k * s1.pos
		im	= Quaternion( Xyz:0.5f*pos, W:0f ) * re
	public def toUnitSpatial(ref s as Spatial) as void:
		kn = 1f / re.Length
		s.pos = (2f * kn * kn) * (im * Quaternion.Conjugate(re)).Xyz
		s.rot = kn * re



#------------------------------------------
#	SPATIAL
#	represents nodes position,rotation & scale in 3D space

public struct Spatial:
	public pos		as Vector3
	public scale	as single
	public rot		as Quaternion
	public static final Identity = Spatial( pos:Vector3.Zero, scale:1.0f, rot:Quaternion.Identity )
	
	#---	Simple parameter packers	---#
	public PackPos as Vector4:
		get: return Vector4(pos,scale)
	public PackRot as Vector4:
		get: return Vector4(rot.Xyz,rot.W)

	#---	Helper static methods		---#
	public static def Combine(ref a as Spatial, ref b as Spatial) as Spatial:
		sp as Spatial
		sp.combine(a,b)
		return sp
	public static def Qrot(ref v as Vector3, ref q as Quaternion) as Vector3:
		return (q * Quaternion(Xyz:v,W:0f) * Quaternion.Invert(q)).Xyz
	public static def Lerp(ref a as Spatial, ref b as Spatial, t as single) as Spatial:
		sp as Spatial
		sp.lerpDq(a,b,t)
		return sp
	public static def EulerQuat(v as Vector3) as Quaternion:
		return\
			Quaternion.FromAxisAngle( Vector3.UnitX, v.X )*\
			Quaternion.FromAxisAngle( Vector3.UnitY, v.Y )*\
			Quaternion.FromAxisAngle( Vector3.UnitZ, v.Z )

	#---	Operations		---#
	public def combine(ref a as Spatial, ref b as Spatial) as void:
		rot		= b.rot * a.rot
		scale	= b.scale * a.scale
		pos		= b.byPoint(a.pos)
	public def inverse() as void:
		rot		= Quaternion.Invert(rot)
		scale	= 1f / scale
		pos	= -scale * Vector3.Transform(pos,rot)
	public def lerp(ref a as Spatial, ref b as Spatial, k as single) as void:
		rot		= Quaternion.Slerp(a.rot, b.rot, k)
		scale	= (1f-k)*a.scale + k*b.scale
		pos		= Vector3.Lerp(a.pos, b.pos, k)
	public def lerpDq(ref a as Spatial, ref b as Spatial, k as single) as void:
		scale	= (1f-k)*a.scale + k*b.scale
		dq as DualQuat
		dq.lerp(a,b,k)
		dq.toUnitSpatial(self)
	public def byPoint(ref v as Vector3) as Vector3:
		return pos + scale * Vector3.Transform(v,rot)



#------------------------------------------
#	NODE
#	hierarchy element in space
#	has a parent, caches the world-space transform

public class Node( kri.ani.data.Player, IComparable[of Node] ):
	public	final name	as string
	private	parent		as Node = null
	private	lastSync	as uint	= 0
	public	local	= Spatial.Identity
	private	cached	= Spatial.Identity
	private	world	= Spatial.Identity
	
	public def constructor(str as string):
		name = str
	public def constructor(n as Node):
		name = n.name
		parent = n.parent
		local = n.local
	#TODO: explicit interface imp, when supported
	public def CompareTo(n as Node) as int:	#imp: IComparable
		return name.CompareTo(n.name)
	
	public static def SafeWorld(n as Node) as Spatial:
		return (n.World	if n else	Spatial.Identity)
	
	public Parent as Node:
		get: return parent
		set:
			parent = value
			return	if not value
			cached = parent.World
			touch()
	public World as Spatial:
		get:
			if parent:
				pw = parent.World
				if lastSync!=Stamp or pw != cached:
					lastSync = Stamp
					cached = pw
					world.combine(local,pw)
				return world
			else:	return local
	

#------------------------------------------
#	NODEBONE
#	bone node with pose sub-space

public class NodeBone(Node):
	public final bindPose	as Spatial
	private invWorldPose	as Spatial
	
	public def constructor(str as string, ref initPose as Spatial):
		super(str)
		local = bindPose = initPose
	public def constructor(nb as NodeBone):
		super(nb)
		local = bindPose = nb.bindPose

	public def genTransPose(ref sloc as Spatial, ref sp as Spatial) as void:
		sp.combine(sloc,invWorldPose)	# object local -> pose
	public def bakeInvPose(ref s as Spatial) as void:
		pp = World
		pp.inverse()
		invWorldPose.combine(s,pp)


#------------------------------------------
#	SKELETON
#	manages bones

public class Skeleton( ani.data.Player, INoded ):
	public final bones	as (NodeBone)
	public final node	as Node
	INoded.Node as Node:
		get: return node
	
	public def constructor(n as Node, num as int):
		node = n
		bones = array[of NodeBone](num)
	public def constructor(s as Skeleton):
		node = s.node
		bones = s.bones.Clone() as (NodeBone)
		for i in range( bones.Length ):
			par = bones[i].Parent
			if not par:	continue
			ind = Array.IndexOf(s.bones, par)
			#ind = Array.BinarySearch(s.nodes, par)
			if ind<0:	continue
			bones[i].Parent = bones[ind]

	public def bakePoseData(np as Node) as void:
		sw = np.World
		for b in bones:
			b.bakeInvPose(sw)
		touch()
	public def reset() as void:
		for b in bones:
			b.local = b.bindPose
			b.touch()
		touch()
