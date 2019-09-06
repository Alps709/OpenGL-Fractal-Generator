#include "Camera.h"



Camera::Camera(bool _isPerspectiveView)
   :m_isPerspectiveView(_isPerspectiveView),
	m_camPosition(glm::vec3{ 0.0f, 0.0f, 500.0f }),
	m_camLookDir(glm::vec3{0.0f, 0.0f, -1.0f}),
	m_worldUp(glm::vec3{ 0.0f, 1.0f, 0.0f }), 
	m_camYaw(-80.0f), m_camPitch(0), m_camRoll(0), m_mouseSens(1.0f), m_camSpeed(0.5),
	m_viewMatrix(glm::lookAt(m_camPosition, m_camPosition + m_camLookDir, m_worldUp)),
	m_orthoProjectionMatrix(glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, 0.0f, 100.0f)),
	m_perspectiveProjectionMatrix(glm::perspective(glm::radians(90.0f), 960.0f/540.0f, -10.0f, 10.0f))
{
	UpdateVectors();
	UpdateView();
}

Camera::~Camera()
= default;

void Camera::SetPosition(glm::vec3 _newPos)
{
	m_camPosition = _newPos;
	UpdateVectors();
	UpdateView();
}

void Camera::SetLookDirection(glm::vec3 _newLookDirection)
{
	m_camLookDir = _newLookDirection;
	UpdateVectors();
	UpdateView();
}

void Camera::UpdateView()
{
	m_viewMatrix = glm::lookAt(m_camPosition, m_camPosition + m_camLookDir, m_worldUp);
}

void Camera::UpdateVectors()
{
	//Update front vector
	m_camLookDir = glm::normalize
                   (
                    glm::vec3(cos(glm::radians(m_camYaw)) * cos(glm::radians(m_camPitch)), //x
                   	sin(glm::radians(m_camPitch)),                                         //y
                   	sin(glm::radians(m_camYaw)) * cos(glm::radians(m_camPitch)))           //z
                   );

	//Update up and right vectors
	m_camRight = glm::normalize(glm::cross(m_camLookDir, m_worldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	m_camUp = glm::normalize(glm::cross(m_camRight, m_camLookDir));
}

void Camera::ProcessMouseInput(float _xOffset, float _yOffset, bool _limitPitch = true)
{
	_xOffset *= m_mouseSens;
	_yOffset *= m_mouseSens;

	m_camYaw += _xOffset;
	
	//Limit the pitch between 90 and -90 degrees, so the camera can't get flipped
	if (_limitPitch)
	{
		m_camPitch += _yOffset;
		if (m_camPitch > 89)
		{
			m_camPitch = 89;
		}
		else if (m_camPitch < -89)
		{
			m_camPitch = -89;
		}
	}

	//Update the vectors from the mouse input
	UpdateVectors();
	UpdateView();
}
