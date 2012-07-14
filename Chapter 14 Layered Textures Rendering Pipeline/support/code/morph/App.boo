namespace support.morph

import OpenTK

#----------------------------------------
#	Animation of morphing between to shapes

public class Anim( kri.ani.Loop ):
	public final k0	as Tag
	public final k1	as Tag
	public def constructor(e as kri.Entity, s0 as string, s1 as string):
		k0 = k1 = null
		for key in e.enuTags[of Tag]():
			if key.name == s0:	k0 = key
			if key.name == s1:	k1 = key
		if not (k0 and k1):
			kri.lib.Journal.Log("Morph: animation keys not found (${s0},${s1})")
	protected override def onRate(rate as double) as void:
		if k0:	k0.Value = 1.0 - rate
		if k1:	k1.Value = rate


#----------------------------------------
#	Update render, puts the morph result into the mesh data

public class Update( kri.rend.Basic ):
	public final tf		= kri.TransFeedback(1)
	public final bu		= kri.shade.Bundle()
	public final pVal	= kri.shade.par.Value[of Vector4]('shape_value')
	private final va	= kri.vb.Array()
	private final eps	= 1.0e-7
	public def constructor():
		d = kri.shade.par.Dict()
		d.var(pVal)
		bu.shader.add('/skin/morph_v')
		bu.shader.feedback(true,'to_pos')
		bu.dicts.Add(d)
	
	public override def process(con as kri.rend.link.Basic) as void:
		using tf.discard():
			for ent in kri.Scene.Current.entities:
				keys = ent.enuTags[of Tag]()
				dirty = System.Array.Find(keys) do(t as Tag):
					return t.Dirty
				if keys.Length<2 or not dirty:
					continue
				assert ent.mesh
				pVal.Value = Vector4( keys[0].Value, keys[1].Value, 0f,0f )
				sum = Vector4.Dot( pVal.Value, Vector4.One )
				pVal.Value.X += 1f-sum
				assert System.Math.Abs(sum-1f) < eps
				d = ent.CombinedAttribs
				# bind attribs & draw
				for i in range( System.Math.Min(4,keys.Length) ):
					d['pos'+i] = kri.vb.Entry( keys[i].data, 'vertex' )
				tf.Bind( ent.mesh.find('vertex') )
				ent.mesh.render(va,bu,d,1,tf)
