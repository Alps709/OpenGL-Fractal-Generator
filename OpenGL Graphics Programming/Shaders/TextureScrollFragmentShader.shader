#version 450 core

in vec4 fragmentColour;
in vec2 fragmentTextureCoord;

out vec4 colour;

uniform sampler2D tex2;

void main(void)
{
	colour = texture(tex2, (fragmentTextureCoord)) * fragmentColour.w;
}