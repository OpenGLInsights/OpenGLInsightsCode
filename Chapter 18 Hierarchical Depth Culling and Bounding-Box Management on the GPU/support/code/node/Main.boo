namespace support.node

import System.Collections.Generic


public class Graph( kri.meta.IBaseMat ):
	public class Node:
		public name	= ''
		public final	sin	= List[of Socket]()
		public final	sot	= List[of Socket]()

	public class Link:
		public	ns	as Node	= null
		public	nd	as Node	= null
		public	sin	as Socket	= null
		public	sot	as Socket	= null

	public class Socket:
		public name	= ''
		public dest	as Node	= null

	public class SocketGen[of T(struct)](Socket):
		public val	as T

	
	public final	nodes	as (Node)	= null
	public final	links	=	List[of Link]()
	
	public def constructor(nn as byte):
		nodes = array[of Node](nn)

	def kri.meta.IBase.link(d as kri.shade.par.Dict) as void:
		pass
	kri.INamed.Name as string:
		get: return 'graph'
	
	def System.ICloneable.Clone() as object:
		gr = Graph( nodes.Length )
		for i in range(nodes.Length):
			gr.nodes[i] = nodes[i]
		gr.links.AddRange( links )
		return gr



public class Extra( kri.IExtension ):
	def kri.IExtension.attach(nt as kri.load.Native) as void:
		nt.readers['graph']		= p_graph
		nt.readers['g_node']	= pg_node
		nt.readers['g_sock']	= pg_sock
		nt.readers['g_links']	= pg_links

	public def p_graph(r as kri.load.Reader) as bool:
		tip = r.getString()
		if tip != 'MATERIAL':
			kri.lib.Journal.Log("Graph: not supported (${tip})")
			return false
		mat = r.geData[of kri.Material]()
		if not mat:
			return false
		num = r.getByte()
		gr = Graph(num)
		mat.metaList.Add(gr)
		return true

	public def pg_node(r as kri.load.Reader) as bool:
		return true
	public def pg_sock(r as kri.load.Reader) as bool:
		return true
	public def pg_links(r as kri.load.Reader) as bool:
		return true
	