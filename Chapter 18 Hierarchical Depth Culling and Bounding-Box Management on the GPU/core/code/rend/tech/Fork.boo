namespace kri.rend.tech

import System.Collections.Generic

public class Fork( kri.rend.Basic ):
	public	final	dict	= Dictionary[of string,Sorted]()
	public def constructor():
		pass
	public override def setup(pl as kri.buf.Plane) as bool:
		for r in dict.Values:
			if not r.setup(pl):
				return false
		return true
	public override def process(con as kri.rend.link.Basic) as void:
		scene = kri.Scene.Current
		if not scene:	return
		for r in dict.Values:
			r.butch.Clear()
		for e in scene.entities:
			for d in dict:
				good = true
				for s in d.Key.Split(char(',')):
					if s.StartsWith('!'):
						if e.findAny(s.Substring(1)):
							good = false
							break
					elif not e.findAny(s):
						good = false
						break
				if good:
					d.Value.addObject(e,null)
		for r in dict.Values:
			for b in r.butch:
				b.draw()
