#!/usr/bin/python

import sys
import os
import datetime

import re
match_def = re.compile("//#for.+")
match_dec = re.compile("//#[A-Za-z]+=[A-Za-z'\" ,\[\]\(\)]+")
match_if = re.compile("//#if.+")
match_end = re.compile("//#end")
match_ins = re.compile("\$<[^>]*>")

date = datetime.date.today().strftime("%d/%m/%y")

class Parser:
	globalVars = {}
	def __init__(this, file, fstr = ""):
		exec("from itertools import *", Parser.globalVars)
		if len(fstr) > 0:
			if fstr[:2] == "if":
				#print "OMFG I GOT AN IF"
				this.optype = "if"
				this.fstr = fstr[2:]
			elif fstr[:3] == "for":
				this.optype = "for"
				fstr = fstr[3:]
				this.fstr = map(str.strip, fstr.split(" in ", 1))
				this.fstr[0] = map(str.strip, this.fstr[0].split(","))
			else:
				this.fstr = ""
		else:
			this.fstr = ""
		this.blocks = []
		for line in file:
			if match_end.search(line):
				return
			dec = match_dec.search(line)
			if dec:
				print "var ", dec.group()[3:]
				exec(dec.group()[3:], Parser.globalVars)
			ins = match_def.search(line) or match_if.search(line)
			if ins:
				this.blocks += [Parser(file, ins.group()[3:])]
			else:
				this.blocks += [line]
	def getStr(this, vars = None):
			if not vars:
				vars = {"date":date}
			got = ""
			def run(got, vars):
				for l in this.blocks:
					if isinstance(l, str):
						line = l
						p = 0
						for ins in match_ins.finditer(line):
							got += line[p: ins.start()]
							p = ins.end()
							#print vars
							#print "GETTING", ins.group()[2:-1]
							res = eval(ins.group()[2:-1], Parser.globalVars, vars)
							got += str(res)
						got += line[p:]
					else:
						got += l.getStr(vars)
				return got
			if this.fstr == "":
				got = run(got, vars)
			else:
				if this.optype == "for":
					varlist = eval(str(this.fstr[1]), Parser.globalVars, vars)
					for j in range(len(varlist)):
						for i in range(len(this.fstr[0])):
							v = this.fstr[0][i]
							if len(this.fstr[0]) > 1:
								n = varlist[j][i]
							else:
								n = varlist[j]
							vars[v] = n
						got = run(got, vars)
				elif this.optype == "if":
					if eval(this.fstr, {}, vars):
						got = run(got, vars)
			return got
				

def generate(filename):
	name, ext = os.path.splitext(filename)
	print "Generating " + name
	try:
		if len(name) == 0 or ext != ".gen": return
		inf = open(filename, "r")
		outf = open(name, "w")
		p = Parser(inf)
		outf.write(p.getStr())
		inf.close()
		outf.close()
	except Exception, e:
		print e
		raise e

if len(sys.argv) < 2:
	for path, dirs, files in os.walk("."):
		print path
		for file in files:
			if os.path.splitext(file)[1] == ".gen":
				print "Parsing " + file
				generate(file)
		break
else:
	generate(sys.argv[1])
