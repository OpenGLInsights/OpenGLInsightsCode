namespace kri.part

import System.Collections.Generic
import OpenTK.Graphics.OpenGL
import kri.shade

#---------------------------------------#
#	PARTICLE GENERIC BEHAVIOR			#
#---------------------------------------#

public class Behavior( kri.meta.IBaseMat, kri.meta.IShaded, kri.vb.ISemanted, Code ):
	private final semantics	= List[of kri.vb.Info]()
	private final sh		as Object
	
	def System.ICloneable.Clone() as object:
		return Behavior(self)
	kri.INamed.Name:
		get: return 'behavior'
	
	kri.meta.IShaded.Shader as kri.shade.Object:
		get: return sh
	kri.vb.ISemanted.Semant as List[of kri.vb.Info]:
		get: return semantics

	public def constructor():
		super( CodeNull() )
		sh = null
	public def constructor(path as string):
		super(path)
		sh = Object( ShaderType.VertexShader, path, Text )
	public def constructor(b as Behavior):
		super(b)
		semantics.AddRange( b.Semant )
		sh = b.sh
	
	public virtual def link(d as par.Dict) as void:	#imp: kri.meta.IBase
		pass
	public def enrich(size as byte, *names as (string)) as void:
		kri.Help.enrich(self,size,*names)



#---------------------------------------#
#	PARTICLE CREATION CONTEXT			#
#---------------------------------------#

public class Context:
	# root shaders
	public final	sh_init		= Object.Load('/part/init_v')
	public final	sh_draw		= Object.Load('/part/draw/main_v')
	public final	sh_root		= Object.Load('/part/root_v')
	public final	sh_tool		= Object.Load('/part/tool_v')
	public final	sh_child	= Object.Load('/part/child_v')
	# fur shaders
	public final	sh_fur_init	= Object.Load('/part/fur/init_v')
	public final	sh_fur_root	= Object.Load('/part/fur/root_v')
	# born shaders
	public final	sh_born_instant	= Object.Load('/part/born/instant_v')
	public final	sh_born_static	= Object.Load('/part/born/static_v')
	public final	sh_born_time	= Object.Load('/part/born/time_v')
	public final	sh_born_loop	= Object.Load('/part/born/loop_v')
	# emit surface shaders
	public final	sh_surf_node	= Object.Load('/part/surf/node_v')
	public final	sh_surf_vertex	= Object.Load('/part/surf/vertex_v')
	public final	sh_surf_edge	= Object.Load('/part/surf/edge_v')
	public final	sh_surf_face	= Object.Load('/part/surf/face_v')

	# helper methods
	public def makePart(cin as Collector, cup as Collector) as void:
		cin.root = sh_init
		cin.mets['init'] = DefMethod.Void
		cup.root = sh_root
		cup.extra.Add( sh_tool )
		cup.mets['reset']	= DefMethod.Float
		cup.mets['update']	= DefMethod.Float
	
	public def makeHair(cin as Collector, cup as Collector) as void:
		cin.mets['init']	= DefMethod.Void
		cin.root	= sh_fur_init
		cup.mets['update']	= DefMethod.Float
		cup.root	= sh_fur_root
