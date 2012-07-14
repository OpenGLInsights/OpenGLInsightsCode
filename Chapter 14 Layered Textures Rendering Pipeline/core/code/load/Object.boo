namespace kri.load

public class ExObject( kri.IExtension ):
	def kri.IExtension.attach(nt as Native) as void:
		# objects
		nt.readers['node']		= p_node
		nt.readers['entity']	= p_entity
		nt.readers['skel']		= p_skel
		nt.readers['cam']		= p_cam
		nt.readers['lamp']		= p_lamp
		# physics
		nt.readers['collide']	= p_collide
		nt.readers['b_stat']	= pb_stat
		nt.readers['b_rigid']	= pb_rigid
	
	
	public static def GetProjector(r as Reader, p as kri.Projector) as void:
		p.rangeIn	= r.getReal()
		p.rangeOut	= r.getReal()
		p.fov		= r.getReal() * 0.5f

	#---	Parse entity	---#
	public def p_entity(r as Reader) as bool:
		off,n = 0,1
		m = r.geData[of kri.Mesh]()
		node = r.geData[of kri.Node]()
		if not (m and node):	return false	
		e = kri.Entity( node:node, mesh:m )
		r.at.scene.entities.Add(e)
		r.puData(e)
		mDef = r.at.mats['']
		while n:
			n = r.bin.ReadUInt16()
			tag = kri.TagMat( off:off, num:n )
			if n:
				tag.mat = r.getMaterial()
				off += n
			else:
				tag.num = m.nPoly - off
				tag.mat = mDef
				if not tag.num:	break
			e.tags.Add(tag)
		return true
	
	#---	Parse spatial node	---#
	public def p_node(r as Reader) as bool:
		n = kri.Node( r.getString() )
		r.at.nodes[n.name] = n
		r.puData(n)
		n.Parent = r.at.nodes[ r.getString() ]
		n.local = r.getSpatial()	# touched by Parent
		return true
	
	#---	Parse camera	---#
	public def p_cam(r as Reader) as bool:
		n = r.geData[of kri.Node]()
		return false	if not n
		c = kri.Camera( node:n )
		r.puData(c)
		r.getByte()	# is current
		GetProjector(r,c)
		r.at.scene.cameras.Add(c)
		return true

	#---	Parse light source	---#
	public def p_lamp(r as Reader) as bool:
		n = r.geData[of kri.Node]()
		return false	if not n
		l = kri.Light( node:n )
		r.puData(l)
		l.Color	= r.getColorByte()
		# attenuation
		l.factor0	= r.getReal()
		l.factor1	= r.getReal()
		l.factor2	= r.getReal()
		l.sphere	= r.getReal()
		# main
		type = r.getString()
		GetProjector(r,l)
		if type == 'SUN':
			l.makeOrtho( l.fov )
		elif type == 'POINT':
			l.fov = 0f
		l.softness	= r.getReal()
		r.at.scene.lights.Add(l)
		return true
	
	#---	Parse skeleton	---#
	public def p_skel(r as Reader) as bool:
		node = r.geData[of kri.Node]()
		return false	if not node
		nbones = r.getByte()
		s = kri.Skeleton( node,nbones )
		r.puData(s)
		# read nodes
		par = array[of byte](nbones)
		for i in range(nbones):
			name = r.getString()
			par[i] = r.getByte()
			s.bones[i] = kri.NodeBone(name, r.getSpatial())
		for i in range(nbones):
			s.bones[i].Parent = (s.bones[par[i]-1] if par[i] else node)
		s.bakePoseData(node)
		return true
	
	#------	PHYSICS	------#
	
	#---	Parse collision bounds	---#
	public def p_collide(r as Reader) as bool:
		r.getReal()	# margin
		r.getString()	# type
		return true
	
	#---	Parse static body	---#
	public def pb_stat(r as Reader) as bool:
		r.bin.ReadBytes(2)	# actor,reacts
		return true
	
	#---	Parse rigid body	---#
	public def pb_rigid(r as Reader) as bool:
		r.bin.ReadBytes(2)	# actor,reacts
		r.getReal()		# mass
		r.getReal()		# radius
		r.getReal()		# form factor
		r.getReal()		# moving damping
		r.getReal()		# rotation damping
		return true
