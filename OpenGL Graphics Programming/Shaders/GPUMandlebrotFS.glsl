#version 450 core

out vec4 colour;

uniform float u_leftBorder;
uniform float u_rightBorder;
uniform float u_topBorder;
uniform float u_bottomBorder;

uniform float u_width;
uniform float u_height;

uniform int u_currentIterations;

float EaseInOut(float t, float b, float c, float d)
{
	//t = (end value - start value) this is the value range you want to lerp between
	//b = start value, the starting value of the lerp (the offset from 0)
	//c = current value, of the thing you want to lerp
	//d = Max of current value, or the max range of the variable to want to lerp
	// currentValue * (range of lerp)/currentIterationCount + offset
	return c * t / d + b;
}

double Magnitude(vec2 z)
{
	//The magnitude of a complex vector is its distance from the origin in the imaginary plane
	return sqrt(z.x * z.x + z.y * z.y);
}

void main(void)
{
	const double complexPointPt1 = (u_rightBorder - u_leftBorder) / u_width;
	const double complexPointPt2 = u_topBorder + (gl_FragCoord.y * (u_bottomBorder - u_topBorder) / u_height);

	//Find the point in the complex plane that aligns with the current pixel
	const vec2 complexPoint = vec2(u_leftBorder + (gl_FragCoord.x * complexPointPt1), complexPointPt2);

	vec2 z = vec2(0.0, 0.0);

	//Times the z value by itself as many times as there are iterations, 
	//if the value is greater than 2, then it is unstable
	int iterations = 0;
	while (Magnitude(z) < 2.0f && iterations < u_currentIterations)
	{
		//z = z^2 + c
		z = vec2(z.x * z.x - z.y * z.y, 2 * z.x * z.y) + complexPoint;
		++iterations;
	}

	if (iterations == u_currentIterations)
	{
		//Max iterations was hit so we can say the pixel is in the set
		colour = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	}
	else
	{
		//Set pixel colour based on number of iterations
		//Z didn't escape and we didn't hit the max amount of iterations
		//so this pixel isn't in the set

		float tempRed = EaseInOut(0.4f, 0.6f, sin(iterations), 1.0f);
		float tempGreen = EaseInOut(0.4f, 0.6f, cos(iterations), 1.0f);
		colour = vec4(tempRed, tempGreen, 1.0f, 1.0f);
	}
}