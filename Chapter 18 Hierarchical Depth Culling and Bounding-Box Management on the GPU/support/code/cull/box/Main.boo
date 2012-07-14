namespace support.cull.box

###	Entity tag - a sign of bounding box update	###

public class Tag( kri.ITag ):
	public	fresh		= false
	private	index		as int	= -1
	private	bufHandle	as uint	= 0
	private	stampBuf	as uint	= 0
	private	stampNode	as uint = 0
	
	public Index	as int:
		get: return index
		set:
			assert index<0
			index = value
	
	public def checkBuf(bv as kri.vb.Object) as bool:
		if bufHandle != bv.handle:
			bufHandle = bv.handle
			stampBuf = 0
		if stampBuf == bv.TimeStamp:
			return false
		stampBuf = bv.TimeStamp
		return true
	
	public def checkNode(n as kri.Node) as bool:
		if not n: return false
		if n.Stamp == stampNode:
			return false
		stampNode = n.Stamp
		return true
