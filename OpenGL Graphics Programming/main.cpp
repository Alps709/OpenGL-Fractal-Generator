#include "Debug.h"
#include <freeglut.h>
#include <iostream>
#include <vector>
#include <complex>
#include <glm.hpp>
#include "vld.h"

#include "Obj.h"
#include "Utils.h"
#include "Shader.h"
#include "ShaderLoader.h"
#include "Mesh.h"
#include "Texture.h"
#include "clock.h"
#include "ThreadPool.h"

CClock myClock;

Mesh* myFractalMesh = nullptr;
Shader* myFractalShader = nullptr;
Texture* myFractalTex = nullptr;

ThreadPool& threadPool = ThreadPool::GetInstance();

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

struct Vec2
{
	//This vec2 will be treated as a complex number for the mandlebrot calculation
	//Using a vec2 instead of std::complex increases performance considerably
	double x = 0.0;
	double y = 0.0;

	Vec2(double a, double b)
	{
		x = a;
		y = b;
	}

	bool operator<(const Vec2 _other) const
	{
		if(x < _other.x)
		{
			return true;
		}
		if(x == _other.x)
		{
			return y < _other.y;
		}
		return false;
	}

	Vec2 operator+(const Vec2 _other) const
	{
		return Vec2(x + _other.x,y + _other.y);
	}

	//Finds the absolute distance of the complex number from the origin
	double absDist() const
	{
		//The magnitude of a complex vector is its distance from the origin in the imaginary plane
		return sqrt(x * x + y * y);
	}
};

//Array of pixels that is made into a texture
Pixel pixelData[Utils::g_SCREEN_HEIGHT][Utils::g_SCREEN_WIDTH];

using std::complex;

void display();
void SetGlobalGLSettings();
void calcMandelbrotOptimised(bool _useVec2, bool _skipOnlyBlack, unsigned int& skipNum, double left, double right, double top, double bottom);
void calcMandelbrot(double left, double right, double top, double bottom);
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

	
	//threadPool.Start();

	unsigned int skipNum = 0;
	//for (int i = 0; i < Utils::g_SCREEN_HEIGHT; i++)
	//{
	//	threadPool.Submit(calcMandelbrotOptimised, true, false, skipNum, -2.0, 1.0, 1.20, -1.2);
	//}
	//Create for loop iterating over the height of the window
	//Create a task that passes in the y/height value of the loop and sumbit it to the threadpool
	//Have the task functor take in a y value and compute the mandlebrot function for a single row of pixels
	//have the task functor write directly to the pixel array
	
	myClock.Initialise();
	calcMandelbrotOptimised(true, false, skipNum, -2.0, 1.0, 1.20, -1.2);
	myClock.Process();

	std::cout << "\n\nIt took " << myClock.GetDeltaTick() << " milliseconds to calculate the mandlebrot fractal,"
			  << "\nwhen skipping all similar pixels and using glm::vec2.\n\n";
	std::cout << "Number of checks skipped: " << skipNum << std::endl;

	myFractalMesh = new Mesh(Objects::vertices1, Objects::indices1);

	myFractalShader = new Shader();

	myFractalTex = new Texture(0, reinterpret_cast<unsigned char *>(pixelData));
	
	glutDisplayFunc(display);
	glutMainLoop();

	delete myFractalMesh;
	delete myFractalShader;
	delete myFractalTex;
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
void calcMandelbrotOptimised(bool _useVec2, bool _skipOnlyBlack, unsigned int& _skipNum, double _leftBorder, double _rightBorder, double _topBorder, double _bottomBorder)
{
	// The number of times to iterate before we assume that a point isn't in the
	// Mandelbrot set.
	// (You may need to turn this up if you zoom further into the set.)
	const int MAX_ITERATIONS = Utils::g_maxIterations;

	const int height = Utils::g_SCREEN_HEIGHT;
	const int width = Utils::g_SCREEN_WIDTH;

	const Pixel black{ 0, 0, 0, 1 };

	const float B = 256 * 256;

	for (int i = 0; i < 2; ++i)
	{
		for (int y = 0; y < height; ++y)
		{
			//Use bitwise XOR with i and (y % 2) as the inputs, to find the starting x pos 
			//This makes it so the pixels are processed in a checkerboard pattern, with each i iteration processing a different colour of the checkerboard
			//Having it processed in this pattern allows orthogonal checking of the pixels around the current pixel, on i's second iteration
			//So that it can skip calculation of pixels if every pixel around the current one, has the same colour
			for (int x = i ^ (y % 2); x < width; x += 2)
			{
				//Check if pixel is not on the outisde border
				if (i == 1 && y > 0 && x > 0 && y < height - 1 && x < width - 1)
				{
					//Will skip pixels of any colours
					if(!_skipOnlyBlack)
					{
						Pixel tempPix = pixelData[y - 1][x];
						//Check whether the 4 surrounding orthogonally adjacent pixels are the same colour
						if (pixelData[y + 1][x] == tempPix && pixelData[y][x - 1] == tempPix && pixelData[y][x + 1] == tempPix)
						{
							//If they are, then the current pixel must also be that colour and no calculation is needed
							pixelData[y][x] = tempPix;
							//Counts number of skipped pixel calculations
							_skipNum++;
							continue;
						}
					}
					//Will only skip pixels of the colour black (black being the pixel after max iterations have been calculated)
					else
					{
						//Check if the pixels above, below, _leftBorder and _rightBorder are the same colour as the current pixel
						if (pixelData[y - 1][x] == black || pixelData[y + 1][x] == black && pixelData[y][x - 1] == black && pixelData[y][x + 1] == black)
						{
							//Check whether the 4 surrounding orthogonally adjacent pixels are the same colour
							//If they are, then the current pixel must also be that colour and no calculation is needed
							pixelData[y][x] = black;
							//Counts number of skipped pixel calculations
							_skipNum++;
							continue;
						}
					}
					
					
				}
				
				unsigned int iterations = 0;
				if (_useVec2)
				{
					// Work out the point in the complex plane that
					// corresponds to this pixel in the output image.
					Vec2 c(_leftBorder + (x * (_rightBorder - _leftBorder) / width),
						_topBorder + (y * (_bottomBorder - _topBorder) / height));

					// Start off z at (0, 0).
					Vec2 z(0.0, 0.0);

					// Iterate z = z^2 + c until either z moves more than 2 units
					// away from (0, 0) in the imaginary plane, or we've iterated too many times.
					while (z.absDist() < 2.0 && iterations < MAX_ITERATIONS)
					{
						//z = z^2 + c
						z = Vec2(z.x * z.x - z.y * z.y, 2 * z.x * z.y) + c;

						++iterations;
					}
				}
				else
				{
					// Work out the point in the complex plane that
					// corresponds to this pixel in the output image.
					complex<double> c(_leftBorder + (x * (_rightBorder - _leftBorder) / width),
						_topBorder + (y * (_bottomBorder - _topBorder) / height));

					// Start off z at (0, 0).
					complex<double> z(0.0, 0.0);

					// Iterate z = z^2 + c until z moves more than 2 units
					// away from (0, 0), or we've iterated too many times.
					while (abs(z) < 2.0 && iterations < MAX_ITERATIONS)
					{
						z = (z * z) + c;
						
						++iterations;
					}
				}

				if (iterations == MAX_ITERATIONS)
				{
					//Max iterations was hit so we can say the pixel is in the set
					pixelData[y][x] = black;
				}
				else
				{
					//Set pixel colour based on number of iterations
					//Z didn't escape and we didn't hit the max amount of iterations
					//so this pixel isn't in the set
					pixelData[y][x].r = unsigned char((sin(iterations) * 255) / 2 + 0.75);
					pixelData[y][x].g = unsigned char((cos(iterations) * 255) / 2 + 0.75);
					pixelData[y][x].b = 255;
					pixelData[y][x].a = 255;
				}
			}
		}
	}
}

void calcMandelbrot(double left, double right, double top, double bottom)
{
	// The number of times to iterate before we assume that a point isn't in the
	// Mandelbrot set.
	const unsigned int MAX_ITERATIONS = Utils::g_maxIterations;

	const int height = Utils::g_SCREEN_HEIGHT;
	const int width = Utils::g_SCREEN_WIDTH;

	const Pixel black{ 0, 0, 0, 1 };

	for (int y = 0; y < height; ++y)
	{
		//Use bitwise XOR with i and (y % 2) as the inputs, 
		//This makes it so the pixels are processed in a checkerboard pattern, with each i iteration processing a different colour of the checkerboard
		//Having it processed in this pattern allows orthogonal checking of the pixels around the current pixel, on i's second iteration
		//So that it can skip calculation of pixels if every pixel around the current one, has the same colour
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
			while (abs(z) < 2.0 && static_cast<unsigned int>(iterations) < MAX_ITERATIONS)
			{
				z = (z * z) + c;

				++iterations;
			}

			if (iterations == MAX_ITERATIONS)
			{
				//Max iterations was hit so say the pixel is in the set
				pixelData[y][x] = black;
			}
			else
			{
				//Set pixel colour based on number of iterations
				// z escaped within less than MAX_ITERATIONS
				// iterations. This point isn't in the set.
				pixelData[y][x].r = 255;
				pixelData[y][x].g = char(sin(iterations) * 255);
				pixelData[y][x].b = char(cos(iterations) * 255);
				pixelData[y][x].a = 255;
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