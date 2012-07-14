namespace kri.load

import System.Collections.Generic
import OpenTK
import OpenTK.Graphics
import kri.ani.data


public class ExAnim( kri.IExtension ):
	public final anid		= Dictionary[of string, System.Func[of Reader,IChannel]]()
	public final badCurves	= Dictionary[of string,byte]()
	
	def kri.IExtension.attach(nt as Native) as void:
		init()
		nt.readers['action']	= p_action
		nt.readers['curve']		= p_curve
	
	# generate private method wrappers here, to pass to 'Rac' function
	wrapper Reader = (getReal,getVec2,getVector,getVec4,getScale,getColor,getQuatRev,getQuatEuler)

	# generates invalid binary format if using generics, bypassing with extenions
	[ext.spec.Method(( Vector3,Quaternion,single ))]
	[ext.RemoveSource()]
	private def genBone		[of T(struct)](fun as callable(kri.NodeBone, ref T)) as System.Action[of IPlayer,T,byte]:
		return do(pl as IPlayer, v as T, i as byte):
			bar = (pl as kri.Skeleton).bones
			if not i or i>bar.Length:	return
			fun( bar[i-1], v )
			bar[i-1].touch()
	
	[ext.spec.Method(( Vector3,Quaternion,single ))]
	[ext.RemoveSource()]
	private def genSpatial	[of T(struct)](fun as callable(ref kri.Spatial, ref T)) as System.Action[of IPlayer,T,byte]:
		return do(pl as IPlayer, v as T, i as byte):
			n = pl as kri.Node
			fun( n.local, v )
			n.touch()
	
	private static def RacMatColor(name as string):
		return Rac(getColor)	do(pl as IPlayer, v as Color4, i as byte):
			((pl as kri.Material).Meta[name] as kri.meta.Data[of Color4]).Value = v
	private static def RacMatValue(name as string):
		return Rac(getReal)		do(pl as IPlayer, v as single, i as byte):
			((pl as kri.Material).Meta[name] as kri.meta.Data[of single]).Value = v
	private static def RacTexUnit(fun as callable(kri.meta.AdUnit) as kri.shade.par.ValuePure[of Vector4]):
		return Rac(getVector)	do(pl as IPlayer, v as Vector3, i as byte):
			fun((pl as kri.Material).unit[i-1]).Value = Vector4(v)
	private static def RacProject(fun as callable(kri.Projector,single)):
		return Rac(getReal)		do(pl as IPlayer, v as single, i as byte):
			assert not i
			fun(pl as kri.Projector, v)

	# fill action dictionary
	private def init() as void:
		# skeleton sub-trans
		def fs_pos(b as kri.NodeBone, ref v as Vector3):
			b.local.pos = b.bindPose.byPoint(v)
		def fs_rot(b as kri.NodeBone, ref v as Quaternion):
			b.local.rot = b.bindPose.rot * v
		def fs_sca(b as kri.NodeBone, ref v as single):
			b.local.scale = b.bindPose.scale * v
		# spatial sub-trans
		def ft_pos(ref sp as kri.Spatial, ref v as Vector3):
			sp.pos = v
		def ft_rot(ref sp as kri.Spatial, ref v as Quaternion):
			sp.rot = v
		def ft_sca(ref sp as kri.Spatial, ref v as single):
			sp.scale = v
		# projector sub-trans
		def fp_prin(pr as kri.Projector, v as single):
			pr.rangeIn = v
		def fp_prout(pr as kri.Projector, v as single):
			pr.rangeOut = v
		# skeleton bone
		anid['s.location']				= Rac( getVector,	genBone(fs_pos) )
		anid['s.rotation_quaternion']	= Rac( getQuatRev,	genBone(fs_rot) )
		anid['s.scale']					= Rac( getScale,	genBone(fs_sca) )
		# node
		anid['n.location']				= Rac( getVector,		genSpatial(ft_pos) )
		anid['n.rotation_euler']		= Rac( getQuatEuler,	genSpatial(ft_rot) )
		anid['n.rotation_quaternion']	= Rac( getQuatRev,		genSpatial(ft_rot) )
		anid['n.scale']					= Rac( getScale,		genSpatial(ft_sca) )
		# material
		anid['m.diffuse_color']		= RacMatColor('diffuse')
		anid['m.specular_color']	= RacMatColor('specular')
		anid['m.specular_hardness']	= RacMatValue('glossiness') 
		anid['m.halo.size']			= Rac(getReal) do(pl as IPlayer, v as single, i as byte):
			((pl as kri.Material).Meta['halo'] as kri.meta.Halo).Data.X = v
		# texture unit
		anid['t.offset']		= RacTexUnit({u| return u.pOffset })
		anid['t.scale']			= RacTexUnit({u| return u.pScale })
		# light
		anid['l.energy']	= Rac(getReal)	do(pl as IPlayer, v as single, i as byte):
			(pl as kri.Light).factor0 = 1f / v
		anid['l.color']		= Rac(getColor)	do(pl as IPlayer, v as Color4, i as byte):
			(pl as kri.IColored).Color = v
		anid['l.clip_start']	= RacProject(fp_prin)
		anid['l.clip_end']		= RacProject(fp_prout)
		# camera
		anid['c.angle']		= Rac(getReal)	do(pl as IPlayer, v as single, i as byte):
			(pl as kri.Camera).fov = v*0.5f
		anid['c.clip_start']	= RacProject(fp_prin)
		anid['c.clip_end']		= RacProject(fp_prout)


	#---	Parse action	---#
	public def p_action(r as Reader) as bool:
		player = r.geData[of Player]()
		if not player:	return false
		name = r.getString()
		rec = Record( name, r.getReal() )
		player.anims.Add(rec)
		r.puData(rec)
		return true
		
	#---	Channel pre-defined interpolators per type	---#
	
	public static def InterColor(q as Color4, w as Color4, t as single) as Color4:
		return Color4( q.R*(1f-t)+w.R*t, q.G*(1f-t)+w.G*t, q.B*(1f-t)+w.B*t, q.A*(1f-t)+w.A*t )
	public static def InterSingle(q as single, w as single, t as single) as single:
		return (1-t)*q + t*w
	
	private static def FixChan(c as Channel[of Vector2]):
		c.lerp = Vector2.Lerp
	private static def FixChan(c as Channel[of Vector3]):
		c.lerp = Vector3.Lerp
	private static def FixChan(c as Channel[of Vector4]):
		c.lerp = Vector4.Lerp
	private static def FixChan(c as Channel[of Quaternion]):
		c.lerp = Quaternion.Slerp
		c.bezier = false
	private static def FixChan(c as Channel[of Color4]):
		c.lerp = InterColor
	private static def FixChan(c as Channel[of single]):
		c.lerp = InterSingle
	
	#---	Read Abstract Channel (rac) constructor	---#
	
	# bypassing incorrect code generation of nested templates
	[ext.spec.Method(( single,Vector2,Vector3,Vector4,Quaternion,Color4 ))]
	[ext.RemoveSource()]
	public static def Rac[of T(struct)](fread as System.Func[of Reader,T],
	fup as System.Action[of IPlayer,T,byte]) as System.Func[of Reader,IChannel]:
		return do(r as Reader) as IChannel:
			ind = r.getByte() # element index
			num = cast(int, r.bin.ReadUInt16() )
			chan = Channel[of T](num,ind,fup)
			FixChan(chan)
			chan.extrapolate = r.getByte()>0
			for i in range(num):
				chan.kar[i] = Key[of T]( t:r.getReal(),
					co:fread(r), h1:fread(r), h2:fread(r) )
			return chan
	
	#---	Unknown channel read	---#
	protected static def ReadNullChannel() as IChannel:
		return null
	protected static def ReadDefaultChannel(size as byte) as System.Func[of Reader,IChannel]:
		if size == 1:	return Rac( getReal, null )
		elif size == 2:	return Rac( getVec2, null )
		elif size == 3:	return Rac( getVector, null )
		elif size == 4:	return Rac( getVec4, null )
		else: return ReadNullChannel
	
	#---	Parse curve	---#
	public def p_curve(r as Reader) as bool:
		rec	= r.geData[of Record]()
		return false	if not rec
		data_path = r.getString()
		siz = r.getByte()	# element size in floats
		fun as System.Func[of Reader,IChannel] = null
		if not anid.TryGetValue(data_path,fun):
			badCurves[data_path] = siz
			fun = ReadDefaultChannel(siz)
		chan = fun(r)
		return false	if not chan
		chan.Tag = data_path
		rec.channels.Add(chan)
		return true
