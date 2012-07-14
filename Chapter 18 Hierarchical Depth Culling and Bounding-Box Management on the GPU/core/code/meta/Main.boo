namespace kri.meta

import kri.shade


#---	stand-alone meta interface	---#

public interface IBase( kri.INamed ):
	def link(d as par.Dict) as void

public interface IBaseMat( IBase, System.ICloneable ):
	pass

public interface ISlave( System.ICloneable ):
	def link(name as string, d as par.Dict) as void

public interface IUnited:
	Unit as int:
		get

public interface IShaded:
	Shader as Object:
		get


#---	Named meta-data with shader		---#
public class Hermit(IBaseMat,IShaded):
	[Property(Name)]
	private name	as string	= ''
	[Property(Shader)]
	private shader	as Object	= null

	public def copyTo(h as Hermit) as Hermit:
		h.name = name
		h.shader = shader
		return h
	def System.ICloneable.Clone() as object:
		return copyTo(Hermit())
	def IBase.link(d as par.Dict) as void:
		pass


#---	Advanced meta-data with unit link	---#
public class Advanced(IUnited,Hermit):
	[Property(Unit)]
	private unit	as int	= -1
	def System.ICloneable.Clone() as object:
		return copyTo( Advanced( Unit:unit ) )

