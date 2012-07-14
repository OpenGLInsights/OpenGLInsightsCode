namespace kri.shade

import System.Collections.Generic
import OpenTK


public class Parameter:
	public abstract def upload() as void:
		pass

# Uniform param representor
[ext.spec.Class(( bool, int, single, Vector4, Quaternion, Graphics.Color4 ))]
[ext.RemoveSource]
public class ParUni[of T(struct)](Parameter):
	public final loc	as int
	public final piv	as par.IBase[of T]
	public def constructor(lc as int, iv as par.IBase[of T]):
		assert iv
		loc = lc
		piv = iv
	public def constructor(lc as int, iv as par.IBaseRoot):
		self(lc, iv as par.IBase[of T])
	public override def upload() as void:
		data = piv.Value
		Program.Param(loc,data)


public class ParTexture(Parameter):
	public final loc	as int
	public final piv	as par.IBase[of kri.buf.Texture]
	public final tun	as int
	public def constructor(lc as int, iv as par.IBase[of kri.buf.Texture], tn as int):
		assert iv and tn>=0
		loc,tun = lc,tn
		piv = iv
	public def constructor(lc as int, iv as par.IBaseRoot, tn as int):
		self(lc, iv as par.IBase[of kri.buf.Texture], tn)
	public override def upload() as void:
		slot = tun
		kri.buf.Texture.Slot(slot)
		Program.Param(loc,slot)
		tex = piv.Value
		if not tex:
			tex = kri.buf.Texture.Zero
		if not tex.CanSample:
			kri.lib.Journal.Log("Texture: not ready to sample (${tex.handle})")
		tex.bind()



#---------

public class Bundle:
	public	final shader	as Mega
	public	final dicts		= List[of par.Dict]((kri.Ant.Inst.dict,))
	private	final params	= List[of Parameter]()
	private	failed			as bool	= false
	
	public	LinkFail		as bool:
		get:
			if not (failed or shader.Ready):
				link()
			return failed
	
	public def constructor():
		shader = Mega()
	public def constructor(sh as Mega):
		shader = sh
		failed = sh == null
	public def constructor(bu as Bundle):
		shader = bu.shader
	
	public def fillParams() as void:
		if LinkFail:	return
		params.Clear()
		tun = 0
		badNames = List[of string]()
		for uni in shader.uniforms:
			loc = shader.getLocation( uni.name )
			for i in range(uni.size):
				str = uni.name + ('',"[${i}]")[uni.size>1]
				iv	as par.IBaseRoot = null
				for d in dicts:
					iv = null
					if d.TryGetValue(str,iv): break
				if not iv:
					badNames.Add(str)
					continue
				p = uni.genParam(loc+i,iv,tun)
				if p: params.Add(p)
		if badNames.Count:
			str = string.Join( ',', badNames.ToArray() )
			kri.lib.Journal.Log('Shader params not bound: '+str)

	public def link() as void:
		if shader.link():
			fillParams()
		else:	failed = true

	public def activate() as bool:
		if LinkFail:	return false
		shader.bind()
		for p in params:
			p.upload()
		if kri.Ant.Inst.debug:
			return shader.validate()
		return true
	
	public def clear() as void:
		failed = false
		shader.clear()
		dicts.Clear()
		params.Clear()

	public def pushAttribs(ind as kri.vb.Object, va as kri.vb.Array, dict as kri.vb.Dict) as bool:
		return (not LinkFail) and va.pushAll( ind, shader.attribs, dict )
