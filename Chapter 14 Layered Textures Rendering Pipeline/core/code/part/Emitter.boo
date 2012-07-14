namespace kri.part

import System
import System.Collections.Generic
import OpenTK.Graphics.OpenGL


#---------------------------------------#
#	PARTICLE EMITTER 					#
#---------------------------------------#

public class Emitter( kri.vb.IProvider, kri.INoded, kri.IMeshed, kri.ani.IBase ):
	public	visible		as bool		= true
	public	filled		as bool		= false
	public	obj			as kri.Entity	= null
	public	mat			as kri.Material	= null
	public	final owner	as Manager
	public	final name	as string
	public	final entries	= kri.vb.Dict()
	public	final techReady	= Dictionary[of string,bool]()
	public	final mesh		= kri.Mesh( BeginMode.Points )
	public	onUpdate	as callable(kri.Entity) as bool	= null

	kri.INoded.Node as kri.Node:
		get: return (obj.node	if obj else null)
	kri.IMeshed.Mesh as kri.Mesh:
		get: return mesh
	kri.vb.IProvider.Data		as kri.vb.Object:
		get: return mesh.buffers[0]
	kri.vb.ISemanted.Semant	as List[of kri.vb.Info]:
		get: return (mesh.buffers[0].Semant	if mesh.buffers.Count	else null)
	public	Ready as bool:
		get: return mesh.buffers.Count>0 and Data.Allocated>0

	public def constructor(pm as Manager, str as string):
		owner,name = pm,str
	public def constructor(pe as Emitter):
		visible	= pe.visible
		obj		= pe.obj
		mat		= pe.mat
		owner	= pe.owner
		name	= pe.name
	
	public def update() as bool:
		return (not onUpdate) or onUpdate(obj)
	
	public def allocate() as void:
		assert owner
		owner.initMesh( mesh )
		entries.Clear()
		mesh.fillEntries(entries)
	
	public def draw(bu as kri.shade.Bundle, num as uint) as bool:
		if not (bu and update()):
			return false
		return mesh.render( owner.va, bu, entries, num, null )

	def kri.ani.IBase.onTick(time as double) as uint:
		kri.Ant.Inst.params.parTime.Value.Y = time
		if not (owner and owner.Ready):
			kri.lib.Journal.Log("Particle: invalid manager for animating '${name}'")
			return 1
		if not owner.process(self):
			kri.lib.Journal.Log("Particle: failed to process '${name}'")
			return 2
		return 0

	def kri.ani.IBase.onKill() as void:
		filled = false
