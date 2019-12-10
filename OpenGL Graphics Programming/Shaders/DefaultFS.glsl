#version 450 core

in vec2 fragmentTextureCoord;

out vec4 colour;

uniform sampler2D u_tex1;

void main(void)
{
	colour = texture(u_tex1, (fragmentTextureCoord));
}