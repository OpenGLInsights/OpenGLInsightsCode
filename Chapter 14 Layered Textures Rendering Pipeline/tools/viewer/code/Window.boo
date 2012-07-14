namespace viewer

public class GladeApp:
	[Glade.Widget]	window			as Gtk.Window
	[Glade.Widget]	drawBox			as Gtk.Container
	[Glade.Widget]	statusBar		as Gtk.Statusbar
	[Glade.Widget]	toolBar			as Gtk.Toolbar
	[Glade.Widget]	butClear		as Gtk.ToolButton
	[Glade.Widget]	butOpen			as Gtk.ToolButton
	[Glade.Widget]	butDraw			as Gtk.ToggleToolButton
	[Glade.Widget]	butPlay			as Gtk.ToolButton
	[Glade.Widget]	butProfile		as Gtk.ToolButton
	[Glade.Widget]	butStereo		as Gtk.ToggleToolButton
	[Glade.Widget]	propertyBook	as Gtk.Notebook
	[Glade.Widget]	objView			as Gtk.TreeView
	[Glade.Widget]	camFovLabel		as Gtk.Label
	[Glade.Widget]	camAspectLabel	as Gtk.Label
	[Glade.Widget]	camActiveBut	as Gtk.ToggleButton
	[Glade.Widget]	metaUnitLabel	as Gtk.Label
	[Glade.Widget]	metaShaderLabel	as Gtk.Label
	[Glade.Widget]	meshModeLabel	as Gtk.Label
	[Glade.Widget]	meshVertLabel	as Gtk.Label
	[Glade.Widget]	meshPolyLabel	as Gtk.Label
	[Glade.Widget]	attrTypeLabel	as Gtk.Label
	[Glade.Widget]	attrSizeLabel	as Gtk.Label
	[Glade.Widget]	entCleanBut		as Gtk.Button
	[Glade.Widget]	entVisibleBut	as Gtk.ToggleButton
	[Glade.Widget]	entFrameVisBut	as Gtk.ToggleButton
	[Glade.Widget]	recNumLabel		as Gtk.Label
	[Glade.Widget]	recPlayBut		as Gtk.Button
	[Glade.Widget]	emiStartBut		as Gtk.Button
	[Glade.Widget]	renderCombo		as Gtk.ComboBox
	
	private	final	journal	= kri.lib.Journal()
	private	final	exception	= ExceptApp()
	private final	stat	= kri.lib.StatPeriod('Viewer',1.0)
	private	final	dOpen	as Gtk.FileChooserDialog
	private final	dialog	as Gtk.MessageDialog
	public	final	gw		as Gtk.GLWidget
	public	final	logic	= Logic()
	public	sceneToLoad		as string	= null
	public	playOnLoad		as bool		= false
	private	curObj			as object	= null
	private	curIter	= Gtk.TreeIter.Zero
	private	flushCount	= 0
	
	private def showMessage(mType as Gtk.MessageType, text as string) as void:
		dialog.MessageType = mType
		dialog.Text = text
		dialog.Run()
		dialog.Hide()
	
	private def flushJournal() as bool:
		all = journal.flush()
		if not all:			return false
		gw.Visible = false
		showMessage( Gtk.MessageType.Info, all )
		gw.Visible = true
		return true
	
	public def load(path as string) as void:
		str = logic.load(path)
		# notify
		logic.updateList()
		flushJournal()
		statusBar.Push(0, 'Loaded '+str)
	
	public def setDraw() as void:
		butDraw.Active = true
	
	public def setStereo() as void:
		butStereo.Active = true
		
	public def setPipe(name as string) as void:
		cur = logic.selectPipe( renderCombo.Model, name )
		renderCombo.Active = cur
	
	#--------------------	
	# signals
	
	public def onException(args as GLib.UnhandledExceptionArgs) as void:
		gw.Visible = false
		args.ExitApplication = true
		GLib.Idle.Remove(onIdle)
		exception.show( args.ExceptionObject.ToString() )
	
	public def onInit(o as object, args as System.EventArgs) as void:
		logic.init()
		kri.Ant.Inst.stats = stat
		gw.QueueResize()
	
	public def onDelete(o as object, args as Gtk.DeleteEventArgs) as void:
		logic.quit()
		Gtk.Application.Quit()
	
	public def onIdle() as bool:
		if not string.IsNullOrEmpty(sceneToLoad):
			load(sceneToLoad)
			sceneToLoad = null
			if playOnLoad:
				logic.playAll()
				playOnLoad = false
		if butDraw.Active:
			gw.QueueDraw()
		elif window.Title != stat.title:
			window.Title = stat.title
		return true
	
	public def onFrame(o as object, args as System.EventArgs) as void:
		core = kri.Ant.Inst
		if not core:	return
		core.update(1)
		logic.frame( butStereo.Active )
		stat.frame()
		str = stat.gather()
		if butDraw.Active and str:
			window.Title = str + logic.getSceneStats()
		if ++flushCount<3:
			flushJournal()
	
	public def onSize(o as object, args as Gtk.SizeAllocatedArgs) as void:
		r = args.Allocation
		logic.size( r.Width, r.Height )
		flushJournal()
		statusBar.Push(0, 'Resized into '+r.Width+'x'+r.Height )
	
	public def onButClear(o as object, args as System.EventArgs) as void:
		flushCount = 0
		logic.clear()
		flushJournal()
		exception.sceneFile = null
		gw.QueueDraw()
		statusBar.Push(0, 'Cleared')
	
	public def onButOpen(o as object, args as System.EventArgs) as void:
		if not kri.Ant.Inst:
			return
		rez = dOpen.Run()
		dOpen.Hide()
		if rez != 0:
			statusBar.Push(0, 'Load cancelled')
			return
		flushCount = 0
		exception.sceneFile = dOpen.Filename
		load( dOpen.Filename )
		gw.QueueDraw()
	
	public def onButPlay(o as object, args as System.EventArgs) as void:
		logic.playAll()
		statusBar.Push(0, 'Started all scene animations')
	
	public def onSelectObj(o as object, args as System.EventArgs) as void:
		curIter = Gtk.TreeIter()
		if not objView.Selection.GetSelected(curIter):
			return
		curObj = obj = logic.TreeModel.GetValue(curIter,0)
		page = 0
		if (ent = obj as kri.Entity):
			entVisibleBut.Active = ent.visible
			entFrameVisBut.Active = ent.VisibleCam
			page = 1
		if obj isa kri.Node:
			page = 2
		if obj isa kri.Material:
			page = 3
		if (cam = obj as kri.Camera):
			camFovLabel.Text = 'Fov: ' + cam.fov
			camAspectLabel.Text = 'Aspect: ' + cam.aspect
			camActiveBut.Active = (logic.ViewCam == cam)
			page = 4
		if obj isa kri.Light:
			page = 5
		if (rec = obj as kri.ani.data.Record):
			recNumLabel.Text = 'Channels: ' + rec.channels.Count
			page = 6
		if (meta = obj as kri.meta.Advanced):
			metaUnitLabel.Text = 'Unit: ' + meta.Unit
			metaShaderLabel.Text = ''
			if meta.Shader:
				metaShaderLabel.Text = meta.Shader.Description
			page = 7
		if (mesh = obj as kri.Mesh):
			meshModeLabel.Text = mesh.drawMode.ToString()
			meshVertLabel.Text = 'nVert: ' + mesh.nVert
			meshPolyLabel.Text = 'nPoly: ' + mesh.nPoly
			page = 8
		if (box = obj as AtBox):
			attrTypeLabel.Text = box.info.type.ToString()
			attrSizeLabel.Text = 'Size: ' + box.info.size + ('','i')[box.info.integer]
			page = 9
		if obj isa kri.part.Emitter:
			page = 10
		propertyBook.Page = page
	
	public def onActivateObj(o as object, args as Gtk.RowActivatedArgs) as void:
		logic.activate( args.Path )
		objView.ExpandRow( args.Path, true )

	#--------------------
	# visuals
	
	private def objFunc(col as Gtk.TreeViewColumn, cell as Gtk.CellRenderer, model as Gtk.TreeModel, iter as Gtk.TreeIter):
		obj = model.GetValue(iter,0)
		text = obj.GetType().ToString()
		icon = 'file'
		iNoded = obj as kri.INoded
		if iNoded and iNoded.Node:
			text = iNoded.Node.name
		# select icon and tet
		if obj isa kri.Camera:
			icon = 'fullscreen'
		if obj isa kri.Light:
			icon = 'dialog-info'
		if obj isa kri.Entity:
			icon = 'orientation-portrait'
		if (emi = obj as kri.part.Emitter):
			text = emi.name
			icon = 'about'
		if obj isa kri.part.Manager:
			text = 'manager'
			icon = 'directory'
		if obj isa kri.part.Behavior:
			icon = 'add'
		if obj isa kri.Node:
			text = 'node'
			icon = 'sort-descending'
		if obj isa kri.Skeleton:
			text = 'sleleton'
			icon = 'disconnect'
		if (rec = obj as kri.ani.data.Record):
			text = "${rec.name} (${rec.length})"
			icon = 'cdrom'
		if (chan = obj as kri.ani.data.IChannel):
			icon = 'execute'
			text = "[${chan.ElemId}] ${chan.Tag}"
		if (mat = obj as kri.Material):
			text = mat.name
			icon = 'select-color'
		if (mad = obj as kri.meta.Advanced):
			text = mad.Name
			icon = 'color-picker'
		if (mun = obj as kri.meta.AdUnit):
			text = '(empty)'
			icon = 'harddisk'
			if mun.Value:
				text = mun.Value.target.ToString()
		if obj isa kri.vb.Storage:
			text = ('store','mesh')[obj isa kri.Mesh]
			icon = 'dnd-multiple'
		if (box = obj as AtBox):
			text = box.info.name
			icon = 'preferences'
		# set the result
		if (ct = cell as Gtk.CellRendererText):
			ct.Text = text
		if (cp = cell as Gtk.CellRendererPixbuf):
			cp.StockId = 'gtk-'+icon


	#--------------------
	# construction
	
	private def makeColumn() as Gtk.TreeViewColumn:
		col = Gtk.TreeViewColumn()
		col.Title = 'Objects:'
		rPix = Gtk.CellRendererPixbuf()
		col.PackStart(rPix,false)
		col.SetCellDataFunc( rPix, objFunc )
		rTex = Gtk.CellRendererText()
		col.PackEnd(rTex,true)
		col.SetCellDataFunc( rTex, objFunc )
		return col
	
	public def constructor():
		GLib.Idle.Add( onIdle )
		GLib.ExceptionManager.UnhandledException += onException
		# load scheme
		xml = Glade.XML('scheme/main.glade', 'window', null)
		xml.Autoconnect(self)
		window.DeleteEvent	+= onDelete
		dialog = Gtk.MessageDialog( window, Gtk.DialogFlags.Modal,
			Gtk.MessageType.Info, Gtk.ButtonsType.Ok, null )
		dialog.WidthRequest = 400
		dialog.HeightRequest = 300
		# make toolbar
		butClear.Clicked	+= onButClear
		butOpen.Clicked 	+= onButOpen
		butPlay.Clicked		+= onButPlay
		butProfile.Clicked	+= do(o as object, args as System.EventArgs):
			showMessage( Gtk.MessageType.Info, logic.report() )
		dOpen = Gtk.FileChooserDialog('Select KRI scene to load:',
			window, Gtk.FileChooserAction.Open )
		dOpen.AddButton('Load',0)
		filt = Gtk.FileFilter( Name:'KRI Scenes' )
		filt.AddPattern('*.scene')
		dOpen.AddFilter(filt)
		# make panel
		propertyBook.ShowTabs = false
		objView.AppendColumn( makeColumn() )
		objView.Model = logic.TreeModel
		objView.CursorChanged	+= onSelectObj
		objView.RowActivated	+= onActivateObj
		camActiveBut.Clicked	+= do(o as object, args as System.EventArgs):
			if not camActiveBut.Active:	return
			logic.ViewCam = curObj as kri.Camera
		entCleanBut.Clicked		+= do(o as object, args as System.EventArgs):
			(curObj as kri.Entity).deleteOwnData()
		entVisibleBut.Clicked	+= do(o as object, args as System.EventArgs):
			(curObj as kri.Entity).visible = entVisibleBut.Active
		recPlayBut.Clicked		+= do(o as object, args as System.EventArgs):
			rec = logic.playRecord(curIter)
			statusBar.Push(0, "Animation '${rec.name}' started")
		emiStartBut.Clicked		+= do(o as object, args as System.EventArgs):
			emi = curObj as kri.part.Emitter
			logic.start(emi)
			statusBar.Push(0, "Particle '${emi.name}' started")
		renderCombo.Changed		+= do(o as object, args as System.EventArgs):
			str = renderCombo.ActiveText
			logic.changePipe(str)
			statusBar.Push(0, 'Pipeline switched to '+str)
			gw.QueueDraw()
		# add gl widget
		drawBox.Child = gw = logic.makeWidget()
		gw.Initialized		+= onInit
		gw.RenderFrame		+= onFrame
		gw.SizeAllocated	+= onSize
		gw.Visible = true
		# run
		statusBar.Push(0, 'Launched')
