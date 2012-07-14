namespace kri.shade

import System.IO
import System.Collections.Generic
import OpenTK.Graphics.OpenGL

//----------------------------------//
//		BASIC OBJECT LOADER			//
//----------------------------------//

public class Loader( kri.data.ILoaderGen[of Object] ):
	public def read(path as string) as Object:	#imp: kri.res.ILoaderGen
		text = Code.Read(path)
		type = Object.Type(path)
		return Object(type, path, text)


//------------------------------------------//
//		CODE INTERFACE & IMPLEMENTATION		//
//------------------------------------------//

public interface ICode:
	Text	as string:
		get
	def getMethod(base as string) as string

public class CodeNull(ICode):
	ICode.Text as string:
		get: return null
	def ICode.getMethod(base as string) as string:
		return null


public class Code(ICode):
	public static Folder	= '.'
	public static def Read(name as string) as string:
		if name.StartsWith('/'):
			name = Folder + name
		name += '.glsl'
		if not kri.data.Manager.Check(name):
			return null
		return File.OpenText(name).ReadToEnd()

	[Getter(Text)]
	private final text	as string
	
	public def constructor(path as string):
		text = Read(path)
	public def constructor(cd as ICode):
		text = cd.Text
	
	def ICode.getMethod(base as string) as string:
		if string.IsNullOrEmpty(text):
			return null
		pos = text.IndexOf(base)
		if pos<0:
			return null
		p2 = text.IndexOf('()',pos)
		assert p2>=0
		return text.Substring(pos,p2+2-pos)
	

//------------------------------//
//		COLLECTOR CLASS			//
//------------------------------//
#todo: use factory for linking

public struct DefMethod:
	public type as string
	public val	as string
	public oper	as string
	public Null as bool:
		get: return string.IsNullOrEmpty(val) or string.IsNullOrEmpty(oper)
	public static final Void	= DefMethod( type:'void' )
	public static final Float	= DefMethod( type:'float', val:'1.0', oper:'*' )


public class Collector:
	public final bu		= kri.shade.Bundle()
	public final mets	= Dictionary[of string,DefMethod]()
	public root			as Object	= null
	public extra		= List[of Object]()
	public Ready	as bool:
		get: return bu.shader.Ready
	
	public def gather(method as string, codes as List[of ICode]) as Object:
		dm = mets[method]
		names = List[of string]()
		for cd in codes:
			cur = cd.getMethod(method+'_')
			assert not cur in names
			names.Add(cur)	if cur != null
		# gather to the new code
		decl = join("\n${dm.type} ${n};"	for n in names)
		if dm.Null:
			body = join("\n\t${n};"			for n in names)
		else:
			help = join("\n\tr${dm.oper}= ${n};"	for n in names)
			body = "\n\t${dm.type} r= ${dm.val};${help}\n\treturn r;"
		all = "#version 130\n${decl}\n\n${dm.type} ${method}()\t{${body}\n}"
		return Object( ShaderType.VertexShader, 'met_'+method, all)
	
	public def absorb[of T( ICode, kri.meta.IShaded )](codes as List[of T]) as void:
		# todo: use IEnumerable istead of List
		# currently produces ivalid IL code
		cl = List[of ICode]()
		for cd in codes:
			cl.Add(cd as ICode)
			bu.shader.add( cd.Shader )
		for key in mets.Keys:
			sh = gather( key, cl )
			bu.shader.add(sh)
	
	public def compose( sem as kri.vb.Info*, *dicts as (par.Dict) ) as void:
		assert root
		sa = bu.shader
		sa.add('/lib/quat_v')
		sa.add(root)
		sa.add( *extra.ToArray() )
		if sem:
			names = List[of string]( 'to_'+at.name for at in sem )
			sa.feedback( false, *names.ToArray() )
		bu.dicts.AddRange(dicts)
		bu.link()


//------------------------------//
//		TEMPLATE CLASS			//
//------------------------------//

public class Template(ICode):
	private final dict	= Dictionary[of string,Object]()
	[Getter(Text)]
	private final text		as string	= null
	private final keys		as (string)	= null
	public final tip		= ShaderType.VertexShader
	
	public def constructor(path as string):
		text = Code.Read(path)
		if not text:
			kri.lib.Journal.Log("Template: failed to read text (${path})")
			return
		tip = Object.Type(path)
		dk = Dictionary[of string,object]()
		pos = 0
		while (p2 = text.IndexOf('%',pos)) >=0:
			pos = p2+1
			dk[ text.Substring(pos,1).ToLower() ] = null
		keys = List[of string]( dk.Keys ).ToArray()

	def ICode.getMethod(base as string) as string:
		return null
	
	public def instance(d as Dictionary[of string,IDictionary[of string,string]]) as Object:
		key = join( join("${v.Key}-${v.Value}" for v in d[k],',') for k in keys, ':')
		sh as Object = null
		if dict.TryGetValue(key,sh):
			return sh
		rez = ''
		def append(line as string):
			pos = line.IndexOf('%')
			if pos>=0:
				k = line.Substring(pos+1,1).ToLower()
				assert d.ContainsKey(k)
				for sub in d[k]:
					append( line.Replace('%'+k,sub.Key).Replace('%'+k.ToUpper(),sub.Value) )
			else: rez += line + "\n"
		for line in text.Split( "\n".ToCharArray()[0] ):
			append(line)
		dict[key] = sh = Object(tip,key,rez)
		return sh
