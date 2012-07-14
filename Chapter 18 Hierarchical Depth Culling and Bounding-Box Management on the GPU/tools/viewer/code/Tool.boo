namespace viewer


private class AtBox:
	public final info	as kri.vb.Info
	public def constructor(ai as kri.vb.Info):
		info = ai


private class TreeWalker:
	public	final	store	as Gtk.TreeStore
	[Getter(Iter)]
	private	cur	= Gtk.TreeIter.Zero
	private final	stack	= System.Collections.Generic.Stack[of Gtk.TreeIter]()
	public Value as object:
		get:	return store.GetValue(cur,0)
	public Parent	as object:
		get:
			it = Gtk.TreeIter.Zero
			if store.IterParent(it,cur):
				return store.GetValue(it,0)
			return null
	
	public def constructor(target as Gtk.TreeStore):
		store = target
	public def next() as bool:
		if cur == Gtk.TreeIter.Zero:
			return store.GetIterFirst(cur)
		it = Gtk.TreeIter.Zero
		if not store.IterChildren(it,cur):
			while not store.IterNext(cur):
				if not store.IterParent(cur,cur):
					return false
		else:	cur = it
		return true
