namespace kri

public interface IView:
	def resize(wid as int, het as int) as bool
	def update() as void


# Renders a scene with camera to some buffer
public class View(IView):
	# rendering
	public ren		as rend.Basic	# root render
	# view
	public cam		as Camera	= null
	public scene	as Scene	= null
	
	private virtual def getLink() as kri.rend.link.Basic:
		return null
	
	protected def resize(pl as kri.buf.Plane) as bool:
		return ren!=null and ren.setup(pl)
	
	def IView.resize(wid as int, het as int) as bool:
		return resize(kri.buf.Plane( wid:wid, het:het ))
	
	def IView.update() as void:
		link = getLink()
		Scene.Current = scene
		if cam and link:
			cam.aspect = link.Frame.getInfo().Aspect
		Ant.Inst.params.activate(cam)
		if ren and ren.active:
			ren.process(link)
			vb.Array.Bind = null
		elif link:
			link.activate(false)
			link.ClearColor()
		Camera.Current	= null
		Scene.Current	= null
	
	public def updateSize() as bool:
		link = getLink()
		return ren!=null and link!=null and ren.setup( link.Frame.getInfo() )
	
	public def countVisible() as int:
		if not scene: return 0
		return List[of Entity](e
			for e in scene.entities	if e.Visible[cam]
			).Count


# View for rendering to screen
public class ViewScreen(View):
	public	final	area	= OpenTK.Box2(0f,0f,1f,1f)
	public	final	link	= kri.rend.link.Screen()
	
	private override def getLink() as kri.rend.link.Basic:
		return link
	
	def IView.resize(wid as int, het as int) as bool:
		sc = link.screen
		pl = sc.plane
		pl.wid	= cast(int, wid*area.Width	)
		pl.het	= cast(int, het*area.Height	)
		sc.ofx	= cast(int, wid*area.Left	)
		sc.ofy	= cast(int, het*area.Top	)
		return resize(pl)
	
	def shift(ofx as int, ofy as int) as void:
		sc = link.screen
		sc.ofx += ofx
		sc.ofy += ofy
