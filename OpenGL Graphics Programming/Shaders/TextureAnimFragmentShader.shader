#version 450 core

in vec3 fragmentColour;

out vec4 colour;

void main(void)
{
	colour = fragmentColour;
}