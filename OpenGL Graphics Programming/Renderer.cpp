#include "Renderer.h"



Renderer::Renderer()
= default;


Renderer::~Renderer()
= default;

void Renderer::Draw(Mesh& _mesh)
{
	
}

void Renderer::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT);
}
