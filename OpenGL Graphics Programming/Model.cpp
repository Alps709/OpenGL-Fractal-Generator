#include "Model.h"



Model::Model(unsigned int _verticesSize, float* _vertices, unsigned int _indicesSize, const unsigned int* _indices)
{
	m_modelMesh = Mesh(_verticesSize, _vertices, _indicesSize, _indices);
}


Model::~Model()
{
}

void Model::SetTexture1(Texture _tex)
{
	m_tex1 = _tex;
}

void Model::SetTexture2(Texture _tex)
{
	m_tex2 = _tex;
}