namespace support.defer.fill

import kri.buf

#---------	RENDER ABSTRACT	--------#

public class Base( kri.rend.tech.Meta ):
	public def constructor(suf as string):
		meta = kri.load.Meta.LightSet + ('emissive',)
		super('g.make.'+suf, false, ('c_diffuse','c_specular','c_normal'), *meta)
		shade('/g/make')
		shade('/g/norm/'+suf)
		shade(('/light/common_f','/lib/defer_f'))


public class Wrap( kri.rend.Basic ) :
	private final	buf		as Holder
	private final	render	as kri.rend.Basic
	# init
	public def constructor(con as support.defer.Context, ren as kri.rend.Basic):
		buf = con.buf
		render = ren
	# resize
	public override def setup(pl as kri.buf.Plane) as bool:
		buf.resize( pl.wid, pl.het )
		return render.setup(pl)
	# work	
	public override def process(link as kri.rend.link.Basic) as void:
		if not render:	return
		buf.at.depth = link.Depth
		buf.bind()
		link.SetDepth(0f, false)
		link.ClearColor()
		render.process(link)


#---------	RENDER QUATS AND NORMALS	--------#

public class Quat(Wrap):
	public def constructor(con as support.defer.Context):
		super( con, Base('quat') )

public class Norm(Wrap):
	public def constructor(con as support.defer.Context):
		super( con, Base('norm') )


#---------	RENDER FORK	--------#

public class Fork(Wrap):
	public def constructor(con as support.defer.Context):
		fork = kri.rend.tech.Fork()
		super(con,fork)
		fork.dict['quat']			= Base('quat')
		fork.dict['!quat,normal']	= Base('norm')
