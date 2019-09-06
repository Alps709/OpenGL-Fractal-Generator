#version 450 core

in vec4 fragmentColour;
in vec2 fragmentTextureCoord;

out vec4 colour;

uniform float u_currentTime;
uniform sampler2D tex3; 
uniform sampler2D tex4;


void main(void)
{
	colour = mix(texture(tex3, fragmentTextureCoord), texture(tex4, fragmentTextureCoord), abs(sin(u_currentTime))) * fragmentColour.w;
}