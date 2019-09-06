#pragma once
#include <detail/type_vec3.hpp>
#include "Mesh.h"

class Entity
{
public:
	Entity();
	~Entity();

private:
	Mesh m_mesh;

	glm::vec3 m_position;
	glm::vec3 m_rotation;
	glm::vec3 m_scale;
};

