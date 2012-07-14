namespace Boo.Lang.Extensions

import Boo.Lang.Compiler
import Boo.Lang.PatternMatching

#---	Transparent property portal		---#
#usage: portal 'name' as 'type' = 'expression'

macro portal(exp as Ast.Expression):
	bex = exp as Ast.BinaryExpression
	assert bex and bex.Operator == Ast.BinaryOperatorType.Assign
	# the type inference doesn't work perfectly with macroses
	# so I need to pass the type explicitely here
	lef = bex.Left	as Ast.TryCastExpression
	rit = bex.Right as Ast.ReferenceExpression
	assert lef and 'The type is missing'
	name = lef.Target	as Ast.ReferenceExpression
	type = lef.Type		as Ast.TypeReference
	yield [|
		public $(name) as $(type):
			get: return $(rit)
			set: $(rit) = value
	|]


#---	Method wrapper		---#
#usage: wrapper 'class' = ('met1','met2')

macro wrapper(exp as Ast.Expression):
	bex = exp as Ast.BinaryExpression
	assert bex	# operator doesn't matter
	clName = bex.Left as Ast.ReferenceExpression
	arMet = bex.Right as Ast.ArrayLiteralExpression
	assert clName and arMet
	for met in arMet.Items:
		metName = met as Ast.ReferenceExpression
		yield [|
			private static def $(metName)(x as $(clName)):
				return x.$(metName)()
		|]


#---	OpenGL boolean state wrapper		---#

macro state(exp as Ast.Expression):
	name = exp.ToString()
	cap = [| EnableCap.$(name) |]
	yield [|
		public static $(name) as bool:
			get: return GL.IsEnabled($(cap))
			set:
				if(value):	GL.Enable($(cap))
				else:		GL.Disable($(cap))
	|]