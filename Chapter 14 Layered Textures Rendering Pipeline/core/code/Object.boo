namespace kri

import System.Collections.Generic
import OpenTK

public interface IColored:
	Color as Graphics.Color4:
		get
		set

public interface INoded:
	Node as Node:
		get

# Perspective projector for light & camera
public class Projector( ani.data.Player, INoded ):
	public node		as Node
	public rangeIn	= 1f
	public rangeOut	= 100f
	public fov		= 0.4f	# ~23 degrees (half)
	public aspect	= 1f
	public offset	= Vector3.Zero
	
	INoded.Node as Node:
		get: return node

	public def project(ref v as Vector3) as Vector3:
		dz = -1f / v.Z
		tn = dz / System.Math.Tan(fov)
		assert fov > 0f
		return offset + Vector3( tn*v.X, tn*v.Y * aspect,
				(2f*dz*rangeIn*rangeOut - rangeIn-rangeOut) / (rangeIn - rangeOut))
	public def unproject(ref vin as Vector3) as Vector3:
		v = vin-offset
		z = 2f*rangeIn*rangeOut / (v.Z*(rangeOut-rangeIn) - rangeOut - rangeIn)
		return Vector3(-z*v.X, -z*v.Y / aspect, z)
	public def makeOrtho(radius as single) as void:
		fov = -2f / radius
	public def setRanges(a as single, b as single) as void:
		assert b>a
		rangeIn,rangeOut = a,b
	
	public def getFrustum() as (Vector4):
		planes = array[of Vector4](6)
		cos = System.Math.Cos(fov)
		sin = System.Math.Sin(fov)
		planes[0] = Vector4(cos,0f,sin,0f)
		planes[1] = Vector4(-cos,0f,sin,0f)
		planes[2] = Vector4(0f,cos*aspect,sin,0f)
		planes[2].Normalize()
		planes[3] = Vector4(0f,-cos*aspect,sin,0f)
		planes[3].Normalize()
		planes[4] = Vector4(0f,0f,1f,rangeIn)
		planes[5] = Vector4(0f,0f,-1f,-rangeOut)
		return planes
		

public class Camera(Projector):
	[property(Current)]
	public static current	as Camera = null


public class Light(Projector,IColored):
	public enum Type:
		Omni
		Directed
		Spot
	# attributes
	public softness	= 0f
	[Property(Color)]
	private color	as Graphics.Color4	= Graphics.Color4(1f,1f,1f,1f)
	public factor0	= 1f	# constant factor
	public factor1	= 0f	# linear factor
	public factor2	= 0f	# quadratic factor
	public sphere	= 0f	# spherical bound
	public depth	as buf.Texture	= null
	# parallel projection
	public def setLimit(radius as single) as void:
		rangeIn = 1
		rangeOut = radius
		sphere = 1f / radius
	public def getType() as Type:
		if fov<0f:	return Type.Directed
		if fov>0f:	return Type.Spot
		return Type.Omni



public abstract class Shape:
	public virtual def collide(sh as Shape) as Vector3:
		return -sh.collide(self)

public class ShapeSphere(Shape):
	public center	as Vector3	= Vector3(0f,0f,0f)
	public radius	as single	= 0f
	public virtual def collide(sh as ShapeSphere) as Vector3:
		rez = sh.center - center
		kf = (sh.radius + radius) / rez.LengthFast
		return rez * System.Math.Max(0f,1f-kf)

# Physics atom
public class Body:
	public final node	as Node
	public final shape	as Shape
	public mass		= 0f
	public vLinear	= Vector3(0f,0f,0f)
	public vAngular	= Vector3(0f,0f,0f)
	public def constructor(n as Node, sh as Shape):
		node,shape = n,sh
	


# Scene that holds entities, lights & cameras
public class Scene:
	private		static	current	as Scene = null
	public		final	name	as string
	public		pGravity		as kri.shade.par.Value[of Vector4]	= null
	public		backColor		= Graphics.Color4.Black
	# properties
	public static Current as Scene:
		get: return current
		set:
			if current and value:
				lib.Journal.Log("Scene: invalid change (${current}->${value})")
			current = value
	# content
	public		final	entities	= List[of Entity]()
	public		final	bodies		= List[of Body]()
	public		final	lights		= List[of Light]()
	public		final	cameras		= List[of Camera]()
	public		final	particles	= List[of part.Emitter]()
	# funcs
	public def constructor(str as string):
		name = str
