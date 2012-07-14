namespace ext

import System
import System.Xml.Serialization
import Boo.Lang.Compiler


public def printXml[of T](ob as T, str as string) as void:
	s = XmlSerializer( ob.GetType() )
	out = IO.StreamWriter(str + '.xml')
	s.Serialize(out,ob)
	out.Close()
	
	
#---	Remove original type declaration	---#

[AttributeUsage(AttributeTargets.Method | AttributeTargets.Class)]
public class RemoveSource(AbstractAstAttribute):
	public override def Apply(node as Ast.Node) as void:
		tm = node as Ast.TypeMember
		if not tm:		raise 'not a type'
		klass = tm.DeclaringType
		klass.Members.Remove(tm)
