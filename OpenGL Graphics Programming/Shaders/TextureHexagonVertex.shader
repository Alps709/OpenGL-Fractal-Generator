#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 colour;
layout (location = 2) in vec2 textureCoord;

uniform float u_currentTime;
uniform mat4 u_PVM;

out vec4 fragmentColour;
out vec2 fragmentTextureCoord;

void main(void)
{
	gl_Position = u_PVM * vec4(position, 1.0f);
	fragmentColour = colour;
	fragmentTextureCoord = textureCoord;
}