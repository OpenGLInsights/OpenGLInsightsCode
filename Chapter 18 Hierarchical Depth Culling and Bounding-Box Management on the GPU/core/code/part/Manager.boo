namespace kri.part

import System.Collections.Generic
import OpenTK.Graphics.OpenGL


#---------------------------------------#
#	ABSTRACT PARTICLE MANAGER			#
#---------------------------------------#

public class Manager( kri.IMeshed ):
	public	final tf	= kri.TransFeedback(1)
	public	final va	= kri.vb.Array()
	public	final behos	= List[of Behavior]()
	public	final dict	= kri.shade.par.Dict()
	public	final mesh	= kri.Mesh( BeginMode.Points )

	public	final col_init		= kri.shade.Collector()
	public	final col_update	= kri.shade.Collector()

	private parTotal	= kri.shade.par.Value[of single]('part_total')
	public	Total	as uint:
		get: return mesh.nVert
	public	Ready	as bool:
		get: return mesh.buffers.Count==1 and col_init.Ready and col_update.Ready
	
	kri.IMeshed.Mesh as kri.Mesh:
		get: return mesh
	
	public def constructor(num as uint):
		mesh.nVert = num
		dict.var(parTotal)
	
	public def initMesh(m as kri.Mesh) as void:
		assert m and mesh.buffers.Count
		if m.buffers.Count:
			m.buffers[0].Semant.Clear()
		else:
			m.buffers.Add( kri.vb.Attrib() )
		m.nVert = Total
		m.buffers[0].Semant.AddRange( mesh.buffers[0].Semant )
		m.allocate()
	
	public def seBeh[of T(Behavior)]() as T:
		for beh in behos:
			bt = beh as T
			return bt	if bt
		return null	as T

	public def init(pc as Context) as void:
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
			col.absorb[of Behavior](behos)
			col.compose( vob.Semant, dict, kri.Ant.Inst.dict )
	
	protected def process(pe as Emitter, bu as kri.shade.Bundle) as bool:
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
		mesh.buffers[0] = pe.mesh.buffers[0]
		pe.mesh.buffers[0] = data
		return true

	public def process(pe as Emitter) as bool:
		if not pe.filled:
			pe.filled = process( pe, col_init.bu )
			return pe.filled
		return process( pe, col_update.bu )
