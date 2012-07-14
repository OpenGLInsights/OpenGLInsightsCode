#ifndef GLF_BOUND_HPP
#define GLF_BOUND_HPP

//-----------------------------------------------------------------------------
// Include
//-----------------------------------------------------------------------------
#include <glm/glm.hpp>
#include <limits>
#include <algorithm>

namespace glf
{
	//-------------------------------------------------------------------------
	struct BBox
	{
		inline BBox():
		pMin( std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max()),
		pMax(-std::numeric_limits<float>::max(),-std::numeric_limits<float>::max(),-std::numeric_limits<float>::max())
		{

		}

		inline void Add(const BBox& _bbox)
		{
			pMin.x 	= std::min(_bbox.pMin.x,pMin.x);
			pMin.y 	= std::min(_bbox.pMin.y,pMin.y);
			pMin.z 	= std::min(_bbox.pMin.z,pMin.z);

			pMax.x 	= std::max(_bbox.pMax.x,pMax.x);
			pMax.y 	= std::max(_bbox.pMax.y,pMax.y);
			pMax.z 	= std::max(_bbox.pMax.z,pMax.z);
		}

		inline void Add(const glm::vec3& _point)
		{
			pMin.x 	= std::min(_point.x,pMin.x);
			pMin.y 	= std::min(_point.y,pMin.y);
			pMin.z 	= std::min(_point.z,pMin.z);

			pMax.x 	= std::max(_point.x,pMax.x);
			pMax.y 	= std::max(_point.y,pMax.y);
			pMax.z 	= std::max(_point.z,pMax.z);
		}

		inline void ToCorners(	glm::vec3& _c0, 
								glm::vec3& _c1,
								glm::vec3& _c2,
								glm::vec3& _c3,
								glm::vec3& _c4,
								glm::vec3& _c5,
								glm::vec3& _c6,
								glm::vec3& _c7 ) const
		{
			_c0 = glm::vec3(pMin.x,pMin.y,pMin.z);
			_c1 = glm::vec3(pMax.x,pMin.y,pMin.z);
			_c2 = glm::vec3(pMax.x,pMax.y,pMin.z);
			_c3 = glm::vec3(pMin.x,pMax.y,pMin.z);

			_c4 = glm::vec3(pMin.x,pMin.y,pMax.z);
			_c5 = glm::vec3(pMax.x,pMin.y,pMax.z);
			_c6 = glm::vec3(pMax.x,pMax.y,pMax.z);
			_c7 = glm::vec3(pMin.x,pMax.y,pMax.z);
		}

		glm::vec3 pMin;
		glm::vec3 pMax;
	};
	//-------------------------------------------------------------------------
	inline BBox Add(const BBox& _bbox1, const BBox& _bbox2)
	{
		BBox bbox;
		bbox.pMin.x	  	= std::min(_bbox1.pMin.x,_bbox2.pMin.x);
		bbox.pMin.y	  	= std::min(_bbox1.pMin.y,_bbox2.pMin.y);
		bbox.pMin.z	  	= std::min(_bbox1.pMin.z,_bbox2.pMin.z);

		bbox.pMax.x	  	= std::max(_bbox1.pMax.x,_bbox2.pMax.x);
		bbox.pMax.y	  	= std::max(_bbox1.pMax.y,_bbox2.pMax.y);
		bbox.pMax.z	  	= std::max(_bbox1.pMax.z,_bbox2.pMax.z);
		return bbox;
	}
	//-------------------------------------------------------------------------
	inline bool Intersect(	const BBox& 		_bound, 
							const glm::vec3& 	_o,
							const glm::vec3& 	_d,
							float& 				_t0,
							float& 				_t1)
	{
		_t0 = 0.f;
		_t1 = std::numeric_limits<float>::max();
		for(int i=0; i<3; ++i) 
		{
			// Update interval for _i_th bounding box slab
			float invRayDir = 1.f / _d[i];
			float tNear = (_bound.pMin[i] - _o[i]) * invRayDir;
			float tFar  = (_bound.pMax[i] - _o[i]) * invRayDir;

			// Update parametric interval from slab intersection $t$s
			if (tNear > tFar) std::swap(tNear, tFar);
			_t0 = tNear > _t0 ? tNear : _t0;
			_t1 = tFar  < _t1 ? tFar  : _t1;
			if (_t0 > _t1) return false;
		}
		return true;
	}
	//-------------------------------------------------------------------------
	inline BBox Transform(	const BBox& 		_bound, 
							const glm::mat4& 	_t)
	{
		BBox bbox;
		glm::vec4 tmp;
		tmp = _t * glm::vec4(_bound.pMin.x,_bound.pMin.y,_bound.pMin.z,1.f); bbox.Add(glm::vec3(tmp.x,tmp.y,tmp.z));
		tmp = _t * glm::vec4(_bound.pMax.x,_bound.pMin.y,_bound.pMin.z,1.f); bbox.Add(glm::vec3(tmp.x,tmp.y,tmp.z));
		tmp = _t * glm::vec4(_bound.pMax.x,_bound.pMax.y,_bound.pMin.z,1.f); bbox.Add(glm::vec3(tmp.x,tmp.y,tmp.z));
		tmp = _t * glm::vec4(_bound.pMin.x,_bound.pMax.y,_bound.pMin.z,1.f); bbox.Add(glm::vec3(tmp.x,tmp.y,tmp.z));

		tmp = _t * glm::vec4(_bound.pMin.x,_bound.pMin.y,_bound.pMax.z,1.f); bbox.Add(glm::vec3(tmp.x,tmp.y,tmp.z));
		tmp = _t * glm::vec4(_bound.pMax.x,_bound.pMin.y,_bound.pMax.z,1.f); bbox.Add(glm::vec3(tmp.x,tmp.y,tmp.z));
		tmp = _t * glm::vec4(_bound.pMax.x,_bound.pMax.y,_bound.pMax.z,1.f); bbox.Add(glm::vec3(tmp.x,tmp.y,tmp.z));
		tmp = _t * glm::vec4(_bound.pMin.x,_bound.pMax.y,_bound.pMax.z,1.f); bbox.Add(glm::vec3(tmp.x,tmp.y,tmp.z));

		return bbox;
	}
}

#endif


