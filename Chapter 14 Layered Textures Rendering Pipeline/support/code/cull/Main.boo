namespace support.cull

public class Context:
	public	final	spatial	= kri.vb.Attrib()
	public	final	bound	= kri.vb.Attrib()
	public	final	maxn	as uint
	public	final	pTex	= kri.shade.par.Texture('input')
	public	final	pMinLev	= kri.shade.par.Value[of int]('min_level')
	public	final	dict	= kri.shade.par.Dict()
	public	final	frame	as kri.gen.Frame	= null
	
	public def constructor(n as uint):
		maxn = n
		kri.Help.enrich(bound,	4,'low','hai')
		bound.initUnit(n)
		kri.Help.enrich(spatial,4,'pos','rot')
		dict.unit(pTex)
		dict.var(pMinLev)
		pMinLev.Value = 0
		m = kri.Mesh( nVert:maxn )
		m.buffers.AddRange(( bound, spatial ))
		frame = kri.gen.Frame('box',m)
		m.nVert = 0
	
	public def reset() as void:
		frame.mesh.nVert = 0
	
	public def genId() as uint:
		m = frame.mesh
		if m.nVert>=maxn:
			kri.lib.Journal.Log('Box: objects limit reached')
			m.nVert = 0
		return m.nVert++
	
	public def fillScene(scene as kri.Scene) as void:
		for e in scene.entities:
			if not e.seTag[of box.Tag]():
				e.tags.Add( box.Tag() )




public class Group( kri.rend.Group ):
	public	final	con			as Context					= null
	# renders
	public	final	rBoxFill	as box.Fill					= null
	public	final	rBoxDraw	as box.Draw					= null
	public	final	rBoxUp		as box.Update				= null
	public	final	rSoft		as Soft						= null
	public	final	rFill		as hier.Fill				= null
	public	final	rApply		as hier.Apply				= null
	public	final	rMap		as kri.rend.debug.MapDepth	= null
	# signatures
	public	final	sBoxFill	= 'box.fill'
	public	final	sBoxDraw	= 'box.draw'
	public	final	sBoxUp		= 'box.up'
	public	final	sSoft		= 'z.soft'
	public	final	sFill		= 'z.fill'
	public	final	sApply		= 'z.app'
	public	final	sMap		= 'z.map'

	public def constructor(maxn as uint):
		con = Context(maxn)
		rBoxFill = box.Fill(con)
		rBoxDraw = box.Draw(con)
		rBoxUp = box.Update(con)
		rSoft = Soft()
		rFill = hier.Fill(con)
		rApply = hier.Apply(con)
		rMap = kri.rend.debug.MapDepth()
		rSoft.active = rMap.active = false
		super(rBoxFill,rSoft,rFill,rApply,rBoxUp,rMap)
	
	public def actNormal(debug as int) as void:
		for r in (rBoxFill,rBoxUp,rFill,rApply):
			r.active = true
		if debug>=0:
			rMap.active = true
			rMap.level = debug
	
	public def actSoft() as void:
		for r in (rBoxFill,rBoxUp,rSoft):
			r.active = true
	
	public def fill(rm as kri.rend.Manager, skin as string, sZ as string, sEmi as string) as void:
		rm.put(sBoxFill,	2,rBoxFill,	skin)
		rm.put(sBoxDraw,	1,rBoxDraw,	sBoxFill,sEmi)
		rm.put(sBoxUp,		1,rBoxUp,	sBoxFill)
		rm.put(sSoft,		1,rSoft,	sBoxUp)
		rm.put(sFill,		1,rFill,	sZ)
		rm.put(sApply,		1,rApply,	sFill,sBoxFill)
		rm.put(sMap,		1,rMap,		sFill,sEmi)
