namespace support.corp

import System.Collections.Generic


#---------------------------------------#
#	ABSTRACT PARTICLE MANAGER			#
#---------------------------------------#

private class MeshHolder( kri.IMeshed ):
	public	final mesh	= kri.Mesh()	//points
	
	public	Total	as uint:
		get: return mesh.nVert
	kri.IMeshed.Mesh as kri.Mesh:
		get: return mesh
	
	public def constructor(n as uint):
		mesh.nVert = n
	
	public def initMesh(m as kri.Mesh) as void:
		assert m and mesh.buffers.Count
		if m.buffers.Count:
			m.buffers[0].Semant.Clear()
		else:
			m.buffers.Add( kri.vb.Attrib() )
		m.nVert = Total
		m.buffers[0].Semant.AddRange( mesh.buffers[0].Semant )
		m.allocate()
	
	public def checkMesh(m as kri.Mesh) as bool:
		return m.buffers.Count>0 and m.buffers[0].Allocated>0
	
	public def insureMesh(m as kri.Mesh, vd as kri.vb.Dict) as void:
		if checkMesh(m):	return
		initMesh(m)
		if not vd:			return
		vd.Clear()
		mesh.fillEntries(vd)

	

public class Manager( MeshHolder ):
	public	final tf	= kri.TransFeedback(1)
	public	final va	= kri.vb.Array()
	public	final behos	= List[of kri.part.Behavior]()
	public	final dict	= kri.shade.par.Dict()

	public	final col_init		= kri.shade.Collector()
	public	final col_update	= kri.shade.Collector()

	private parTotal	= kri.shade.par.Value[of single]('part_total')
	public	Ready	as bool:
		get: return mesh.buffers.Count==1 and col_init.Ready and col_update.Ready
	
	
	public def constructor(num as uint):
		super(num)
		dict.var(parTotal)
	
	public def seBeh[of T(kri.part.Behavior)]() as T:
		for beh in behos:
			bt = beh as T
			return bt	if bt
		return null	as T

	public def init(pc as kri.part.Context) as void:
		# collect attributes
		mesh.buffers.Clear()
		mesh.buffers.Add( vob = kri.vb.Attrib() )
		for b in behos:
			vob.Semant.AddRange( (b as kri.vb.ISemanted).Semant )
			b.link(dict)
		mesh.allocate()
		# collect shaders
		for col in (col_init,col_update):
			col.bu.clear()
			col.absorb[of kri.part.Behavior](behos)
			col.compose( vob.Semant, dict, kri.Ant.Inst.dict )
	
	protected def process(pe as Emitter, target as kri.Mesh, bu as kri.shade.Bundle) as bool:
		if not (Ready and pe.update()):
			return false
		if not tf.Bind( mesh.buffers[0] ):
			return false
		parTotal.Value = (0f, 1f / (Total-1))[ Total>1 ]
		using tf.discard():
			if not mesh.render( va, bu, pe.entries, 0, tf ):
				return false
		if not 'Debug':
			ar = array[of single]( Total * mesh.buffers[0].countSize() >>2 )
			mesh.buffers[0].read(ar,0)
		# swap data
		mesh.fillEntries( pe.entries )
		data = mesh.buffers[0]
		mesh.buffers[0] = target.buffers[0]
		target.buffers[0] = data
		return true

	public def process(pe as Emitter, target as kri.Mesh) as bool:
		rez = false
		if pe.state == State.Unknown:
			rez = process( pe, target, col_init.bu )
			if rez:	pe.state = State.Filled
		if pe.state == State.Filled:
			rez = process( pe, target, col_update.bu )
		if not rez: pe.state = State.Invalid
		return rez
