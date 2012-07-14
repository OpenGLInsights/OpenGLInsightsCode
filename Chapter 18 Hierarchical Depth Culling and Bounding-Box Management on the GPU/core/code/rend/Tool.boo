namespace kri.rend

import OpenTK.Graphics

#---------	COLOR CLEAR	--------#

public class Clear( Basic ):
	public backColor	= Color4.Black
	public override def process(link as link.Basic) as void:
		link.activate(false)
		link.ClearColor( backColor )


#---------	WRAPPER	--------#

public class Wrap( Basic ):
	public final	sub	as Basic	= null
	public def constructor(rend as Basic):
		sub = rend
	public override def process(con as link.Basic) as void:
		if sub: sub.process(con)


#---------	SCREEN COPY		--------#

public class Copy( Basic ):
	public	final	tun	= kri.shade.par.Texture('input')
	public	final	bu	= kri.shade.Bundle()
	public def constructor():
		d = kri.shade.par.Dict()
		d.unit(tun)
		bu.dicts.Add(d)
		bu.shader.add('/copy_v','/copy_f')
	public override def process(con as link.Basic) as void:
		if con.activate(false):
			kri.Ant.Inst.quad.draw(bu)
	public def process(ln as link.Buffer, con as link.Basic) as bool:
		if not (ln and con):		return	false
		if not con.activate(false):	return	false
		if kri.Ant.Inst.gamma and con.Frame.fixGamma:
			tun.Value = ln.buf.at.color[0] as kri.buf.Texture
			return kri.Ant.Inst.quad.draw(bu)
		return ln.blitTo(con)


#---------	EARLY Z FILL	--------#

public class EarlyZ( tech.Sorted ):
	public	final	bu	= kri.shade.Bundle()
	public	offset	= 1f
	public def constructor():
		super('zcull')
		bu.shader.add( '/zcull_v', '/lib/tool_v', '/lib/quat_v' )
		bu.link()
	public override def construct(mat as kri.Material) as kri.shade.Bundle:
		return bu
	public override def process(con as link.Basic) as void:
		con.activate( con.Target.None, offset, true )
		con.ClearDepth(1f)
		drawScene()


#---------	INITIAL FILL EMISSION	--------#

public class Emission( tech.Meta ):
	public final pBase	= kri.shade.par.Value[of Color4]('base_color')
	public fillDepth	= false
	public backColor	= Color4.Black
	
	public def constructor():
		super('mat.emission', false, null, 'emissive','diffuse')
		shade('/mat_base')
		dict.var(pBase)
		pBase.Value = Color4.Black
	public override def process(link as link.Basic) as void:
		if fillDepth:
			link.activate( link.Target.Same, 1f, true )
			link.ClearDepth(1f)
		else: link.activate( link.Target.Same, 0f, false )
		link.ClearColor( backColor )
		drawScene()


#---------	ADD COLOR	--------#

public class Color( tech.Sorted ):
	private final bu	= kri.shade.Bundle()
	public fillColor	= false
	public fillDepth	= false
	public def constructor():
		super('color')
		bu.shader.add( '/color_v','/color_f', '/lib/quat_v','/lib/tool_v' )
		bu.link()
	public override def construct(mat as kri.Material) as kri.shade.Bundle:
		return bu
	public override def process(link as link.Basic) as void:
		if fillDepth:
			link.ClearDepth(1.0)
			link.activate( link.Target.Same, 1f, true )
		else:
			link.activate( link.Target.Same, 0f, false )
		if fillColor:
			link.ClearColor()
			drawScene()
		else:
			using kri.Blender( kri.Blend.Add ):
				drawScene()


#---------	RENDER SSAO	--------#


#---------	RENDER EVERYTHING AT ONCE	--------#

public class All( tech.Sorted ):
	public def constructor():
		super('all')
	public override def construct(mat as kri.Material) as kri.shade.Bundle:
		bu = kri.shade.Bundle()
		bu.link()
		return bu
	public override def process(link as link.Basic) as void:
		link.activate( link.Target.Same, 0f, true )
		link.ClearDepth(1f)
		link.ClearColor()
		drawScene()
