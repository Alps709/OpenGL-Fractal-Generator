#version 450 core

in vec2 fragmentTextureCoord;

out vec4 colour;

uniform sampler2D u_tex1;

void main(void)
{
	
	if(abs(sin(gl_FragCoord.x/20)) * 540< gl_FragCoord.y)
	{
		colour = vec4(0.0f, 0.0f, 1.0f, 1.0f) * texture(u_tex1, (fragmentTextureCoord));
	}
	else
	{
		colour = texture(u_tex1, (fragmentTextureCoord));
	}
}