#include "Debug.h"
#include <freeglut.h>
#include <iostream>
#include <complex>
#include <glm.hpp>
#include "vld.h"

#include "Obj.h"
#include "Utils.h"
#include "Shader.h"
#include "Mesh.h"
#include "Texture.h"
#include "clock.h"
#include "Input.h"

ThreadPool& threadPool = ThreadPool::GetInstance();

CClock myClock;
long long u_currentTime = 0;

Mesh* myFractalMesh = nullptr;
Shader* myFractalShader = nullptr;

using std::complex;

void ProcessInput();
void Update();
void display();
void SetGlobalGLSettings();
void calcMandelbrotOY(int _i, int _y);
void calcMandelbrotOptimised(double left, double right, double top, double bottom);
void calcMandelbrot(double left, double right, double top, double bottom);
float EaseInOut(float t, float b, float c, float d);

int main(int argc, char** argv)
{
	//Load variables from config file
	//Utils::LoadFile();
	//Start the threadpool and give it the number of threads to use
	threadPool.Start(Utils::threadNum);

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
	
	myClock.Initialise();

	myFractalMesh = new Mesh(Objects::vertices1, Objects::indices1);

	myFractalShader = new Shader();

	///Input function callbacks
	//KeyBoard
	glutKeyboardFunc(Input::KeyBoardDown);
	glutKeyboardUpFunc(Input::KeyBoardUp);
	glutSpecialFunc(Input::SpecialKeyBoardDown);
	glutSpecialUpFunc(Input::SpecialKeyBoardUp);

	//Game loop functions
	glutDisplayFunc(display);
	glutIdleFunc(Update);
	glutMainLoop();

	delete myFractalMesh;
	delete myFractalShader;
	return 0;
}

void Update()
{
	//Update game information
	u_currentTime = static_cast<long long>(glutGet(GLUT_ELAPSED_TIME)); //Gets current time
	u_currentTime = u_currentTime * static_cast<long long>(0.001); //Convert milliseconds to seconds
	glutPostRedisplay();
	ProcessInput();
}

void display()
{
	//Calculate the mandlebrot this frame
	std::vector<std::future<void>> myFutures;
	Utils::skipNum = 0;
	myClock.Process();
	for (int i = 0; i < 2; ++i)
	{
		for (int y = 0; y < Utils::SCREEN_HEIGHT; y++)
		{
			myFutures.push_back(threadPool.Submit(calcMandelbrotOY, i, y));
		}
	}
	
	if(Utils::currentIterationCount <= 1000)
	{
		Utils::currentIterationCount += 0.2f;
	}

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	myFractalShader->Bind();

	//Make sure the threads have finished with the mandlebrot calculation before sending the pixel data to the texture
	for (auto& myFuture : myFutures)
	{
		myFuture.get();
	}
	myClock.Process();

	system("cls");
	std::cout << "FPS: " << 1000.0f / myClock.GetDeltaTick();
	
	Texture myFractalTex = Texture(0, reinterpret_cast<unsigned char *>(Utils::pixelData));
	myFractalTex.Bind();
	myFractalShader->SetUniform1i("u_tex1", 0);

	myFractalMesh->Bind();
	glDrawElements(GL_TRIANGLES, myFractalMesh->GetindicesCount(), GL_UNSIGNED_INT, static_cast<void *>(0));

	Mesh::Unbind();
	Texture::Unbind();
	Shader::Unbind();

	glutSwapBuffers();
}

float EaseInOut(float t, float b, float c, float d)
{
	//t = (end value - start value) this is the value range you want to lerp between
	//b = start value, the starting value of the lerp (the offset from 0)
	//c = current value, of the thing you want to lerp
	//d = Max of current value, or the max range of the variable to want to lerp
	// currentValue * (max - offset)/currentIterationCount + offset
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

//Mandlebrot function with optimizations that runs concurrently on multiple threads
void calcMandelbrotOY(int _i, int _y)
{
	// The number of times to iterate before we assume that a point isn't in the
	// Mandelbrot set.
	// (You may need to turn this up if you zoom further into the set.)
	const unsigned int MAX_ITERATIONS = (unsigned int)Utils::currentIterationCount;

	const int height = Utils::SCREEN_HEIGHT;
	const int width = Utils::SCREEN_WIDTH;

	//Do all the calculations before hand
	//And also create local versions of the variables for faster access
	const double leftBorder = Utils::leftBorder;
	const double complexPointPt1 = (Utils::rightBorder - leftBorder) / width;
	const double complexPointPt2 = Utils::topBorder + (_y * (Utils::bottomBorder - Utils::topBorder) / height);

	const Utils::Pixel black{ 0, 0, 0, 1 };

	//Use bitwise XOR with i and (y % 2) as the inputs, to find the starting x pos 
	//This makes it so the pixels are processed in a checkerboard pattern, with each i iteration processing a different colour of the checkerboard
	//Having it processed in this pattern allows orthogonal checking of the pixels around the current pixel, on i's second iteration
	//So that it can skip calculation of pixels if every pixel around the current one, has the same colour
	for (int x = _i ^ (_y % 2); x < width; x += 2)
	{
		//Check if pixel is not on the outisde border
		if (_i == 1 && _y > 0 && x > 0 && _y < height - 1 && x < width - 1)
		{
			//Will skip pixels of any colours
			if (Utils::skipPixels)
			{
				Utils::Pixel tempPix = Utils::pixelData[_y - 1][x];
				//Check whether the 4 surrounding orthogonally adjacent pixels are the same colour
				if (Utils::pixelData[_y + 1][x] == tempPix && Utils::pixelData[_y][x - 1] == tempPix && Utils::pixelData[_y][x + 1] == tempPix)
				{
					//If they are, then the current pixel must also be that colour and no calculation is needed
					Utils::pixelData[_y][x] = tempPix;
					//Counts number of skipped pixel calculations
					Utils::skipNum++;
					continue;
				}
			}
		}

		unsigned int iterations = 0;
		if (Utils::useVec2)
		{
			// Work out the point in the complex plane that
			// corresponds to this pixel in the output image.
			const Utils::Vec2 complexPoint(leftBorder + (x * complexPointPt1), complexPointPt2);

			// Start off z at (0, 0).
			Utils::Vec2 z(0.0, 0.0);

			// Iterate z = z^2 + c until either z moves more than 2 units
			// away from (0, 0) in the imaginary plane, or we've iterated too many times.
			while (z.absDist() < 2.0 && iterations < MAX_ITERATIONS)
			{
				//z = z^2 + c
				z = Utils::Vec2(z.x * z.x - z.y * z.y, 2 * z.x * z.y) + complexPoint;

				++iterations;
			}
		}
		else
		{
			// Work out the point in the complex plane that
			// corresponds to this pixel in the output image.
			std::complex<double> complexPoint(Utils::leftBorder + (x * (Utils::rightBorder - Utils::leftBorder) / width),
				Utils::topBorder + (_y * (Utils::bottomBorder - Utils::topBorder) / height));

			// Start off z at (0, 0).
			std::complex<double> z(0.0, 0.0);

			// Iterate z = z^2 + c until z moves more than 2 units
			// away from (0, 0), or we've iterated too many times.
			while (abs(z) < 2.0 && iterations < MAX_ITERATIONS)
			{
				z = (z * z) + complexPoint;

				++iterations;
			}
		}

		if (iterations == MAX_ITERATIONS)
		{
			//Max iterations was hit so we can say the pixel is in the set
			Utils::pixelData[_y][x] = black;
		}
		else
		{
			//Set pixel colour based on number of iterations
			//Z didn't escape and we didn't hit the max amount of iterations
			//so this pixel isn't in the set
			Utils::pixelData[_y][x].r = unsigned char(EaseInOut(100, 155, (float)sin(iterations) * 255, 255));
			Utils::pixelData[_y][x].g = unsigned char(EaseInOut(100, 155, (float)cos(iterations) * 255, 255));
			Utils::pixelData[_y][x].b = 255;
			Utils::pixelData[_y][x].a = 255;
		}
	}
}

void ProcessInput()
{
	long double horizDif = fabs(Utils::rightBorder - Utils::leftBorder) * 0.05;
	long double vertDif = fabs(Utils::topBorder - Utils::bottomBorder) * 0.05;
	
	//Keyboard input
	//Move the camera forward with the w button
	if (
		Input::KeyState['w'] == Input::INPUT_DOWN || Input::KeyState['w'] == Input::INPUT_DOWN_FIRST ||
		Input::KeyState['W'] == Input::INPUT_DOWN || Input::KeyState['W'] == Input::INPUT_DOWN_FIRST
		)
	{
		//move fractal down
		Utils::topBorder += vertDif;
		Utils::bottomBorder += vertDif;

		//Print camera position for debugging
		//std::cout << "Camera pos: x: " << myCamera->GetPosition().x << " y: " << myCamera->GetPosition().y << " z: " << myCamera->GetPosition().z  << std::endl;

		//The key is has now been processed for a frame, so set it to the appropriate state
		Input::KeyState['w'] = Input::INPUT_DOWN;
		Input::KeyState['W'] = Input::INPUT_DOWN;
	}
	//Move the camera to the left with the a button
	if (
		Input::KeyState['a'] == Input::INPUT_DOWN || Input::KeyState['a'] == Input::INPUT_DOWN_FIRST ||
		Input::KeyState['A'] == Input::INPUT_DOWN || Input::KeyState['A'] == Input::INPUT_DOWN_FIRST
		)
	{
		//move fractal to the right
		Utils::leftBorder -= horizDif;
		Utils::rightBorder -= horizDif;

		//The key is has now been processed for a frame, so set it to the appropriate state
		Input::KeyState['a'] = Input::INPUT_DOWN;
		Input::KeyState['A'] = Input::INPUT_DOWN;
	}
	//Move the camera backwards with the s button
	if (
		Input::KeyState['s'] == Input::INPUT_DOWN || Input::KeyState['s'] == Input::INPUT_DOWN_FIRST ||
		Input::KeyState['S'] == Input::INPUT_DOWN || Input::KeyState['S'] == Input::INPUT_DOWN_FIRST
		)
	{
		//move fractal up
		Utils::topBorder -= vertDif;
		Utils::bottomBorder -= vertDif;

		//The key is has now been processed for a frame, so set it to the appropriate state
		Input::KeyState['s'] = Input::INPUT_DOWN;
		Input::KeyState['S'] = Input::INPUT_DOWN;
	}
	//Move the camera to the right witht the d button
	if (
		Input::KeyState['d'] == Input::INPUT_DOWN || Input::KeyState['d'] == Input::INPUT_DOWN_FIRST ||
		Input::KeyState['D'] == Input::INPUT_DOWN || Input::KeyState['D'] == Input::INPUT_DOWN_FIRST
		)
	{
		//move fractal to the left
		Utils::leftBorder += horizDif;
		Utils::rightBorder += horizDif;

		//The key is has now been processed for a frame, so set it to the appropriate state
		Input::KeyState['d'] = Input::INPUT_DOWN;
		Input::KeyState['D'] = Input::INPUT_DOWN;
	}
	//Move the camera up with the space button
	if (Input::KeyState[32] == Input::INPUT_DOWN || Input::KeyState[32] == Input::INPUT_DOWN_FIRST)
	{
		//Zoom out of fractal
		Utils::leftBorder += horizDif;
		Utils::rightBorder -= horizDif;
		Utils::topBorder -= vertDif;
		Utils::bottomBorder += vertDif;
		
		//The key is has now been processed for a frame, so set it to the appropriate state
		Input::KeyState[32] = Input::INPUT_DOWN;
	}
	//Move the camera down with the left shift button
	if (Input::SpecialKeyState[GLUT_KEY_SHIFT_L] == Input::INPUT_DOWN || Input::SpecialKeyState[GLUT_KEY_SHIFT_L] == Input::INPUT_DOWN_FIRST)
	{
		//Zoom into fractal
		Utils::leftBorder -= horizDif;
		Utils::rightBorder += horizDif;
		Utils::topBorder += vertDif;
		Utils::bottomBorder -= vertDif;

		//The key is has now been processed for a frame, so set it to the appropriate state
		Input::SpecialKeyState[GLUT_KEY_SHIFT_L] = Input::INPUT_DOWN;
	}
}