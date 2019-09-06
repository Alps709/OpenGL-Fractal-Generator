#pragma once

#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"



class Renderer
{
public:
	Renderer();
	~Renderer();

	static void Draw(Mesh& _mesh);
	static void Clear();
};

