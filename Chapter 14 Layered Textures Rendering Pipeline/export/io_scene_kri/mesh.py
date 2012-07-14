__author__ = ['Dzmitry Malyshau']
__bpydoc__ = 'Mesh module of KRI exporter.'

import mathutils
from io_scene_kri.common	import *


def calc_TBN(verts, uvs):
	va = verts[1].co - verts[0].co
	vb = verts[2].co - verts[0].co
	n0 = n1 = va.cross(vb)
	tan,bit,hand = None,None,0.0
	if len(uvs) and n1.dot(n1) > 0.0:
		ta = uvs[0][1] - uvs[0][0]
		tb = uvs[0][2] - uvs[0][0]
		tan = va*tb.y - vb*ta.y
		if tan.length_squared>0.0:
			bit = vb*ta.x - va*tb.x
			n0 = tan.cross(bit)
			hand = (-1.0 if n0.dot(n1) < 0.0 else 1.0)
		else:	tan = None
	return (tan, bit, n0, hand, n1)


class Vertex:
	__slots__= 'face', 'vert','vert2', 'coord', 'tex', 'color', 'normal', 'quat', 'dual'
	def __init__(self, v):
		self.face = None
		self.vert = v
		self.vert2 = None
		self.coord = v.co
		self.tex = None
		self.color = None
		self.normal = v.normal
		self.quat = None
		self.dual = -1


class Face:
	__slots__ = 'v', 'vi', 'no', 'uv', 'color', 'mat', 'ta', 'hand', 'normal', 'wes'
	def __init__(self, face, m, ind = None, uves = None, colors = None):
		if not ind: # clone KRI face
			self.vi		= list(face.vi)
			self.hand	= face.hand
			self.mat	= face.mat
			return
		# this section requires optimization!
		# hint: try 'Recalculate Outside' if getting lighting problems
		self.mat = face.material_index
		self.vi = [ face.vertices[i]	for i in ind   ]
		self.v  = tuple( m.vertices[x]	for x in self.vi )
		self.no = tuple( x.normal	for x in self.v  )
		self.normal = ( face.normal, mathutils.Vector((0,0,0)) )[face.use_smooth]
		xuv	= tuple(tuple( layer[i]	for i in ind ) for layer in uves)
		color	= tuple(tuple( layer[i]	for i in ind ) for layer in colors)
		t,b,n,hand,nv = calc_TBN(self.v, xuv)
		self.wes = tuple( 3 * [0.1+nv.length_squared] )
		if Settings.putQuat and t != None:
			self.ta = t.normalized()
		else:	self.ta = None
		self.hand = hand
		self.uv		= ([],xuv)	[Settings.putUv]
		self.color	= ([],color)	[Settings.putColor]


###  MESH   ###

def save_mesh(mesh,armature,groups):
	out = Writer.inst
	# 1: convert Mesh to Triangle Mesh
	for layer in mesh.uv_textures:
		if not len(layer.data):
			out.log(1,'e','UV layer is locked by the user')
			return
	hasQuat = len(mesh.uv_textures)>0 and Settings.putQuat
	ar_face = []
	for i,face in enumerate(mesh.faces):
		uves,colors,nvert = [],[],len(face.vertices)
		for layer in mesh.uv_textures:
			d = layer.data[i]
			cur = tuple(mathutils.Vector(x) for x in (d.uv1,d.uv2,d.uv3,d.uv4))
			uves.append(cur)
		for layer in mesh.vertex_colors:
			d = layer.data[i]
			cur = tuple(mathutils.Vector(x) for x in (d.color1,d.color2,d.color3,d.color4))
			colors.append(cur)
		if nvert>=3:	ar_face.append( Face(face, mesh, (0,1,2), uves,colors) )
		if nvert>=4:	ar_face.append( Face(face, mesh, (0,2,3), uves,colors) )
	#else: out.logu(1,'converted to tri-mesh')
	if not 'ClearNonUV':
		n_bad_face =	len(ar_face)
		ar_face = list(filter( lambda f: f.ta!=None, ar_face ))
		n_bad_face -=	len(ar_face)
		if n_bad_face:
			out.log(1,'w','%d pure faces detected' % (n_bad_face))
	if not len(ar_face):
		out.log(1,'e','object has no faces')
		return

	# 2: fill sparsed vertex array
	avg,set_vert = 0.0,{}
	for face in ar_face:
		avg += face.hand
		nor = face.normal
		for i in range(3):
			v = Vertex( face.v[i] ) 
			v.normal = (nor if nor.length_squared>0.1 else face.no[i])
			v.tex	= [layer[i] for layer in face.uv]
			v.color	= [layer[i] for layer in face.color]
			v.face = face
			vs = str((v.coord,v.tex,v.color,v.normal,face.hand))
			if not vs in set_vert:
				set_vert[vs] = []
			set_vert[vs].append(v)
	out.log(1,'i', '%.2f avg handness' % (avg / len(ar_face)))
	
	# 3: update triangle indexes
	avg,ar_vert,bad_vert = 0.0,[],0
	for i,vgrup in enumerate(set_vert.values()):
		v = vgrup[0]
		tan,lensum = mathutils.Vector((0,0,0)),0.0
		for v2 in vgrup:
			f = v2.face
			ind = f.v.index(v2.vert)
			f.vi[ind] = i
			wes = f.wes[ind]
			if f.ta:
				lensum += wes * f.ta.length
				tan += wes * f.ta
		no = v.normal.copy()
		no.normalize()
		if hasQuat and lensum>0.0:
			avg += tan.length / lensum
			tan.normalize()		# mean tangent
			bit = no.cross(tan) * v.face.hand	# using handness
			tan = bit.cross(no)	# handness will be applied in shader
			tbn = mathutils.Matrix((tan,bit,no))	# tbn is orthonormal, right-handed
			v.quat = tbn.to_quaternion().normalized()
		elif hasQuat:
			bad_vert += 1
			v.quat = mathutils.Quaternion((0,0,0,1))
		ar_vert.append(v)
	if bad_vert:
		out.log(1,'w','%d pure vertices detected' % (bad_vert))
	if hasQuat:
		out.log(1,'i','%.2f avg tangent accuracy' % (avg / len(ar_vert)))
	del set_vert

	# 4: unlock quaternions to make all the faces QI-friendly
	def qi_check(f):	# check Quaternion Interpolation friendliness
		qx = tuple( ar_vert[x].quat for x in f.vi )
		assert qx[0].dot(qx[1]) >= 0 and qx[0].dot(qx[2]) >= 0
	def mark_used(ind):	# mark quaternion as used
		v = ar_vert[ind]
		if v.dual < 0: v.dual = ind
	n_dup,ex_face = 0,[]
	for f in ([],ar_face)[hasQuat]:
		vx,cs,pos,n_neg = (1,2,0),[0,0,0],0,0
		def isGood(j):
			ind = f.vi[j]
			vi = ar_vert[ind]
			if vi.dual == ind: return False	# used, no clone
			if vi.dual < 0: vi.quat.negate()	# not used
			else:   f.vi[j] = vi.dual	# clone exists
			return True
		def duplicate():
			src = ar_vert[ f.vi[pos] ]
			dst = Vertex(src.vert)
			dst.face = f
			dst.tex = tuple( layer.copy() for layer in src.tex )
			dst.quat = src.quat.copy()
			dst.quat.negate()
			dst.dual = f.vi[pos]
			f.vi[pos] = src.dual = len(ar_vert)
			ar_vert.append(dst)
			return 1
		for j in range(3):
			qx = tuple( ar_vert[f.vi[x]].quat for x in (vx[j],vx[vx[j]]) )
			cs[j] = qx[0].dot(qx[1])
			if cs[j] > cs[pos]: pos = j
			if(cs[j] < 0): n_neg += 1
		#print ("\t %d: %.1f, %.1f, %.1f" % (pos,cs[0],cs[1],cs[2]))
		if n_neg == 2 and not isGood(pos):   # classic duplication case
			n_dup += duplicate()
		if n_neg == 3:  # extremely rare case
			pos = next((j for j in range(3) if isGood(j)), -1)
			if pos < 0:
				pos = 1
				n_dup += duplicate()
			cs[vx[pos]] *= -1
			cs[vx[vx[pos]]] *= -1
			n_neg -= 2
		if n_neg == 1: # that's a bad case
			pos = min((x,j) for j,x in enumerate(cs))[1]
			# prepare
			ia,ib = vx[pos],vx[vx[pos]]
			va = ar_vert[ f.vi[ia] ]
			vb = ar_vert[ f.vi[ib] ]
			vc = ar_vert[ f.vi[pos] ]
			# create mean vertex
			v = Vertex( vc.vert )
			v.vert = va.vert
			v.vert2 = vb.vert
			n_dup += 1
			v.face = f
			v.coord = 0.5 * (va.coord + vb.coord)
			v.quat = va.quat + vb.quat
			v.quat.normalize()
			v.tex = tuple( 0.5*(a[0]+a[1]) for a in zip(va.tex,vb.tex) )
			# create additional face
			f2 = Face( f, mesh )
			mark_used( f.vi[ia] )	# caution: easy to miss case
			v.dual = f.vi[ia] = f2.vi[ib] = len(ar_vert)
			# it's mathematically proven that both faces are QI friendly now!
			ar_vert.append(v)
			ex_face.append(f2)
		# mark as used
		for ind in f.vi: mark_used(ind)

	if Settings.doQuatInt and hasQuat:
		out.log(1,'i', 'extra %d vertices, %d faces' % (n_dup,len(ex_face)))
		ar_face += ex_face
		# run a check
		for f in ar_face: qi_check(f)
	del ex_face

	# 5: face indexes
	ar_face.sort(key = lambda x: x.mat)
	face_num = (len(mesh.materials)+1) * [0]
	for face in ar_face:
		face_num[face.mat] += 1
	out.logu(1, 'total: %d vertices, %d faces' % (len(ar_vert),len(ar_face)))
	avg_vu = 3.0 * len(ar_face) / len(ar_vert)
	out.log(1,'i', '%.2f avg vertex usage' % (avg_vu))
	out.begin('mesh')
	out.pack('H', len(ar_vert) )
	out.end()
	
	if 'putVertex':
		out.begin('v_pos')
		for v in ar_vert:
			out.pack('4f', v.coord.x, v.coord.y, v.coord.z, v.face.hand)
		out.end()
	if Settings.putNormal:
		out.begin('v_nor')
		for v in ar_vert:
			out.pack('3f', v.normal.x, v.normal.y, v.normal.z)
		out.end()
	if hasQuat:
		out.begin('v_quat')
		for v in ar_vert:
			out.pack('4f', v.quat.x, v.quat.y, v.quat.z, v.quat.w)
		out.end()
	
	if Settings.putUv:
		all = mesh.uv_textures
		out.log(1,'i', 'UV layers: %d' % (len(all)))
		for i,layer in enumerate(all):
			out.begin('v_uv')
			out.text( layer.name )
			for v in ar_vert:
				assert i<len(v.tex)
				out.pack('2f', v.tex[i].x, v.tex[i].y)
			out.end()
	if Settings.putColor:
		all = mesh.vertex_colors
		out.log(1,'i', 'Color layers: %d' % (len(all)))
		for i,layer in enumerate(all):
			out.begin('v_color')
			out.text( layer.name )
			for v in ar_vert:
				assert i<len(v.color)
				save_color(v.color[i])
			out.end()
	if not 'putSticky':
		all = mesh.sticky
		out.log(1,'i', 'Sticky layers: %d' % (len(all)))
		for i in range(len(all)):
			out.begin('v_sticky')
			for v in ar_vert:
				pass
			out.end()

	out.begin('v_ind')
	out.pack('H', len(ar_face))
	for face in ar_face:
		out.array('H', face.vi)
	out.end()

	# 6: materials
	out.begin('entity')
	for fn,m in zip(face_num,mesh.materials):
		if not fn: break
		out.pack('H', fn)
		s = (m.name	if m else '')
		out.text(s)
		out.logu(1, '+entity: %d faces, [%s]' % (fn,s))
	out.pack('H',0)
	out.end()
	
	# 7: shape keys
	shapes = mesh.shape_keys
	for sk in (shapes.keys if shapes else []):
		out.logu(1, '+shape: %.3f [%s]' % (sk.value,sk.name))
		out.begin('v_shape')
		out.text( sk.name )
		rel_id = list(shapes.keys).index(sk.relative_key)
		out.pack('Bf', rel_id, sk.value )
		for v in ar_vert:
			pos = sk.data[v.vert.index].co
			if v.vert2:
				p2 = sk.data[v.vert2.index].co
				pos = 0.5*(pos+p2)
			out.pack('3f', pos.x, pos.y, pos.z)
		out.end()

	# 8: bone weights
	if not armature: return
	out.begin('v_skin')
	nempty, avg = 0, 0.0
	for v in ar_vert:
		nw = len(v.vert.groups)
		avg += nw
		if not nw:
			nempty += 1
			out.array('H',[255,0,0,0])
			continue
		bone = sorted(v.vert.groups, key=lambda x: x.weight, reverse=True) [:min(4,nw)]
		left, total = 255, sum(b.weight for b in bone)
		for i in range(4):
			bid,weight = 0,0
			if i < len(bone):
				name = groups[ bone[i].group ].name
				bid = armature.bones.keys().index(name) + 1
				weight = int(255.0 * bone[i].weight / total + 0.5)
			if i==3: weight = left
			else: weight = min(left,weight)
			left -= weight
			assert weight>=0 and weight<256
			out.pack('2B', weight,bid)
	avg /= len(ar_vert)
	out.end()
	out.logu(1, 'bone weights: %d empty, %.1f avg' % (nempty,avg))
