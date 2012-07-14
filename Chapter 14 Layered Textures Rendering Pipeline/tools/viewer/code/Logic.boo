namespace viewer

public class Logic:
	private	final	config	= kri.lib.Config('kri.conf')
	private final	options	= kri.lib.OptionReader(config)
	private	final	view	= kri.ViewScreen()
	private	final	al		= kri.ani.Scheduler()
	private	final	objTree	= Gtk.TreeStore(*(object, ))
	private	vProxy	as kri.IView	= null
	private rset	as RenderSet	= null
	private	ant		as kri.Ant			= null
	private	opera	as kri.sound.Opera	= null
	
	public ViewCam	as kri.Camera:
		get: return view.cam
		set: view.cam = value
	
	public TreeModel as Gtk.TreeModel:
		get: return objTree

	
	public def playRecord(it as Gtk.TreeIter) as kri.ani.data.Record:
		par = Gtk.TreeIter.Zero
		objTree.IterParent(par,it)
		pl = objTree.GetValue(par,0) as kri.ani.data.Player
		rec = objTree.GetValue(it,0) as kri.ani.data.Record
		al.add( kri.ani.data.Anim(pl,rec) )
		return rec
	
	public def playAll() as void:
		al.clear()
		used = List[of object]()
		tw = TreeWalker(objTree)
		while tw.next():
			ob = tw.Value
			if ob as kri.ani.data.Record:
				par = tw.Parent
				if par not in used:
					used.Add(par)
					playRecord( tw.Iter )
			emi = ob as kri.part.Emitter
			if emi:
				emi.filled = false
				al.add(emi)
	

	private def addPlayer(par as Gtk.TreeIter, ob as object) as void:
		pl = ob as kri.ani.data.Player
		if not pl: return
		for ani in pl.anims:
			objTree.AppendValues(par,ani)
	
	private def addObject(par as Gtk.TreeIter, ob as object) as Gtk.TreeIter:
		if not ob:	return par
		if par == Gtk.TreeIter.Zero:
			it = objTree.AppendValues(ob)
		else:
			it = objTree.AppendValues(par,ob)
		on = ob as kri.INoded
		if on:	addObject(it, on.Node)
		me = ob as kri.IMeshed
		if me:	addObject(it, me.Mesh)
		addPlayer(it,ob)
		return it
	
	private def addObject(ob as object) as Gtk.TreeIter:
		return addObject( Gtk.TreeIter.Zero, ob )
		
	public def updateList() as void:
		objTree.Clear()
		if not view.scene:
			return
		for cam in view.scene.cameras:
			addObject(cam)
		for lit in view.scene.lights:
			addObject(lit)
		for ent in view.scene.entities:
			#ent.tags.Add( kri.rend.box.Tag() )
			it = addObject(ent)
			addObject( it, ent.store )
			for tag in ent.tags:
				td = tag as kri.ITagData
				if td:	addObject(it,td.Data)
		for par in view.scene.particles:
			it = addObject(par)
			addObject( it, par.owner )
	

	public def load(path as string) as string:
		rset.grCull.con.reset()
		pos = path.LastIndexOfAny("/\\".ToCharArray())
		fdir = path.Substring(0,pos)
		# load scene
		kri.Ant.Inst.loaders.materials.prefix = fdir
		kri.load.image.Basic.Compressed = true
		loader = kri.load.Native()
		at = loader.read(path)
		view.scene = at.scene
		rset.grCull.con.fillScene(at.scene)
		if at.scene.cameras.Count:	# set camera
			view.cam = at.scene.cameras[0]
		return path.Substring(pos+1)
	

	public def selectPipe(md as Gtk.TreeModel, str as string) as int:
		cur = 0
		it as Gtk.TreeIter
		rez = md.GetIterFirst(it)
		while rez:
			sx = md.GetValue(it,0) as string
			if sx == str:
				return cur
			rez = md.IterNext(it)
			++cur
		kri.lib.Journal.Log("Viewer: pipeline '${str}' not found");
		return -1
	
	public def resetScene() as void:
		if not view.scene:	return
		for e in view.scene.entities:
			e.frameVisible.Clear()
		#for l in view.scene.lights:
		#	l.depth = null
	
	public def changePipe(str as string) as void:
		view.ren = rset.gen(str)
		resetScene()
		view.updateSize()

	public def getSceneStats() as string:
		if not view.scene: return ''
		total = view.scene.entities.Count
		active = view.countVisible()
		return ", ${active}/${total} visible"
	

	public def init() as void:
		auDev = config.ask('AL.Device',null)
		samples = byte.Parse(config.ask('InnerSamples','0'))
		ant = kri.Ant( config, options.debug, options.gamma )
		eLayer	= support.layer.Extra()
		eSkin	= support.skin.Extra()
		eCorp	= support.corp.Extra()
		eMorph	= support.morph.Extra()
		ant.extensions.AddRange((of kri.IExtension:eLayer,eSkin,eMorph)) #eCorp
		ant.anim = al
		if auDev!=null:
			opera = kri.sound.Opera(auDev)
		rset = RenderSet( true, samples, eCorp.con )
		rset.grDeferred.rBug.layer = -1
		vProxy = support.stereo.Proxy(view,0.01f,0f)

	public def quit() as void:
		rset = null
		for dis in (of System.IDisposable: ant,opera):
			if dis: dis.Dispose()
		ant = null
		opera = null

	public def frame(stereo as bool) as void:
		(view,vProxy)[stereo].update()
	
	public def size(w as int, h as int) as bool:
		mv = (view,vProxy)[vProxy!=null]
		return mv.resize(w,h)
	
	public def clear() as void:
		view.scene = null
		view.cam = null
		objTree.Clear()

	
	public def activate(treePath as Gtk.TreePath) as void:
		par = it = Gtk.TreeIter()
		objTree.GetIter(par,treePath)
		rez = objTree.IterChildren(it,par)
		while rez:
			ox = objTree.GetValue(it,0)
			if	(ox isa kri.meta.AdUnit) or (ox isa kri.meta.Advanced) or\
				(ox isa kri.ani.data.IChannel) or (ox isa AtBox) or (ox isa kri.part.Behavior):
				rez = objTree.Remove(it)
			else:
				rez = objTree.IterNext(it)
		ox = objTree.GetValue(par,0)
		if (mat = ox as kri.Material):
			for unit in mat.unit:
				objTree.AppendValues(par,unit)
			for meta in mat.metaList:
				objTree.AppendValues(par,meta)
		if (rec = ox as kri.ani.data.Record):
			for ch in rec.channels:
				objTree.AppendValues(par,ch)
		if (vs = ox as kri.vb.Storage):
			for vat in vs.buffers:
				for ai in vat.Semant:
					objTree.AppendValues(par,AtBox(ai))
		if (own = ox as kri.part.Manager):
			for beh in own.behos:
				objTree.AppendValues(par,beh)


	public def makeWidget() as Gtk.GLWidget:
		gm = options.genMode(32,24)
		fl = options.genFlags()
		widget = Gtk.GLWidget( gm, options.verMajor, options.verMinor, fl )
		# change size
		sizes	= config.ask('GL.Size','0x0').Split(char('x'))
		wid	= uint.Parse( sizes[0] )
		het	= uint.Parse( sizes[1] )
		if wid*het:
			widget.SetSizeRequest(wid,het)
		return widget
	
	public def report() as string:
		return rset.rMan.genReport()
	
	public def start(emi as kri.part.Emitter) as void:
		emi.filled = false
		al.remove(emi)
		al.add(emi)
	