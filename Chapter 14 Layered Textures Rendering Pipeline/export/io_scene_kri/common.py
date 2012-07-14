__author__ = ['Dzmitry Malyshau']
__bpydoc__ = 'Settings & Writing access for KRI exporter.'

class Settings:
	breakError	= False
	putNormal	= True
	putQuat		= True
	putUv		= True
	putColor	= True
	doQuatInt	= True
	allowEmptyTex	= True
	cutPaths	= True
	kFrameSec	= 1.0 / 25.0

	def printLog():
		print("Settings:")
		s = ''
		s += ('','n')[	Settings.putNormal	]
		s += ('','q')[	Settings.putQuat	]
		s += ('','u')[	Settings.putUv		]
		s += ('','c')[	Settings.putColor	]
		print("\tData:",s)
		ny = ('no','yes')
		val = (Settings.doQuatInt and Settings.putQuat)
		print("\tQuat interpolation:",ny[val])
		val = Settings.allowEmptyTex
		print("\tEmpty textures:",ny[val])
		print("\tTick:" , Settings.kFrameSec)


import sys


class Writer:
	tabs = ('',"\t","\t\t","\t\t\t")
	inst = None
	__slots__= 'fx','pos','counter','stop','oldout'

	def __init__(self,path):
		self.fx = open(path,'wb')
		self.pos = 0
		self.counter = {'':0,'i':0,'w':0,'e':0}
		self.stop = False
		self.oldout = sys.stdout
		log_name = path.replace('.scene','.log')
		sys.stdout = open(log_name,'w')
		Settings.printLog()

	def pack(self,tip,*args):
		import struct
		assert self.pos
		self.fx.write( struct.pack('<'+tip,*args) )
	def array(self,tip,ar):
		import array
		assert self.pos
		array.array(tip,ar).tofile(self.fx)
	def text(self,*args):
		for s in args:
			x = len(s)
			assert x<256
			bt = bytes(s,'ascii')
			self.pack('B%ds'%(x),x,bt)

	def begin(self,name):
		import struct
		assert len(name)<8 and not self.pos
		bt = bytes(name,'ascii')
		self.fx.write( struct.pack('<8sL',bt,0) )
		self.pos = self.fx.tell()
	def end(self):
		import struct
		assert self.pos
		off = self.fx.tell() - self.pos
		self.fx.seek(-off-4,1)
		self.fx.write( struct.pack('<L',off) )
		self.fx.seek(+off+0,1)
		self.pos = 0

	def log(self,indent,level,message):
		self.counter[level] += 1
		if level=='e' and Settings.breakError:
			self.stop = True
		print('%s(%c) %s' % (Writer.tabs[indent],level,message))
	def logu(self,indent,message):
		print( "%s%s" % (Writer.tabs[indent],message) )

	def conclude(self):
		self.fx.close()
		sys.stdout.close()
		sys.stdout = self.oldout
		c = self.counter
		print(c['e'],'errors,',c['w'],'warnings,',c['i'],'infos')
		



def save_color(rgb):
	for c in rgb:
		Writer.inst.pack('B', int(255*c) )


def save_matrix(mx):
	import math
	pos,rot,sca = mx.decompose()
	scale = (sca.x + sca.y + sca.z)/3.0
	out = Writer.inst
	if math.fabs(sca.x-sca.y) + math.fabs(sca.x-sca.z) > 0.01:
		out.log(1,'w', 'non-uniform scale: (%.1f,%.1f,%.1f)' % sca.to_tuple(1))
	out.pack('8f',
		pos.x, pos.y, pos.z, scale,
		rot.x, rot.y, rot.z, rot.w )
