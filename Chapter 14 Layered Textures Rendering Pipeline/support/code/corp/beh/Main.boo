namespace support.corp.beh

import OpenTK
import kri.shade

#-------------------------------------------#
#	STANDARD FOR LOADED PARTICLES			#
#-------------------------------------------#

public class Standard( kri.part.Behavior ):
	public final parLife	= par.Value[of Vector4]('part_life')
	public final parVelTan	= par.Value[of Vector4]('part_speed_tan')
	public final parVelObj	= par.Value[of Vector4]('part_speed_obj')
	public final parVelKeep	= par.Value[of Vector4]('object_speed')

	public def constructor(pc as kri.part.Context):
		super('/part/beh/main')
		enrich(2,'sub')
		enrich(3,'pos','speed')

	public def constructor(std as Standard):
		super(std)	#is that enough?

	public override def link(d as par.Dict) as void:
		d.var(parLife, parVelTan, parVelObj, parVelKeep)
