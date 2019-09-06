#pragma once

#include "Mesh.h"
#include "Texture.h"

class Model
{
public:
	Model() = default;
	Model(unsigned int _verticesSize, float* _vertices, unsigned int _indicesSize, const unsigned int* _indices);
	~Model();

	void SetTexture1(Texture _tex);
	void SetTexture2(Texture _tex);

private:
	Mesh m_modelMesh;
	Texture m_tex1;
	Texture m_tex2;
};

