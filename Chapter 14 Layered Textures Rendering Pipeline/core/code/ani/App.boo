namespace kri.ani

import System
import OpenTK

###		Counter		###

public class Counter(Basic):
	public final limit	as int
	public count	= 0
	public def constructor(lim as int):
		limit = lim
	def IBase.onTick(time as double) as uint:
		count = limit	if not count
		--count
		return (2,1)[count>0]


###		Compensate parent	###

public class Compensate( Action ):
	public final node	as kri.Node
	public def constructor(n as kri.Node):
		node = n
	protected override def execute() as void:
		par = node.Parent
		assert par
		sp = par.local
		par.local.combine(node.local,sp)
		par.touch()
		node.local = kri.Spatial.Identity
	
	public static def Wrap(n as kri.Node, name as string, loops as int) as IBase:
		ag = Graph()
		c0 = ag.init.append( n.play(name) )
		c1 = c0.append( Compensate(n) )
		c2 = c1.append( Counter(loops) )
		c2.to.Add(ag.init)
		c2.to.Add(null)
		return ag


#-------------------------------#
#	Rotate node with mouse		#
#-------------------------------#

public class ControlMouse(Basic):
	private final node	as kri.Node
	private final cam	as kri.Camera
	private final sense	as single
	private final mouse as Input.MouseDevice
	private active	= false
	private x	= -1
	private y	= -1
	private base	as kri.Spatial
	
	public def constructor(rat as Input.MouseDevice, n as kri.Node, c as kri.Camera, sen as single):
		assert n
		mouse=rat; node=n; cam=c; sense=sen
		mouse.ButtonDown	+= def():
			x = mouse.X
			y = mouse.Y
			base = n.local
			active = true
		mouse.ButtonUp		+= def():
			active = false
		
	def IBase.onTick(time as double) as uint:
		return 0	if not active

		dx = cast(single, mouse.X-x)
		dy = cast(single, mouse.Y-y)
		axis	= Vector3(dy, dx, 0f)
		size	= sense * axis.LengthFast
		
		qrot	= Quaternion.FromAxisAngle(axis, size)
		n		= cam.node
		s		= (n.World	if n else kri.Spatial.Identity)
		qrot	= s.rot * qrot * Quaternion.Invert(s.rot)
		s		= base
		s.rot	= qrot * s.rot
		node.local = s
		node.touch()
		return 0
