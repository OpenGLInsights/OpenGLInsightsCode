namespace viewer

private struct Launcher:
	public	final	ga	as GladeApp
	public	final	unknown as (string)
	# startup options
	public	final	initScene	as string
	public	final	initPipe	as string
	public	final	doUpdate	as bool
	public	final	doStereo	as bool
	public	final	doPlay		as bool
	
	# init
	public def onInit(o as object, args as System.EventArgs):
		if initPipe:	ga.setPipe(initPipe)
		if doUpdate:	ga.setDraw()
		if doStereo:	ga.setStereo()
		ga.sceneToLoad = initScene
		ga.playOnLoad = doPlay
	
	# create
	public def constructor(argv as (string)):
		initScene = initPipe = null
		doUpdate = doStereo = doPlay = false
		# parse command line
		oScene	= '-scene='
		oPipe	= '-pipe='
		for ar in argv:
			if ar.StartsWith(oPipe):
				initPipe	= ar.Substring( oPipe.Length )
			elif ar.StartsWith(oScene):
				initScene	= ar.Substring( oScene.Length )
			elif ar == '-draw':		doUpdate = true
			elif ar == '-stereo':	doStereo = true
			elif ar == '-play':		doPlay = true
		# create app
		ga = GladeApp()
		ga.gw.Initialized += onInit



[System.STAThread]
public def Main(argv as (string)) as void:
	Gtk.Application.Init()
	Launcher(argv)
	Gtk.Application.Run()
