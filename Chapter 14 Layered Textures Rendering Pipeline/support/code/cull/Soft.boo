namespace support.cull

import OpenTK

public class Soft( kri.rend.Basic ):
	# CPU-side frustum culling:
	#www.gamedev.net/topic/512123-fast--and-correct-frustum---aabb-intersection/
	
	public override def process(link as kri.rend.link.Basic) as void:
		scene = kri.Scene.Current
		cam = kri.Camera.Current
		if not (scene and cam):	return
		sc = kri.Node.SafeWorld(cam.node)
		sx as kri.Spatial
		planes = cam.getFrustum()
		for ent in scene.entities:
			se = kri.Node.SafeWorld(ent.node)
			se.inverse()
			sx.combine(sc,se)
			bmin = ent.localBox.center - ent.localBox.hsize
			bmax = ent.localBox.center + ent.localBox.hsize
			visible = true
			for i in range(6):
				pl = planes[i].Xyz
				normal = Vector3.Transform(pl, sx.rot) * sx.scale
				normal.Normalize()
				point = -planes[i].W * pl
				newp = sx.byPoint(point)
				vmin = bmin
				if normal.X<0f:	vmin.X = bmax.X
				if normal.Y<0f:	vmin.Y = bmax.Y
				if normal.Z<0f:	vmin.Z = bmax.Z
				dot = Vector3.Dot(normal,vmin-newp)
				if dot>0f:
					visible = false
					break
			ent.frameVisible[cam] = visible
