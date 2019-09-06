#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 colour;

out vec3 fragmentColour;


void main(void)
{
	fragmentColour = colour;
}