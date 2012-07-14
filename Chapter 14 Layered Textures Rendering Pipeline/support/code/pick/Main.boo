namespace support.pick

import OpenTK.Graphics.OpenGL
import OpenTK.Input

public class Tag( kri.ITag ):
	public pick	as callable(kri.Entity, OpenTK.Vector3) as void	= null
	

public class Render( kri.rend.Basic ):
	private final fbo	= kri.buf.Holder( mask:1 )
	private final bu	= kri.shade.Bundle()
	private final qlog	as uint
	private final pInd	= kri.shade.par.Value[of single]('index')
	private final mouse	as MouseDevice
	private final va	= kri.vb.Array()
	private coord	=	(of uint: 0,0)
	#debug data
	private final bv	= kri.shade.Bundle()
	private final pTex	= kri.shade.par.Value[of kri.buf.Texture]('input')

	public def constructor(win as kri.Window, reduct as uint, numorder as uint):
		active = false
		qlog = reduct
		mouse = win.Mouse
		mouse.ButtonDown += ev
		# make buffer
		assert numorder<=16
		fbo.at.depth = kri.buf.Texture.Depth(0)
		fbo.at.color[0] = kri.buf.Texture(
			intFormat:PixelInternalFormat.Rgba16 )
		# make shader
		d = kri.shade.par.Dict()
		d.var(pInd)
		d.unit(pTex)
		bu.shader.add('/zcull_v', '/pick_f', '/lib/tool_v', '/lib/quat_v')
		bu.dicts.Add(d)
		bv.shader.add('/copy_v', '/copy_f')
		bv.dicts.Add(d)
	
	def destructor():
		mouse.ButtonDown -= ev

	public override def setup(pl as kri.buf.Plane) as bool:
		fbo.resize( pl.wid>>qlog, pl.het>>qlog)
		return true

	public override def process(con as kri.rend.link.Basic) as void:
		fbo.bind()
		con.SetDepth(0f, true)
		con.ClearDepth( 1f )
		con.ClearColor()
		#GL.ClearBuffer(ClearBuffer.Color, 0, (of uint:10,10,10,10))
		pInd.Value = 0f
		ents = List[of kri.Entity](e for e in kri.Scene.Current.entities if e.seTag[of Tag]()).ToArray()
		for i in range(ents.Length):
			pInd.Value = (i+1f) / ((1<<16)-1)
			kri.Ant.Inst.params.activate(ents[i])
			ents[i].render(va,bu)
		if not 'Debug':
			con.activate( con.Target.Same, 0f, false )
			pTex.Value = fbo.at.color[0] as kri.buf.Texture
			kri.Ant.Inst.quad.draw(bv)
			return
		# react, todo: use PBO and actually read on demand
		kri.vb.Object.Pack = null
		rect = System.Drawing.Rectangle(coord[0],coord[1], 1,1)
		index = fbo.read[of ushort]( PixelFormat.Red, rect )
		active = false
		if not index[0]: return
		val = fbo.read[of single]( PixelFormat.DepthComponent, rect )
		pl = fbo.at.color[0]
		vin = OpenTK.Vector3(coord[0]*1f / pl.wid, coord[1]*1f / pl.het, val[0])
		vin = OpenTK.Vector3.Multiply(vin,2f) - OpenTK.Vector3.One
		c = kri.Camera.Current
		point = c.unproject(vin)
		pw = kri.Node.SafeWorld( c.node ).byPoint(point)
		# call the react method
		e = ents[ index[0]-1 ]
		sp = kri.Node.SafeWorld( e.node )
		sp.inverse()
		pe = sp.byPoint(pw)
		fun = e.seTag[of Tag]().pick
		if fun:	fun(e,pe)

	public def ev(ob as object, arg as OpenTK.Input.MouseButtonEventArgs) as void:
		active = true
		coord[0] = (mouse.X >> qlog)
		coord[1] = fbo.at.color[0].het - (mouse.Y >> qlog)
