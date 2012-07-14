namespace ext.spec

import System
import Boo.Lang.Compiler


internal def getPredicate(*names as (string)) as Ast.NodePredicate:
	return def(n as Ast.Node):
		st = n as Ast.SimpleTypeReference
		return false	if not st
		for n in names:
			return true	if st.Name == n
		return false


internal class ParameterRazor(Ast.DepthFirstVisitor):
	public log	= ''
	public override def OnGenericTypeReference(gtr as Ast.GenericTypeReference):
		name = gtr.Name			# cut off [] part
		ind = name.IndexOf('[')
		return	if ind < 0
		gtr.Name = name.Substring(0,ind)
		log += "${name}: ${gtr.Name}\n"
