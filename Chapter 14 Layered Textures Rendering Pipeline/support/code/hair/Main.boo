namespace support.hair

import OpenTK
import OpenTK.Graphics.OpenGL

#-----------------------------------#
#		Hair baking Tag				#
#-----------------------------------#

public class Tag( kri.ITag ):
	public	final va	= kri.vb.Array()
	public	final tf	= kri.TransFeedback(1)
	public	final data	= kri.vb.Attrib()
	public	final mesh	= kri.Mesh( BeginMode.Points )
	# XYZ: tangent space direction, W: randomness
	public	param	= Vector4.UnitZ
	private	stamp	as double	= -1f
	public	Ready	as bool:
		get: return stamp>=0f
	public	Pixels	as uint:
		get: return mesh.nVert

	public def constructor(size as uint):
		# init fake vertex attrib for drawing
		mesh.buffers.Add( vat = kri.vb.Attrib() )
		vat.Semant.Add( kri.vb.Info.Dummy )
		mesh.nVert = size
		mesh.allocate()
		# init output buffer
		for i in range(2):
			kri.Help.enrich( data, 3, ('root_prev','root_base')[i] )
		data.initUnit(size)
	
	public def bake(bu as kri.shade.Bundle) as void:
		tf.Bind( data )
		using tf.discard():
			mesh.render(va,bu,tf)
		stamp = kri.Ant.Inst.Time
