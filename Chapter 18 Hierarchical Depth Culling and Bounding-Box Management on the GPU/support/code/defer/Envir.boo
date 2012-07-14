namespace support.defer

import OpenTK


public class Envir( kri.rend.Basic ):
	public	final	bu		= kri.shade.Bundle()
	public	final	effect	= 'mirror'
	private	final	pTex	= kri.shade.par.Texture('env')
	private	final	pMulti	= kri.shade.par.Value[of Vector4]('env_multi')
	private	final	va		= kri.vb.Array()
	private final	fbo		= kri.buf.Holder()
	private	mesh	as kri.Mesh		= null
	private dict	as kri.vb.Dict	= null
	
	public def constructor(con as support.defer.Context):
		bu.shader.add('/zcull_v','/lib/quat_v','/lib/tool_v')
		bu.shader.add('/g/env_f','/lib/math_f','/lib/defer_f')
		d = kri.shade.par.Dict()
		d.unit(pTex)
		d.var(pMulti)
		bu.dicts.AddRange(( d, con.dict ))
		bu.link()
	
	public override def process(link as kri.rend.link.Basic) as void:
		link.activate( link.Target.Same, 0f, false )
		scene = kri.Scene.Current
		if not scene:	return
		using kri.Blender( kri.Blend.Add ):
			for e in scene.entities:
				for tm in e.enuTags[of kri.TagMat]():
					meta = tm.mat.Meta['mirror'] as kri.meta.Mirror
					if not (meta and meta.Unit>=0):	continue
					u = tm.mat.unit[meta.Unit]
					if not meta.cube:
						fbo.at.color[0] = tin = u.Value
						meta.cube = kri.gen.Texture.toCube(fbo)
						if not meta.cube:
							kri.lib.Journal.Log("Envir: error converting to cube (${tin.Description})")
							meta.cube = kri.buf.Texture.Zero
					if meta.cube == kri.buf.Texture.Zero:
						continue
					pMulti.Value = meta.Value
					pTex.Value = meta.cube
					dict = e.CombinedAttribs
					e.mesh.render( va,bu, dict, tm.off, tm.num, 1,null )
