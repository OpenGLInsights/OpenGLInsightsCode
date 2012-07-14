//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <glf/camera.hpp>
#include <glf/utils.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <limits>

namespace glf
{
	//-------------------------------------------------------------------------
	// Camera methods
	//-------------------------------------------------------------------------
	Camera::Camera():
	nearPlane(0.1f),
	farPlane(100.f),
	fov(std::numeric_limits<float>::infinity()),
	ratio(1.f)
	{

	}
	//-------------------------------------------------------------------------
	Camera::~Camera()
	{

	}
	//-------------------------------------------------------------------------	
	const glm::mat4& Camera::View() const 
	{
		viewMatrix = glm::lookAt(Eye(),Center(),Up());
		return viewMatrix;
	}
	//-------------------------------------------------------------------------	
	const glm::mat4& Camera::Projection() const
	{
		return projectionMatrix;
	}
	//-------------------------------------------------------------------------
	void Camera::Projection(const glm::mat4& _proj, float _near, float _far)
	{
		nearPlane = _near;
		farPlane  = _far;
		ratio	  = 1.f;
		fov		  = std::numeric_limits<float>::infinity();
		projectionMatrix = _proj;
	}
	//-------------------------------------------------------------------------
	void Camera::Orthogonal(float _halfSize, float _near, float _far)
	{
		nearPlane = _near;
		farPlane  = _far;
		//projectionMatrix = glm::ortho3D(-_halfSize, _halfSize, -_halfSize, _halfSize, _near, _far);
		projectionMatrix   = glm::ortho(-_halfSize, _halfSize, -_halfSize, _halfSize, _near, _far);
	}
	//-------------------------------------------------------------------------
	void Camera::Perspective(float _verticalFov, 
							 int   _width, 
							 int   _height, 
							 float _near, 
							 float _far)
	{
		nearPlane	= _near;
		farPlane	= _far;
		ratio		= _width/float(_height);
		fov			= _verticalFov;
		projectionMatrix = glm::perspective(_verticalFov,    
											 _width/float(_height), 
											 _near, _far);
	}

	//-------------------------------------------------------------------------
	// Hybrid Camera methods
	//-------------------------------------------------------------------------
	HybridCamera::HybridCamera(float _speed):
	Camera(),
	Theta(M_PI*0.5f), 
	Phi(M_PI*1.5f),
	Distance(5.f),
	lastPos(-1,-1),
	action(NONE),
	center(0,0,0),
	phiAxis(0,0,1),
	speed(_speed)
	{

	}
	//-------------------------------------------------------------------------
	HybridCamera::~HybridCamera()
	{

	}
	//-------------------------------------------------------------------------
	void HybridCamera::Speed(float _speed)
	{
		speed = _speed;	
	}
	//-------------------------------------------------------------------------
	glm::vec3 HybridCamera::Eye() const
	{
		float sinTheta = sin(Theta);
		return glm::vec3(Distance*sinTheta*cos(Phi), Distance*sinTheta*sin(Phi), Distance*cos(Theta)) + center;					  
	}
	//-------------------------------------------------------------------------
	glm::vec3 HybridCamera::Center() const
	{
		return center;
	}
	//-------------------------------------------------------------------------
	glm::vec3 HybridCamera::Up() const
	{
		float sinTheta = sin(Theta);
		glm::vec3 dir	= glm::vec3(Distance*sinTheta*cos(Phi), Distance*sinTheta*sin(Phi), Distance*cos(Theta));
		glm::vec3 n		= glm::vec3(0,0,1);
		//glm::vec3 dir	= glm::normalize(Eye());
		glm::vec3 right = glm::normalize(glm::cross(n,dir));
		return glm::normalize(glm::cross(dir,right)); // up
	}
	//-------------------------------------------------------------------------
	void HybridCamera::MouseEvent(int _x, int _y, Mouse::Button _b, Mouse::State _s)
	{
		switch(_b)
		{
			case Mouse::RIGHT  :
				if(_s == Mouse::PRESS)
					action = action | PANE;
				else
					action = action & ~PANE;
				break;
			case Mouse::LEFT   :
				if(_s == Mouse::PRESS)
					action = action | MOVE;
				else
					action = action & ~MOVE;
				break;
			case Mouse::SCROLL_UP   :
				if(_s == Mouse::PRESS)
				{
					//Distance -= glm::clamp((float)exp(Distance-5.f),0.001f,1.f);
					Distance  -= Distance*speed;
					Distance  = std::max(Distance,0.001f);
				}
				break;
			case Mouse::SCROLL_DOWN   :
				if(_s == Mouse::PRESS)
				{
					//Distance += glm::clamp(Distance*2.f,0.001f,1.f);
					Distance  += Distance*speed;
					Distance  = std::min(Distance,1000000000.f);
				}
				break;
			default: 
				break;
		}

		if(action==NONE)
			lastPos.x = lastPos.y = -1;
	}
	//-------------------------------------------------------------------------
	void HybridCamera::MoveEvent(int _x, int _y)
	{
		// Compute centered coordinate
		glm::ivec2 pos(_x,_y);

		// If is the fisrt move after click, update coordinate
		if( lastPos.x == -1 || lastPos.y == -1)
			lastPos = pos;

		if(action & PANE)
		{
			glm::vec3 dir   = center - Eye();
			glm::vec3 right = glm::normalize(glm::cross(dir,phiAxis));
			glm::vec3 up	 = glm::normalize(glm::cross(right,dir));
			
			
			float horizontallyOff = Distance * (pos.x-lastPos.x) / 500.f;
			float verticallyOff   = Distance * (pos.y-lastPos.y) / 1000.f;
			center				 += -horizontallyOff * right +verticallyOff * up;
		}
		else if(action & MOVE)
		{
			Phi   -= (pos.x-lastPos.x) / 100.f;
			Theta -= (pos.y-lastPos.y) / 200.f;

			Theta = glm::clamp(Theta,0.01f,3.14f);
//			Theta = std::max(Theta, 0.01f);
//			Theta = std::min(Theta, 3.14f);

			// Check bound and recenter value 
			// Add 2PI to have positive value (fmod need positive value)
			if(Phi<0)
				Phi += 2.f*M_PI;
			if(Theta<0)
				Theta += 2.f*M_PI;
			Phi   = fmod(Phi,float(2.f*M_PI));
			//Theta = fmod(Theta,glm::Constant::HALF_PI);
			Theta = fmod(Theta,float(M_PI));
		}

		// Update last coordinate
		lastPos = pos;
	}
}
