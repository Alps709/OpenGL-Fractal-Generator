#include "Debug.h"
#include <freeglut.h>
#include <iostream>
#include <complex>
#include <glm.hpp>

#include "Obj.h"
#include "Utils.h"
#include "Shader.h"
#include "Mesh.h"
#include "Texture.h"
#include "clock.h"
#include "ThreadPool.h"
#include "Input.h"

CClock myClock;
long long u_currentTime = 0;

Mesh* myFractalMesh = nullptr;
Shader* myFractalShader = nullptr;


ThreadPool& threadPool = ThreadPool::GetInstance();

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
	std::cout << "Using " << std::thread::hardware_concurrency() << " threads." <<  std::endl;
	system("pause");

	threadPool.Start();
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
	u_currentTime = static_cast<float>(glutGet(GLUT_ELAPSED_TIME)); //Gets current time
	u_currentTime = u_currentTime * 0.001f; //Convert milliseconds to seconds
	glutPostRedisplay();
	ProcessInput();
}

void display()
{
	//Calculatethe mandlebrot this frame
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

	for (auto& myFuture : myFutures)
	{
		myFuture.get();
	}
	myClock.Process();

	std::cout << "\n\nIt took " << myClock.GetDeltaTick() << " milliseconds to calculate the mandlebrot fractal,"
			  << "\nwhen skipping all similar pixels and using glm::vec2.\n\n";
	std::cout << "Number of checks skipped: " << Utils::skipNum << std::endl;

	Texture myFractalTex = Texture(0, reinterpret_cast<unsigned char *>(Utils::pixelData));

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	myFractalShader->Bind();
	myFractalTex.Bind();
	myFractalShader->SetUniform1i("u_tex1", 0);

	myFractalMesh->Bind();
	glDrawElements(GL_TRIANGLES, myFractalMesh->GetindicesCount(), GL_UNSIGNED_INT, static_cast<void *>(0));

	Mesh::Unbind();
	Texture::Unbind();
	Shader::Unbind();

	glutSwapBuffers();
}

//Mandlebrot function with optimizations that runs sequentially on one thread
void calcMandelbrotOptimised(double _leftBorder, double _rightBorder, double _topBorder, double _bottomBorder)
{
	const int MAX_ITERATIONS = Utils::maxIterations;

	const int height = Utils::SCREEN_HEIGHT;
	const int width = Utils::SCREEN_WIDTH;

	const Utils::Pixel black{ 0, 0, 0, 1 };

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
					if(!Utils::skipOnlyBlack)
					{
						Utils::Pixel tempPix = Utils::pixelData[y - 1][x];
						//Check whether the 4 surrounding orthogonally adjacent pixels are the same colour
						if (Utils::pixelData[y + 1][x] == tempPix && Utils::pixelData[y][x - 1] == tempPix && Utils::pixelData[y][x + 1] == tempPix)
						{
							//If they are, then the current pixel must also be that colour and no calculation is needed
							Utils::pixelData[y][x] = tempPix;
							//Counts number of skipped pixel calculations
							Utils::skipNum++;
							continue;
						}
					}
					//Will only skip pixels of the colour black (black being the pixel after max iterations have been calculated)
					else
					{
						//Check if the pixels above, below, _leftBorder and _rightBorder are the same colour as the current pixel
						if (Utils::pixelData[y - 1][x] == black || Utils::pixelData[y + 1][x] == black && Utils::pixelData[y][x - 1] == black && Utils::pixelData[y][x + 1] == black)
						{
							//Check whether the 4 surrounding orthogonally adjacent pixels are the same colour
							//If they are, then the current pixel must also be that colour and no calculation is needed
							Utils::pixelData[y][x] = black;
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
					Utils::Vec2 c(_leftBorder + (x * (_rightBorder - _leftBorder) / width),
						_topBorder + (y * (_bottomBorder - _topBorder) / height));

					// Start off z at (0, 0).
					Utils::Vec2 z(0.0, 0.0);

					// Iterate z = z^2 + c until either z moves more than 2 units
					// away from (0, 0) in the imaginary plane, or we've iterated too many times.
					while (z.absDist() < 2.0 && iterations < MAX_ITERATIONS)
					{
						//z = z^2 + c
						z = Utils::Vec2(z.x * z.x - z.y * z.y, 2 * z.x * z.y) + c;

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
					Utils::pixelData[y][x] = black;
				}
				else
				{
					//Set pixel colour based on number of iterations
					//Z didn't escape and we didn't hit the max amount of iterations
					//so this pixel isn't in the set
					Utils::pixelData[y][x].r = unsigned char((sin(iterations) * 255) / 2 + 0.75);
					Utils::pixelData[y][x].g = unsigned char((cos(iterations) * 255) / 2 + 0.75);
					Utils::pixelData[y][x].b = 255;
					Utils::pixelData[y][x].a = 255;
				}
			}
		}
	}
}

//Base mandlebrot function with no optimizations that runs sequentially on one thread
void calcMandelbrot(double left, double right, double top, double bottom)
{
	// The number of times to iterate before we assume that a point isn't in the
	// Mandelbrot set.
	const unsigned int MAX_ITERATIONS = Utils::maxIterations;

	const int height = Utils::SCREEN_HEIGHT;
	const int width = Utils::SCREEN_WIDTH;

	const Utils::Pixel black{ 0, 0, 0, 1 };

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
				Utils::pixelData[y][x] = black;
			}
			else
			{
				//Set pixel colour based on number of iterations
				// z escaped within less than MAX_ITERATIONS
				// iterations. This point isn't in the set.
				Utils::pixelData[y][x].r = 255;
				Utils::pixelData[y][x].g = char(sin(iterations) * 255);
				Utils::pixelData[y][x].b = char(cos(iterations) * 255);
				Utils::pixelData[y][x].a = 255;
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

//Mandlebrot function with optimizations that runs concurrently on multiple threads
void calcMandelbrotOY(int _i, int _y)
{
	// The number of times to iterate before we assume that a point isn't in the
	// Mandelbrot set.
	// (You may need to turn this up if you zoom further into the set.)
	const int MAX_ITERATIONS = Utils::maxIterations;

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
			if (!Utils::skipOnlyBlack)
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
			//Will only skip pixels of the colour black (black being the pixel after max iterations have been calculated)
			else
			{
				//Check if the pixels above, below, _leftBorder and _rightBorder are the same colour as the current pixel
				if (Utils::pixelData[_y - 1][x] == black || Utils::pixelData[_y + 1][x] == black && Utils::pixelData[_y][x - 1] == black && Utils::pixelData[_y][x + 1] == black)
				{
					//Check whether the 4 surrounding orthogonally adjacent pixels are the same colour
					//If they are, then the current pixel must also be that colour and no calculation is needed
					Utils::pixelData[_y][x] = black;
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
			Utils::pixelData[_y][x].r = unsigned char(EaseInOut(100, 155, sin(iterations) * 255, 255));
			Utils::pixelData[_y][x].g = unsigned char(EaseInOut(100, 155, cos(iterations) * 255, 255));
			Utils::pixelData[_y][x].b = 255;
			Utils::pixelData[_y][x].a = 255;
		}
	}
}

void ProcessInput()
{
	//Keyboard input
	//Move the camera forward with the w button
	if (
		Input::KeyState['w'] == Input::INPUT_DOWN || Input::KeyState['w'] == Input::INPUT_DOWN_FIRST ||
		Input::KeyState['W'] == Input::INPUT_DOWN || Input::KeyState['W'] == Input::INPUT_DOWN_FIRST
		)
	{
		//move fractal down
		Utils::topBorder += 0.1;
		Utils::bottomBorder += 0.1;

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
		Utils::leftBorder -= 0.1;
		Utils::rightBorder -= 0.1;

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
		Utils::topBorder -= 0.1;
		Utils::bottomBorder -= 0.1;

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
		Utils::leftBorder += 0.1;
		Utils::rightBorder += 0.1;

		//The key is has now been processed for a frame, so set it to the appropriate state
		Input::KeyState['d'] = Input::INPUT_DOWN;
		Input::KeyState['D'] = Input::INPUT_DOWN;
	}
	//Move the camera up with the space button
	if (Input::KeyState[32] == Input::INPUT_DOWN || Input::KeyState[32] == Input::INPUT_DOWN_FIRST)
	{
		//Zoom out of fractal

		//The key is has now been processed for a frame, so set it to the appropriate state
		Input::KeyState[32] = Input::INPUT_DOWN;
	}
	//Move the camera down with the left shift button
	if (Input::SpecialKeyState[GLUT_KEY_SHIFT_L] == Input::INPUT_DOWN || Input::SpecialKeyState[GLUT_KEY_SHIFT_L] == Input::INPUT_DOWN_FIRST)
	{
		//Zoom into fractal

		//The key is has now been processed for a frame, so set it to the appropriate state
		Input::SpecialKeyState[GLUT_KEY_SHIFT_L] = Input::INPUT_DOWN;
	}
}