__author__ = ['Dzmitry Malyshau']
__bpydoc__ = 'Material module of KRI exporter.'

import bpy
from io_scene_kri.common	import *


###  NODE TREE   ###

def save_sockets(slist,orient):
	out = Writer.inst
	for sock in slist:
		out.begin('g_sock')
		out.pack('B',orient)
		out.text(sock.name)
		dv = sock.default_value
		if	sock.type=='VALUE':
			out.pack('Bf',1,dv)
		elif	sock.type=='VECTOR':
			out.pack('B3f',3,dv[0],dv[1],dv[2])
		elif	sock.type=='RGBA':
			out.pack('B',4)
			save_color(dv)
		else:	out.log(2,'w','bad socket type: '+sock.type)
		out.end()

def save_graph(nt):
	out = Writer.inst
	def save_io(io):
		save_sockets(io.inputs,0)
		save_sockets(io.outputs,1)
	def save_nid(node):
		nid = 0	# parent node index
		if node:	nid = 1+nt.nodes.values().index(node)
		out.pack('B',nid)
		return node
	out.begin('graph')
	out.text(nt.type)
	out.pack('B',len(nt.nodes))
	out.end()
	save_io(nt)
	for node in nt.nodes:
		ins = ','.join(x.name	for x in node.inputs)
		ots = ','.join(x.name	for x in node.outputs)
		out.logu(2,"%s: %s -> %s" % (node.name,ins,ots))
		tip = ''
		if node.__getattribute__('type'):
			tip = node.type
		out.begin('g_node')
		out.text(tip, node.name, node.label)
		save_nid(node.parent)
		out.end()
		save_io(node)
	out.begin('g_links')
	out.pack('B',len(nt.links))
	for link in nt.links:
		if save_nid( link.from_node ):
			out.pack('B', link.from_node.outputs.values().index( link.from_socket ))	
		if save_nid( link.to_node ):
			out.pack('B', link.to_node.inputs.values().index( link.to_socket ))	
	out.end()
	return


###  MATERIAL:UNIT   ###

def find_uv_layer(mtex,name):
	# dirty: resolving the UV layer ID
	for ent in bpy.context.scene.objects:
		if ent.type != 'MESH': continue
		mlist = []	# need 'flat' function
		for ms in ent.material_slots:
			mlist.extend( ms.material.texture_slots )
		if not mtex in mlist: continue
		uves = [ut.name for ut in ent.data.uv_textures]
		if name in uves:
			return uves.index( name )
		out.log(2,'w','entity (%s) has incorrect UV names' % (ent.name))
	return -1


def save_mat_unit(mtex):
	out = Writer.inst
	# map input chunk
	out.begin('unit')
	colored = ('diffuse','emission','spec','reflection')
	flat = ['normal','warp','displacement','mirror','hardness','diffuse','specular','emit']
	xcolor = 'color_'
	supported = flat + list(xcolor + x for x in colored)
	current = {}
	#list(x for x in supported	if mtex.__getattribute__('use_map_'+x))
	#out.text( *(current+['']) )
	infoStr = 'affects:'
	for x in supported:
		enabled = mtex.__getattribute__('use_map_'+x)
		#x = x.replace('spec','specular')
		if not enabled: continue
		if x.startswith(xcolor):
			fname = x.replace(xcolor,'') + '_' + xcolor
		else:	fname = x+'_'
		factor = mtex.__getattribute__(fname+'factor')
		out.text(x)
		out.pack('f',factor)
		infoStr += " %s(%.2f)," % (x,factor)
	out.text('')	# terminator
	out.logu(2, infoStr)
	tc,mp = mtex.texture_coords, mtex.mapping
	out.logu(2, '%s input, %s mapping' % (tc,mp))
	out.text(tc,mp)
	if tc == 'UV':
		lid,name = 0,mtex.uv_layer
		if len(name):
			lid = find_uv_layer(mtex,name)
			if lid == -1:
				out.log(2,'w','failed to resolve UV layer')
				lid = 0
			else:	out.logu(2, 'layer: %s -> %d' % (name,lid))
		else:	out.log(2,'w','UV layer name is not set')
		out.pack('B',lid)
	if tc == 'OBJECT':	out.text( mtex.object.name )
	out.end()
	out.begin('u_apply')
	out.pack('B', mtex.use )
	out.text( mtex.blend_type, mtex.normal_map_space )
	out.pack('3B', mtex.use_rgb_to_intensity, mtex.invert, mtex.use_stencil )
	save_color( mtex.color )
	out.pack('f', mtex.default_value )
	out.end()
	



###  MATERIAL:TEXTURE TYPES   ###

def save_mat_image(mtex):
	out = Writer.inst
	it = mtex.texture
	assert it
	out.logu(2, 'type: ' + it.type)
	hasNormals = True
	# tex mapping
	out.begin('t_map')
	if mtex.mapping_x != 'X' or mtex.mapping_y != 'Y' or mtex.mapping_z != 'Z':
		out.log(2,'w','tex coord swizzling not supported')
	out.array('f', tuple(mtex.offset) + tuple(mtex.scale) )
	out.end()
	# colors
	out.begin('t_color')
	out.pack('3f', it.factor_red, it.factor_green, it.factor_blue )
	out.pack('3f', it.intensity, it.contrast, it.saturation )
	out.end()
	# ramp
	if it.use_color_ramp:
		ramp = it.color_ramp
		num = len(ramp.elements)
		out.logu(2, 'ramp: %d stages' % (num))
		out.begin('t_ramp')
		out.text( ramp.interpolation )
		out.pack('B',num)
		for el in ramp.elements:
			out.pack('f', el.position)
			out.array('f', el.color)
		out.end()

	if it.type == 'ENVIRONMENT_MAP':
		# environment map chunk
		hasNormals = False
		env = mtex.texture.environment_map
		if env.source == 'IMAGE_FILE':
			out.begin('t_cube')
			out.end()
		else:
			clip = (env.clip_start, env.clip_end)
			out.logu(2, 'environ: %s [%.2f-%2.f]' % (env.mapping,clip[0],clip[1]))
			view = ''
			if not env.viewpoint_object:
				out.log(2,'w','view point is not set')
			else: view = env.viewpoint_object.name
			out.begin('t_env')
			out.pack('2BH3f', env.source=='ANIMATED',
				env.depth, env.resolution, env.zoom,
				clip[0], clip[1] )
			out.text( env.mapping, view )
			out.end()
			return
	elif it.type == 'BLEND':	# blend chunk
		out.begin('t_blend')
		out.text( it.progression )
		out.pack('B', it.use_flip_axis=='VERTICAL' )
		out.end()
		return
	elif it.type == 'NOISE':	# noise chunk
		out.begin('t_noise')
		out.end()
		return
	elif it.type == 'NONE':
		out.begin('t_zero')
		out.end()
		return
	elif it.type != 'IMAGE':
		out.log(2,'w', 'unknown type')
		return
	# image path
	img = it.image
	assert img
	out.begin('t_path')
	fullname = img.filepath
	out.logu(2, '%s: %s' % (img.source,fullname))
	if Settings.cutPaths:
		name = '/'+fullname.rpartition('\\')[2].rpartition('/')[2]
	if fullname.find(name) not in (0,1):
		out.log(2,'w', 'path cut to: %s' % (name))
	out.text( name)
	if hasNormals:
		hasNormals = it.use_normal_map
	out.pack('B', hasNormals)
	if hasNormals:
		out.log(2,'i', 'normal space: %s' % (mtex.normal_map_space))
	out.end()
	# texture image sampling
	if it.type == 'IMAGE':
		out.begin('t_samp')
		out.text( it.extension )
		out.pack( '2B', it.use_mipmap, it.use_interpolation )
		out.end()
	# image sequence chunk
	if img.source == 'SEQUENCE':
		user = mtex.texture.image_user
		out.begin('t_seq')
		out.pack( '3H', user.frames, user.offset, user.start_frame )
		out.end()
	elif img.source != 'FILE':
		out.log(2,'w','unknown image source')



###  MATERIAL:CORE   ###

def save_mat(mat):
	out = Writer.inst
	print("[%s] %s" % (mat.name,mat.type))
	out.begin('mat')
	out.text( mat.name )
	out.pack('2B', mat.use_shadeless, mat.use_tangent_shading)
	out.end()
	if mat.use_nodes:
		out.logu(1,"+graph")
		save_graph( mat.node_tree )
	# diffuse subroutine
	def save_diffuse(model):
		out.begin('m_diff')
		save_color( mat.diffuse_color )
		out.pack('3f', mat.alpha, mat.diffuse_intensity, mat.emit)
		out.text(model)
		out.end()
	def save_specular(model):
		out.begin('m_spec')
		save_color( mat.specular_color )
		out.pack('3f', mat.specular_alpha, mat.specular_intensity, mat.specular_hardness)
		out.text(model)
		out.end()
	if mat.strand:	# hair strand
		st = mat.strand
		out.begin('m_hair')
		dist = -1.0
		if st.use_surface_diffuse:
			dist = st.blend_distance
		out.pack('4fBf', st.root_size, st.tip_size, st.shape,
			st.width_fade, st.use_tangent_shading, dist )
		out.text( st.uv_layer )
		out.end()
	# particle halo material
	if	mat.type == 'HALO':
		out.begin('m_halo')
		halo = mat.halo
		if halo.use_ring or halo.use_lines or halo.use_star:
			out.log(1,'w', 'halo rings, lines & star modes are not supported')
		data = (halo.size, halo.hardness, halo.add)
		out.array('f', data)
		out.pack('B', halo.use_texture)
		out.log(1,'i', 'size: %.2f, hardness: %.0f, add: %.2f' % data)
		out.end()
		save_diffuse('')
	# regular surface material
	elif	mat.type == 'SURFACE':
		out.begin('m_surf')
		parallax = 0.5
		out.pack('B3f', mat.use_shadeless, parallax,
			mat.ambient, mat.translucency )
		out.end()
		sh = (mat.diffuse_shader, mat.specular_shader)
		out.log(1,'i', 'shading: %s %s' % sh)
		save_diffuse(sh[0])
		save_specular(sh[1])
		mirr = mat.raytrace_mirror
		if mirr.use:
			out.log(1,'i', 'mirror: ' + mirr.reflect_factor)
			out.begin('m_mirr')
			save_color( mat.mirror_color )
			out.pack('2f', 1.0, mirr.reflect_factor)
			out.end()
	else:	out.log(1,'w','unsupported type')
	# texture units
	for mt in mat.texture_slots:
		if not mt: continue
		out.logu(1,'+map: ' + mt.name)
		if mt.texture.type != 'IMAGE' and not Settings.allowEmptyTex:
			out.log(2,'w','skipped')
			continue
		save_mat_unit(mt)
		save_mat_image(mt)
