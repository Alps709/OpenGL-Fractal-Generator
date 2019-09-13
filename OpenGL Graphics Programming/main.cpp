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
#include "clock.h"

CClock myClock;

Mesh* myFractalMesh = nullptr;
Shader* myFractalShader = nullptr;
Texture* myFractalTex = nullptr;

struct Pixel
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;

	bool operator==(const Pixel &other) const
	{
		return (r == other.r
			&& g == other.g
			&& b == other.b
			&& a == other.a);
	}
};


Pixel pixelData[Utils::g_SCREEN_HEIGHT][Utils::g_SCREEN_WIDTH];

using std::complex;

void display();
void SetGlobalGLSettings();
void compute_mandelbrot(unsigned int& skipNum, double left, double right, double top, double bottom);
float EaseInOut(float t, float b, float c, float d);

int main(int argc, char** argv)
{
	//Setup and create at glut controlled window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(Utils::g_SCREEN_WIDTH, Utils::g_SCREEN_HEIGHT);
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
	unsigned int skipNum = 0;
	myClock.Initialise();
	compute_mandelbrot(skipNum, -1.5, 0.0, 1.20, -1.0);
	myClock.Process();

	std::cout << "\n\nIt took " << myClock.GetDeltaTick() / 1000.0 << " seconds to calculate the mandlebrot fractal.\n\n";
	std::cout << "Number of checks skipped: " << skipNum << std::endl;

	//for (int i = 0; i < 540; ++i)
	//{
	//	for (int j = 0; j < 960; ++j)
	//	{
	//		pixelData[i][j].r = 0.5f;
	//		pixelData[i][j].g = 1.0f;
	//		pixelData[i][j].b = 0.5f;
	//		pixelData[i][j].a = 1.0f;
	//	}
	//}


	myFractalMesh = new Mesh(Objects::vertices1, Objects::indices1);

	myFractalShader = new Shader();

	myFractalTex = new Texture(0, reinterpret_cast<unsigned char *>(pixelData));
	

	
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

// Render the Mandelbrot set into the image array.
// The parameters specify the region on the complex plane to plot.
void compute_mandelbrot(unsigned int& skipNum, double left, double right, double top, double bottom)
{
	// The number of times to iterate before we assume that a point isn't in the
	// Mandelbrot set.
	// (You may need to turn this up if you zoom further into the set.)
	const int MAX_ITERATIONS = 100;

	const int height = Utils::g_SCREEN_HEIGHT;
	const int width = Utils::g_SCREEN_WIDTH;

	const Pixel black = { 0, 0, 0, 1 };

	for (int i = 0; i < 2; ++i)
	{
		for (int y = 0; y < height; ++y)
		{
			for (int x = i ^ (y % 2); x < width; x += 2)
			{
				// Work out the point in the complex plane that
				// corresponds to this pixel in the output image.
				complex<double> c(left + (x * (right - left) / width),
					top + (y * (bottom - top) / height));

				// Start off z at (0, 0).
				complex<double> z(0.0, 0.0);

				
				if(i == 1 && y > 0 && x > 0 && y < height - 1 && x < width - 1)
				{
					if(pixelData[y - 1][x] == black && pixelData[y + 1][x] == black && pixelData[y][x - 1] == black && pixelData[y][x + 1] == black)
					{
						pixelData[y][x] = black;
						skipNum++;
						continue;
					}
				}
				
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
					// Set color to draw mandelbrot
					// z didn't escape from the circle.
					// This point is in the Mandelbrot set.

					pixelData[y][x] = black;
				}
				else
				{
					//Set pixel colour based on number of iterations
					// z escaped within less than MAX_ITERATIONS
					// iterations. This point isn't in the set.
					pixelData[y][x].r = 255;
					pixelData[y][x].g = abs(sin(iterations)) * 255;
					pixelData[y][x].b = abs(cos(iterations)) * 255;
					pixelData[y][x].a = 255;
				}
			}
		}
	}
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