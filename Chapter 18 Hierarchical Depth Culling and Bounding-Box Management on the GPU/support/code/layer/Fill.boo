namespace support.layer

import System.Collections.Generic
import OpenTK
import OpenTK.Graphics.OpenGL
import kri.shade


public class Fill( kri.rend.tech.General ):
	state	Blend
	portal	Parallax	as single	= pPax.Value
	public	shadeUnits		= true
	private final	fbo		as kri.buf.Holder
	private final	factory	= kri.shade.Linker(onLink)
	private	doNormal		= false
	private	mesh		as kri.Mesh		= null
	private	vDict		as kri.vb.Dict	= null
	private final	din		= Dictionary[of string,kri.meta.Hermit]()
	private final	sDefer	= Object.Load('/lib/defer_f')
	private	final	sVert	= Object.Load('/g/layer/pass_v')
	private final	sFrag	= Object.Load('/g/layer/pass_f')
	private final	sNorm	= Object.Load('/g/layer/normal_f')
	private final	sParax	= Object.Load('/g/layer/parallax_f')
	private final	fout	= ('c_diffuse','c_specular','c_normal')
	# params
	private final	pDic	= par.Dict()
	private final	pPax	= par.Value[of single]('parallax')
	private	final	pHas	= par.Value[of int]('has_data')
	private final	pTex	= par.Texture('texture')
	private final	pColor	= par.Value[of Vector4]('user_color')

	# init
	public def constructor(con as support.defer.Context):
		super('g.layer.fill')
		fbo = con.buf
		pDic.var(pColor)
		pDic.var(pHas)
		pDic.var(pPax)
		pBump = par.Texture('bump')
		pBump.Value = con.Bump
		pDic.unit(pTex,pBump)
	
	private def onLink(sa as Mega) as void:
		if doNormal:
			sa.fragout(fout[2])
		else:
			sa.fragout(fout[0],fout[1])
	
	private def getSpaceShader(str as string) as Object:
		rez = Object.Load("/g/layer/bump/${str.ToLower()}_v")
		if not rez:
			kri.lib.Journal.Log("Deferred layer: unknow normal space (${str})")
		return rez
	
	private def setBlend(str as string) as bool:
		GL.BlendEquation( BlendEquationMode.FuncAdd )
		if str == '':
			GL.BlendFunc( BlendingFactorSrc.One, 		BlendingFactorDest.Zero )
		elif str == 'MULTIPLY':
			GL.BlendFunc( BlendingFactorSrc.DstColor,	BlendingFactorDest.Zero )
		elif str == 'ADD':
			GL.BlendFunc( BlendingFactorSrc.One, 		BlendingFactorDest.One )
		elif str == 'MIX':
			GL.BlendFuncSeparate(
				BlendingFactorSrc.SrcAlpha,	BlendingFactorDest.OneMinusSrcAlpha,
				BlendingFactorSrc.Zero,		BlendingFactorDest.One)
		else:	return false
		return true
	
	private def setParams(pa as Pass, affects as string*) as void:
		# set blending
		if not setBlend( pa.blend ):
			kri.lib.Journal.Log("Blend: unknown mode (${pa.blend})")
			pa.blend = ''
		# set mask
		afDiff = afEmis = false
		afSpec = afHard = false
		for aff in affects:
			if aff == 'color_diffuse':
				afDiff = true
			if aff == 'color_emission':
				afEmis = true
			if aff == 'color_specular':
				afSpec = true
			if aff == 'hardness':
				afHard = true
		GL.ColorMask(0, afDiff,afDiff,afDiff, afEmis)
		GL.ColorMask(1, afSpec,afSpec,afSpec, afHard)
		# set manual color
		c = pa.color
		flag = (0f,1f)[pa.doIntensity]
		pColor.Value = Vector4( c.R, c.G, c.B, flag )

	# construct: fill
	public override def construct(mat as kri.Material) as Bundle:
		bu = Bundle()
		bu.dicts.AddRange(( mat.dict, pDic ))
		sa = bu.shader
		sa.add( *kri.Ant.Inst.libShaders )
		sa.add( '/g/layer/make_v', '/g/layer/get_uni_v', '/g/layer/make_f' )
		sa.fragout(*fout)
		return bu
	
	# construct: layer
	private def makeLayerProgram(un as kri.meta.AdUnit, space as string) as Bundle:
		uname = 'unit'
		din[uname] = un.input
		mapins = kri.load.Meta.MakeTexCoords(false,din)
		if not mapins:	return null
		(un as kri.meta.ISlave).link(uname,pDic)	# add offset and scale under proper names
		(un.input as kri.meta.IBase).link(pDic)		# add input-specific data
		sall = List[of Object](mapins)
		sall.Add( (sFrag,sNorm)[doNormal] )			# normal space shader
		ss as Object = null
		if doNormal==false or not (ss=getSpaceShader(space)):
			ss = getSpaceShader('zero')
		sall.Add(ss)
		if not doNormal:	sall.Add(sParax)		# parallax
		sall.AddRange(( sVert, un.input.Shader, sDefer ))	# core shaders
		sall.AddRange( kri.Ant.Inst.libShaders )	# standard shaders
		return factory.link( sall, pDic )			# generate program
	
	# draw
	protected override def onPass(va as kri.vb.Array, tm as kri.TagMat, bu as Bundle) as void:
		fbo.setMask(7)
		GL.ColorMask(true,true,true,true)
		if not mesh.render( va, bu, vDict, tm.off, tm.num, 1, null ):
			return
		if not shadeUnits:	return
		for meta in tm.mat.metaList:
			app = meta as Pass
			if not (app and app.enable):	continue
			un = tm.mat.unit[ app.Unit ]
			if not (un and un.input):
				continue
			doNormal = un.isBump
			if not app.prog:
				app.prog = makeLayerProgram( un, app.bumpSpace )
			if app.prog.LinkFail:
				continue
			pTex.Value = un.Value
			if doNormal:
				Blend = false
				fbo.setMask(4)
			else:
				Blend = true
				fbo.setMask(3)
				setParams( app, un.affects.Keys )
			mesh.render( va, app.prog, vDict, tm.off, tm.num, 1, null )
			GL.ColorMask(0, true,true,true,true)
			GL.ColorMask(1, true,true,true,true)
		Blend = false

	# resize
	public override def setup(pl as kri.buf.Plane) as bool:
		fbo.resize( pl.wid, pl.het )
		return super(pl)

	# work	
	public override def process(link as kri.rend.link.Basic) as void:
		fbo.bind()
		# prepare depth layer
		if true or link.Depth.samples:
			if not fbo.at.depth:
				fbo.at.depth = td = kri.buf.Texture.Depth(0)
				td.filt(false,false)
				td.wid = fbo.at.color[0].wid
				td.het = fbo.at.color[0].het
			assert fbo.at.depth.samples == 0
			GL.DepthMask(true)
			fbo.mask = 0
			link.blitTo(fbo, ClearBufferMask.DepthBufferBit)
		else:	fbo.at.depth = link.Depth
		# prepare buffer
		fbo.setMask(7)
		link.SetDepth(-1f, false)
		link.ClearColor( Graphics.Color4(0f,0f,0f,0.5f) )
		scene = kri.Scene.Current
		if not scene:	return
		# render objects
		for e in scene.entities:
			kri.Ant.Inst.params.activate(e)
			vDict = e.CombinedAttribs
			mesh = e.mesh
			pHas.Value = vDict.fake('vertex','normal','quat')
			addObject(e,vDict)
		fbo.setMask(7)
