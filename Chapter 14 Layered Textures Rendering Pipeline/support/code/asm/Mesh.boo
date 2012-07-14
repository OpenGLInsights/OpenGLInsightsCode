namespace support.asm

import System.Collections.Generic
import OpenTK.Graphics.OpenGL


public struct Range:
	public	start	as uint
	public	total	as uint
	public	static	final	Zero = Range( start:0, total:0 )
	public def constructor(tm as kri.TagMat):
		start = tm.off
		total = tm.num


private class IndexAccum:
	public	final	ind	= kri.vb.Object()
	public	final	mat	= kri.vb.Object()
	public	final	tex = kri.buf.Texture()

	public	MaxElements	as uint:
		get: return mat.Allocated
	public	curNumber	as uint	= 0
	
	public def init(np as uint) as void:
		ind.init( np<<2 )
		mat.init( np<<2 )
		tex.init( SizedInternalFormat.R32ui, mat )

	public def bindOut(tf as kri.TransFeedback, np as uint) as void:
		n = curNumber<<2
		tf.Bind( (ind,mat), (n,n), (np<<2,np<<2) )


public class Mesh:
	public	final	data	= kri.vb.Attrib()
	public	final	eMap	= Dictionary[of kri.Mesh,Range]()
	public	final	buData	= kri.shade.Bundle()
	public	final	buInd	= kri.shade.Bundle()
	private	final	tf		= kri.TransFeedback(1)
	private	final	vDic	= kri.vb.Dict()
	private	final	vao		= kri.vb.Array()
	private	final	pIndex	= kri.shade.par.Value[of int]('index')
	private	final	pOffset	= kri.shade.par.Value[of int]('offset')
	private	final	mot		= kri.Mesh()
	public			nVert	as uint	= 0
	public	final	maxVert	as uint	= 0
	
	public	Count	as int:
		get: return pIndex.Value
	
	public def constructor(nv as uint):
		clear()
		# init data buffer
		maxVert = nv
		kri.Help.enrich(data,4,'vertex','quat','tex')
		data.initUnit(maxVert)
		# prepare shader dictionary
		d = kri.shade.par.Dict()
		d.var(pIndex,pOffset)
		# make data compose shader
		sa = buData.shader
		sa.add('/asm/copy/data_v')
		sa.feedback(false,'to_vertex','to_quat','to_tex')
		# make index copy shader
		sa = buInd.shader
		sa.add('/asm/copy/ind_v')
		sa.attrib( 8, 'index')	# better chaching in VAO
		sa.feedback(true,'to_index','to_mat')
		# common routines
		for bu in (buData,buInd):
			bu.dicts.Add(d)
			bu.link()
	
	public def clear() as void:
		eMap.Clear()
		nVert = 0
	
	public def copyData(m as kri.Mesh) as Range:
		r = Range.Zero
		if eMap.TryGetValue(m,r):
			return r
		# copy vertex data
		if nVert+m.nVert > maxVert:
			kri.lib.Journal.Log("Asm: vertex buffer overflow (${m.nVert})")
			return r
		tf.Cache[0] = data
		data.bindAsDestination(0, nVert, m.nVert)
		m.fillEntries(vDic)
		vDic.fake('tex0','tex1')
		if m.render( vao, buData, vDic, 1,tf ):
			r.start = nVert
			r.total = m.nVert
			nVert += m.nVert
		eMap.Add(m,r)
		return r

	public def copyIndex(m as kri.Mesh, out as IndexAccum, tm as kri.TagMat, matIndex as uint) as bool:
		if not (m.ind and out):	return false
		rv = Range.Zero
		if not eMap.TryGetValue(m,rv):
			kri.lib.Journal.Log('Asm: mesh not registered')
			return false
		pOffset.Value = rv.start
		pIndex.Value = matIndex
		ps = m.polySize
		isize = m.indexSize
		assert ps==3 and isize == 2
		mot.nVert = num = tm.num * ps
		if out.curNumber + num > out.MaxElements:
			kri.lib.Journal.Log('Asm: index buffer overflow')
			return false
		ai = kri.vb.Info( name:'index', size:1, integer:true,
			type:VertexAttribPointerType.UnsignedShort )
		vDic.add( m.ind, ai, tm.off*ps*isize, isize )
		out.bindOut(tf,num)
		if mot.render( vao, buInd, vDic, 1,tf ):
			out.curNumber += num
			return true
		return false
