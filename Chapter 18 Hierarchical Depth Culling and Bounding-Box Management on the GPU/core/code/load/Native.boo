namespace kri.load

import System
import System.Collections.Generic
import OpenTK


[StructLayout(LayoutKind.Sequential)]
public struct ColorRaw:
	public red		as byte
	public green	as byte
	public blue		as byte


#------		LOAD ATOM		------#

public class Atom:
	public final scene		as kri.Scene
	public final nodes		= Dictionary[of string,kri.Node]()
	public final mats		= Dictionary[of string,kri.Material]()
	
	public def constructor(name as string):
		scene = kri.Scene(name)
		mats[''] = kri.Ant.Inst.loaders.materials.con.mDef
		nodes[''] = null


#------		NATIVE LOADER		------#

public class Native( kri.data.ILoaderGen[of Atom] ):
	public	final readers	= Dictionary[of string,callable(Reader) as bool]()
	public	final resMan	= kri.data.Manager()
	private	final blocks	= Dictionary[of string,string]()
	private	goodSign		= true
	
	public	final swImage	= kri.data.Switch[of kri.buf.Texture]()
	public	final swSound	= kri.data.Switch[of kri.sound.Buffer]()
	
	public def constructor():
		swImage.ext['.tga'] = image.Targa()
		swImage.ext['.dds'] = image.Compress()
		swSound.ext['.wav'] = sound.Wave()
		resMan.register( swImage )
		resMan.register( swSound )
		# attach extensions
		readers['kri']	= p_sign
		readers['grav']	= p_grav
		for ext in kri.Ant.Inst.extensions:
			ext.attach(self)
	
	public def read(path as string) as Atom:	#imp: kri.res.ILoaderGen
		if not kri.data.Manager.Check(path):
			return null
		goodSign = false
		rd = Reader(path,resMan)
		bs = rd.bin.BaseStream
		while bs.Position != bs.Length:
			name = rd.getString(8)
			size = rd.bin.ReadUInt32()
			if name in blocks:
				rd.bin.ReadBytes(size)
				continue
			size += bs.Position
			if size > bs.Length:
				kri.lib.Journal.Log("Loader: file size exceeded (${path})")
				break
			p as callable(Reader) as bool = null
			status as string = null
			if readers.TryGetValue(name,p):
				if p(rd):	# call reader
					if bs.Position != size:
						status = 'incomplete'
				else:	status = 'screwed'
			else:		status = 'unknown'
			if status:
				kri.lib.Journal.Log("Loader: block '${name}' is ${status}")
				blocks[name] = status
				bs.Seek( size, IO.SeekOrigin.Begin )
			if not goodSign:
				kri.lib.Journal.Log("Loader: invalid signature (${path})")
				break
		return rd.finish()
	
	public def p_sign(r as Reader) as bool:
		ver = r.getByte()
		goodSign = (ver == 3)
		return r.Clear
	
	public def p_grav(r as Reader) as bool:
		r.at.scene.pGravity = pg = kri.shade.par.Value[of Vector4]('gravity')
		pg.Value = Vector4( r.getVector() )
		return true


#------		STANDARD EXTENSIONS PACK	------#

public class Standard( kri.IExtension ):
	public final objects	= ExObject()
	public final meshes		= ExMesh()
	public final materials	= ExMaterial()
	public final animations	= ExAnim()
	
	def kri.IExtension.attach(nt as Native) as void:
		for ex in (of kri.IExtension: objects,meshes,materials,animations):
			ex.attach(nt)
