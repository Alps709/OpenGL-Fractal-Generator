#include "Debug.h"
#include <freeglut.h>
#include <iostream>
#include <vector>
#include <complex>
//#include "vld.h"

#include "Obj.h"
#include "Utils.h"
#include "Shader.h"
#include "ShaderLoader.h"
#include "Mesh.h"
#include "Texture.h"

Mesh* myFractalMesh = nullptr;
Shader* myFractalShader = nullptr;
Texture* myFractalTex = nullptr;

const static int SCREEN_WIDTH = 960;
const static int SCREEN_HEIGHT = 540;

struct Pixel
{
	float r;
	float g;
	float b;
	float a;
};

Pixel pixelData[SCREEN_WIDTH][SCREEN_HEIGHT];

using std::complex;

void display();
void SetGlobalGLSettings();
void compute_mandelbrot(double left, double right, double top, double bottom);
float EaseInOut(float t, float b, float c, float d);

int main(int argc, char** argv)
{
	//Setup and create at glut controlled window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(Utils::SCREEN_WIDTH, Utils::SCREEN_HEIGHT);
	glutCreateWindow("Anton's OpenGL Window");

	//Print openGL version
	std::cout << "Currently using OpenGL version: " << glGetString(GL_VERSION) << std::endl;

	//Set up all gl function callbacks based on pc hardware
	if (glewInit() != GLEW_OK)
	{
		//If glew setup failed then application will not run graphics correctly
		std::cout << "Glew Initialization Failed. Aborting Application." << std::endl;
		system("pause");
		return 0;
	}

	SetGlobalGLSettings();

	for (int i = 0; i < Utils::SCREEN_WIDTH; ++i)
	{
		for (int j = 0; j < Utils::SCREEN_HEIGHT; ++j)
		{
			pixelData[i][j].r = 0.5f;
			pixelData[i][j].g = 1.0f;
			pixelData[i][j].b = 0.5f;
			pixelData[i][j].a = 1.0f;
		}
	}


	myFractalMesh = new Mesh(Objects::vertices1, Objects::indices1);

	myFractalShader = new Shader();

	myFractalTex = new Texture(0, reinterpret_cast<float *>(pixelData));
	

	
	glutDisplayFunc(display);
	glutMainLoop();
	return 0;
}
void display()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	myFractalShader->Bind();
	myFractalTex->Bind();
	myFractalShader->SetUniform1i("u_tex1", 0);

	myFractalMesh->Bind();
	glDrawElements(GL_TRIANGLES, myFractalMesh->GetindicesCount(), GL_UNSIGNED_INT, static_cast<void *>(0));
	Mesh::Unbind();
	Texture::Unbind();
	Shader::Unbind();
	glutSwapBuffers();
}

//void display()
//{
//	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
//	glClear(GL_COLOR_BUFFER_BIT);
//
//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();
//	const int width = glutGet(GLUT_WINDOW_WIDTH);
//	const int height = glutGet(GLUT_WINDOW_HEIGHT);
//	glOrtho(0, width, 0, height, -1, 1);
//
//	glMatrixMode(GL_MODELVIEW);
//	glLoadIdentity();
//
//	compute_mandelbrot(-1.0, 0.5, 0.625, -1.2);
//	glutSwapBuffers();
//}

// Render the Mandelbrot set into the image array.
// The parameters specify the region on the complex plane to plot.
void compute_mandelbrot(double left, double right, double top, double bottom)
{
	// The number of times to iterate before we assume that a point isn't in the
	// Mandelbrot set.
	// (You may need to turn this up if you zoom further into the set.)
	const int MAX_ITERATIONS = 100;

	const int width = glutGet(GLUT_WINDOW_WIDTH);
	const int height = glutGet(GLUT_WINDOW_HEIGHT);

	glBegin(GL_POINTS); // start drawing in single pixel mode
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			// Work out the point in the complex plane that
			// corresponds to this pixel in the output image.
			complex<double> c(left + (x * (right - left) / width),
				top + (y * (bottom - top) / height));

			// Start off z at (0, 0).
			complex<double> z(0.0, 0.0);

			// Iterate z = z^2 + c until z moves more than 2 units
			// away from (0, 0), or we've iterated too many times.
			int iterations = 0;
			while (abs(z) < 2.0 && iterations < MAX_ITERATIONS)
			{
				z = (z * z) + c;

				++iterations;
			}

			if (iterations == MAX_ITERATIONS)
			{
				glColor3f(0.0, 0.0, 0.0); // Set color to draw mandelbrot
				// z didn't escape from the circle.
				// This point is in the Mandelbrot set.
				glVertex2i(x, y);
			}
			else
			{
				//Set pixel colour based on number of iterations
				glColor3f(1.0, sin(iterations) * 100, cos(iterations) * 100);
				// z escaped within less than MAX_ITERATIONS
				// iterations. This point isn't in the set.
				glVertex2i(x, y);
			}
		}
	}
	glEnd();
}	

float EaseInOut(float t, float b, float c, float d)
{
	//t = (end value - start value) this is the value range you want to lerp between
	//b = start value, the starting value of the lerp (the offset from 0)
	//c = current value, of the thing you want to lerp
	//d = Max of current value, or the max range of the variable to want to lerp
	// currentValue * (max - offset)/maxIterations + offset
	return c * t / d + b;
}

void SetGlobalGLSettings()
{
	///Set global settings
	//Cull the back faces of objects 
	GLCall(glCullFace(GL_BACK));

	//Normal are calculated counter clockwise
	GLCall(glFrontFace(GL_CCW));

	//Doesn't draw stuff that is not facing the screen
	//GLCall(glEnable(GL_CULL_FACE));

	////Setup alpha blending
	//GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	//GLCall(glEnable(GL_BLEND));

	//Allows the program to continue executing after the glutMainLoop()
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);

	//Sets the clear colour when calling glClear()
	GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
}