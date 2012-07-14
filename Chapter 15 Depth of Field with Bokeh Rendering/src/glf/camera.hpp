#ifndef GLF_CAMERA_HPP
#define GLF_CAMERA_HPP

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <glm/glm.hpp>
#include <glf/memory.hpp>

namespace glf
{
	struct Mouse
	{
		enum Button { LEFT, RIGHT, MIDDLE, SCROLL_UP, SCROLL_DOWN };
		enum State  { PRESS, RELEASE };
	};

	class Camera
	{
		public:
			typedef SmartPointer<Camera>	Ptr;
								Camera(		);
			virtual 		   ~Camera(		);
			virtual void		MouseEvent(	int _x, int _y, Mouse::Button _b, Mouse::State _s)=0;
			virtual void		MoveEvent(	int _x, int _y)=0;
			void 				Orthogonal(	float _halfSize, 
											float _near, 
											float _far);
			void 				Perspective(float _verticalFov, 
											int   _width, 
											int   _height, 
											float _near, 
											float _far);
			void				Projection(	const glm::mat4& _proj, 
											float _near, 
											float _far);
			const glm::mat4&	Projection(	) const;
			const glm::mat4&	View(		) const;
			virtual glm::vec3	Eye(		) const=0;
			virtual glm::vec3	Center(		) const=0;
			virtual glm::vec3	Up(			) const=0;
			inline float 		Near() const  { return nearPlane; };
			inline float 		Far()  const  { return farPlane; };
			inline float 		Fov()  const  { return fov; };
			inline float 		Ratio() const { return ratio; };

		private:
			//-----------------------------------------------------------------
			// Attributes
			//-----------------------------------------------------------------
			mutable glm::mat4 	viewMatrix, 		// World to view space matrix
								projectionMatrix;	// View to normalized projection space matrix
			float 				nearPlane,farPlane,fov,ratio;
	};


	class HybridCamera : public Camera
	{
		public:
								HybridCamera(float _speed=0.2f);
			virtual 		   ~HybridCamera();
			virtual glm::vec3   Eye(		) const;
			virtual glm::vec3   Center(		) const;
			virtual glm::vec3   Up(			) const;
			void 				Speed(		float _speed);
			virtual void		MouseEvent(	int _x, int _y, Mouse::Button _b, Mouse::State _s);
			virtual void		MoveEvent(	int _x, int _y);
			float 	Theta, 			// Theta angle (angle between y-axis and camera direction
					Phi, 			// Phi angle (angle between x-axis and xz-projected camera direction)
					Distance;		// Distance between camera target and camera center

		private:
			//------------------------------------------------------------------
			// Attributes
			//------------------------------------------------------------------
			enum Action { NONE = 0, 
						  MOVE = 1,
						  ZOOM = 2,
						  PANE = 4 };
			//------------------------------------------------------------------
			glm::ivec2 lastPos;	// Last mouse position
			int 		action;		// Zoom mode
			glm::vec3 	center;		// Center of referential
			glm::vec3 	phiAxis;	// Phi axis
			float 		speed;		// Velocity
	};
}

#endif
