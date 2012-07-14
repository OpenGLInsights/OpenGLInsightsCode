# <pep8 compliant>

bl_info = {
    'name': 'KRI Scene format (.scene)',
    'author': 'Dzmitry Malyshau',
    'version': (0, 1, 0),
    'blender': (2, 5, 8),
    'api': 36079,
    'location': 'File > Export > Kri Scene (.scene)',
    'description': 'Export KRI Scenes with meshes, armatures, particles and stuff.',
    'warning': '',
    'wiki_url': 'http://code.google.com/p/kri/wiki/Exporter',
    'tracker_url': '',
    'category': 'Import-Export'}

# To support reload properly, try to access a package var, if it's there, reload everything
if 'bpy' in locals():
	import imp
	if 'export_kri' in locals():
	        imp.reload(export_kri)


import bpy
from bpy.props			import *
from bpy_extras.io_utils	import ImportHelper, ExportHelper
from io_scene_kri.scene		import save_scene
from io_scene_kri.common	import Settings


class ExportKRI(bpy.types.Operator, ExportHelper):
	'''Export to KRI scene format'''
	bl_idname	= 'export_scene.kri_scene'
	bl_label	= '-= KRI =- (.scene)'
	filename_ext	= '.scene'

	filepath	= StringProperty( name='File Path',
		description='Filepath used for exporting the KRI scene',
		maxlen=1024, default='')
	break_err	= BoolProperty( name='Break on error',
		description='Stop the process on first error',
		default=Settings.breakError )
	put_normal	= BoolProperty( name='Put normals',
		description='Export vertex normals',
		default=Settings.putNormal )
	put_quat	= BoolProperty( name='Put quaternions',
		description='Export vertex quaternions',
		default=Settings.putQuat )
	put_uv		= BoolProperty( name='Put UV layers',
		description='Export vertex UVs',
		default=Settings.putUv )
	put_color	= BoolProperty( name='Put color layers',
		description='Export vertex colors',
		default=Settings.putColor )
	quat_int	= BoolProperty( name='Process quaternions',
		description='Prepare mesh quaternions for interpolation',
		default=Settings.doQuatInt )
	empty_tex	= BoolProperty( name='Empty textures',
		description='Allow texture units with non-existing textures',
		default=Settings.allowEmptyTex )
	cut_paths	= BoolProperty( name='Cut paths',
		description='Force relative paths',
		default=Settings.cutPaths )

	def execute(self, context):
		Settings.breakError	= self.properties.break_err
		Settings.putNormal	= self.properties.put_normal
		Settings.putQuat	= self.properties.put_quat
		Settings.putUv		= self.properties.put_uv
		Settings.putColor	= self.properties.put_color
		Settings.doQuatInt	= self.properties.quat_int
		Settings.allowEmptyTex	= self.properties.empty_tex
		save_scene( self.properties.filepath, context )
		return {'FINISHED'}


# Add to a menu
def menu_func(self, context):
	self.layout.operator( ExportKRI.bl_idname, text=ExportKRI.bl_label )

def register():
	bpy.utils.register_module(__name__)
	bpy.types.INFO_MT_file_export.append(menu_func)

def unregister():
	bpy.utils.unregister_module(__name__)
	bpy.types.INFO_MT_file_export.remove(menu_func)

if __name__ == '__main__':
	register()
