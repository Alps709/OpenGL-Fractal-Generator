#pragma once
#include "gtc/matrix_transform.hpp"

class Camera
{
public:
	Camera(bool _isPerspectiveView = true);
	~Camera();

	glm::mat4 GetView() const { return m_viewMatrix; }
	glm::mat4 GetProjection() const { return ((m_isPerspectiveView) ? m_perspectiveProjectionMatrix : m_orthoProjectionMatrix); }

	glm::vec3 GetPosition() const { return m_camPosition; }
	glm::vec3 GetLookDirection() const { return m_camLookDir; }
	glm::vec3 GetCamRight() const { return m_camRight; }
	float GetCamSpeed() const { return m_camSpeed; }

	void SetPosition(glm::vec3 _newPos);
	void SetLookDirection(glm::vec3 _newLookDirection);

	void UpdateView();
	void UpdateVectors();
	void ProcessMouseInput(float _xOffset, float _yOffset, bool _limitPitch);


private:
	//Camera vectors
	bool m_isPerspectiveView;
	glm::vec3 m_camPosition;
	glm::vec3 m_camLookDir;
	glm::vec3 m_camUp;
	glm::vec3 m_camRight;
	glm::vec3 m_worldUp;

	//Camera controls (rotation values in degrees)
	float m_camYaw;
	float m_camPitch;
	float m_camRoll;
	float m_camSpeed;
	float m_mouseSens;

	//Camera matrices
	glm::mat4 m_viewMatrix{};
	glm::mat4 m_orthoProjectionMatrix{};
	glm::mat4 m_perspectiveProjectionMatrix{};
};

