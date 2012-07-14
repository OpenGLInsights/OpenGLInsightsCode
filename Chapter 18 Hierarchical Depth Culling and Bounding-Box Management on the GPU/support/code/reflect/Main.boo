namespace support.reflect

import kri.shade


#----------------------------------------
#	Reflection data, stored as an entity tag

public class Tag( kri.ITag ):
	public final pTex	as par.ValuePure[of kri.buf.Texture]
	public counter	as int		= 0
	public depth	as byte		= 0
	public size		as ushort	= 256
	public zoom		as single	= 1f
	public rangeIn	as single	= 1f
	public rangeOut	as single	= 100f
	public cubic	as bool		= true
	public def constructor(pt as par.ValuePure[of kri.buf.Texture]):
		pTex = pt


#----------------------------------------
#	Engine extension - loader

public class Extra( kri.IExtension ):
	def kri.IExtension.attach(nt as kri.load.Native) as void:
		nt.readers['mt_env']	= pmt_env

	#---	Texture: environment	---#
	public def pmt_env(r as kri.load.Reader) as bool:
		u = r.geData[of kri.meta.AdUnit]()
		return false	if not u
		tag = Tag(u)
		tag.counter = 0 - r.getByte()
		tag.depth	= r.getByte()
		tag.size	= r.bin.ReadUInt16()
		tag.zoom	= r.getReal()
		tag.rangeIn		= r.getReal()
		tag.rangeOut	= r.getReal()
		tag.cubic	= r.getString() == 'CUBE'
		r.addResolve() do(n as kri.Node):
			ent = r.at.scene.entities.Find() do(e as kri.Entity):
				e.node == n
			assert ent
			ent.tags.Add(tag)
		return true
