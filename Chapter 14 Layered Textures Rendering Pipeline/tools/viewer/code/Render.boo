namespace viewer

import OpenTK

public class RenderSet:
	public	final	rMan	= kri.rend.Manager()
	public	final	rClear	= kri.rend.Clear()
	public	final	rZ		= kri.rend.EarlyZ()
	public	final	rColor	= kri.rend.Color( fillColor:true, fillDepth:false )
	public	final	rSkin	= support.skin.Universal()
	public	final	rSkelet	= support.skin.Debug()
	public	final	rAttrib		= kri.rend.debug.Attrib()
	public	final	rSurfBake	= support.bake.surf.Update(0,false)
	public	final	rStereo		= support.stereo.Split(0.1f)
	public	final	rNormal		as support.light.normal.Apply	= null
	public	final	rDummy		as kri.rend.part.Dummy			= null
	public	final	rParticle	as kri.rend.part.Standard		= null
	public	final	grForward	as support.light.group.Forward	= null
	public	final	grDeferred	as support.defer.Group			= null
	public	final	grCull		= support.cull.Group(410)

	public	ClearColor	as Graphics.Color4:
		set:	rClear.backColor = grForward.rEmi.backColor = value
	
	public def constructor(profile as bool, samples as byte, pc as kri.part.Context):
		rMan.doProfile = profile
		# create render groups
		rDummy		= kri.rend.part.Dummy(pc)
		rParticle	= kri.rend.part.Standard(pc)
		grForward	= support.light.group.Forward( 8, false )
		grDeferred	= support.defer.Group( 3, 10, grForward.con, true, null )
		rNormal		= support.light.normal.Apply( grForward.con )
		# populate render manager
		sk = 'skin'
		sz = 'zcull'
		emi = grForward.sEmi
		rMan.put('clear',	1,rClear)
		rMan.put(sk,		2,rSkin)
		rMan.put(sz,		2,rZ,		sk)
		rMan.put('color',	3,rColor,	'clear',sk)
		rMan.put('atr',		3,rAttrib,	sk)
		rMan.put('skinbug',	3,rSkelet,	'atr')
		rMan.put('surf',	3,rSurfBake,sk)
		grForward.fill( rMan, sk, sz)
		grDeferred.fill( rMan, sz )
		grCull.fill( rMan, sk, sz, emi )
		rMan.put('norm',	3,rNormal,	'color',grCull.sApply)
		rMan.put('dummy',	2,rDummy,	emi)
		rMan.put('part',	3,rParticle,emi)
		rMan.put('stereo',	3,rStereo,	'norm',sz)
	
	public def gen(str as string) as kri.rend.Basic:
		grForward.BaseColor = Graphics.Color4.Black
		for ren in rMan.Renders:
			ren.active = false
		if str == 'Debug':
			rAttrib.active = rSkelet.active = true
		if str == 'Simple':
			for ren in (rSkin,rZ,rColor,rNormal,rDummy,rSurfBake):
				ren.active = true
		if str == 'Forward':
			for ren in (rSkin,rZ,rParticle,rSurfBake)+grForward.renders:
				ren.active = true
			grForward.rEmi.fillDepth = false
		if str in ('Deferred','Layered'):
			for ren in (rSkin,rZ,rParticle,rSurfBake):
				ren.active = true
			grDeferred.actNormal(str == 'Layered')
		if str in ('HierZ'):
			for ren in (rSkin,rZ,rClear,rNormal):
				ren.active = true
			grCull.actNormal(-1)
			#grCull.actSoft()
		if str in ('Anaglyph'):
			for ren in (rSkin,rZ,rColor,rNormal,rStereo):
				ren.active = true
		return rMan
