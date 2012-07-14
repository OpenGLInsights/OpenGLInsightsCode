namespace kri.ani.sim

import System
import OpenTK
#import NewtonWrapper

#-----------------------#
#	PHYSICS SECTION		#
#-----------------------#

public interface IField:
	def effect(ref s as kri.Spatial) as Vector3:
		pass


public class Basic(kri.ani.Delta):
	public final scene		as kri.Scene
	public final fLinear	= List[of IField]()
	public final fAngular	= List[of IField]()
	public def constructor(s as kri.Scene):
		scene = s
	private abstract def assembly(d as double, b as kri.Body,
	ref vlin as Vector3, ref vang as Vector3) as void:
		pass
	protected override def onDelta(delta as double) as uint:
		for b in scene.bodies:
			continue	if not b.node
			lin = ang = Vector3(0f,0f,0f)
			for f in fLinear:
				lin += f.effect( b.node.local )
			for f in fAngular:
				ang += f.effect( b.node.local )
			assembly( delta, b, lin, ang )
		return 0


public class Native(Basic):
	public def constructor(s as kri.Scene):
		super(s)
	private override def assembly(d as double, b as kri.Body,
	ref vlin as Vector3, ref vang as Vector3) as void:
		b.vLinear	+= d*vlin
		b.vAngular	+= d*vang
		q = Quaternion( Xyz:0.5f*d*b.vAngular, W:0f )
		n = b.node
		assert n
		n.local.pos += d * b.vLinear
		n.local.rot += q * n.local.rot
		n.local.rot.Normalize()
		n.touch()

/*
public class Newton(Basic):
	#private final world	= Newton.Create(0,0)
	public def constructor(s as kri.Scene):
		super(s)
	protected override def onDelta(delta as double) as uint:
		super(delta)
		#Newton.Update(world, cast(single,delta))
		return 0
	private override def assembly(ref s as kri.Spatial, d as double,
			b as kri.Body, *v as (Vector3)) as void:
		b.vLinear	+= d*v[0]
		b.vAngular	+= d*v[1]
		s.pos += d * b.vLinear
		s.rot += Quaternion(Xyz: 0.5f*d*b.vAngular, W:0f) * s.rot
		s.rot.Normalize()
*/