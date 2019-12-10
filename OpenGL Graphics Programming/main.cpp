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
#include "TextLabel.h"
#include <fstream>

enum GameState
{
	MENU = 0,
	CPUMODE = 1,
	GPUMODE = 2
};

ThreadPool& threadPool = ThreadPool::GetInstance();

bool testFirstFrame = true;

CClock myClock;
long long u_currentTime = 0;

Mesh* myFractalMesh = nullptr;
Shader* myCPUFractalShader = nullptr;
Shader* myGPUFractalShader = nullptr;

using std::complex;

void ProcessInput();
void Update();
void Render();
void SetGlobalGLSettings();
void calcMandelbrotOY(int _i, int _y);
float EaseInOut(float t, float b, float c, float d);
static void LoadFile();

int main(int argc, char** argv)
{
	//Load variables from config file
	LoadFile();

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

	myCPUFractalShader = new Shader();
	myGPUFractalShader = new Shader("Shaders/DeafultVS.glsl", "Shaders/GPUMandlebrotFS.glsl");

	///Input function callbacks
	//KeyBoard
	glutKeyboardFunc(::KeyBoardDown);
	glutKeyboardUpFunc(::KeyBoardUp);
	glutSpecialFunc(::SpecialKeyBoardDown);
	glutSpecialUpFunc(::SpecialKeyBoardUp);

	//Game loop functions
	glutDisplayFunc(Render);
	glutIdleFunc(Update);
	glutMainLoop();

	//Cleanup
	threadPool.Stop();
	delete myFractalMesh;
	delete myCPUFractalShader;
	return 0;
}

void Update()
{
	//Update game information
	u_currentTime = static_cast<long long>(glutGet(GLUT_ELAPSED_TIME)) * static_cast<long long>(0.001); //Gets current time and convert it to milliseconds
	glutPostRedisplay();
	ProcessInput();
}

void Render()
{
	//Calculate the mandlebrot this frame
	std::vector<std::future<void>> myFutures;

	//Use maxIterations for the first test
	if(testFirstFrame)
		//Calculate full mandlebrot image at max iterations to test performance
	{
		//Start timer
		myClock.Process();

		//Do two passes on calculating the image data, this calculates the colour of every second pixel on the x axis, so the calculations are done in a checker board fashion
		//(first i calculates the black squares of the checkerboard, second i iterates over the white squares), this allows for the pixel skipping optimization
		for (int i = 0; i < 2; ++i)
		{
			//Create a task to calculate each row of pixels for the texture
			for (int y = 0; y < Utils::SCREEN_HEIGHT; y++)
			{
				myFutures.push_back(threadPool.Submit(calcMandelbrotOY, i, y));
			}
		}
		//Make sure the threads have finished with the mandlebrot calculation before sending the pixel data to the texture
		for (auto& myFuture : myFutures)
		{
			myFuture.get();
		}
		myClock.Process();
	}
	else
		//Have iterations increase over time for the awesome animated texture
	{
		//Start timer
		myClock.Process();

		//Do two passes on calculating the image data, this calculates the colour of every second pixel on the x axis, so the calculations are done in a checker board fashion
		//(first i calculates the black squares of the checkerboard, second i iterates over the white squares), this allows for the pixel skipping optimization
		for (int i = 0; i < 2; ++i)
		{
			////Create a task to calculate each row of pixels for the texture
			for (int y = 0; y < Utils::SCREEN_HEIGHT; y++)
			{
				myFutures.push_back(threadPool.Submit(calcMandelbrotOY, i, y));
			}
		}

		//Increase the iterations every 5 frames so it gives a cool animation
		if (Utils::currentIterationCount <= Utils::maxIterations)
		{
			Utils::currentIterationCount += 0.2f;
		}

		//Make sure the threads have finished with the mandlebrot calculation before sending the pixel data to the texture
		for (auto& myFuture : myFutures)
		{
			myFuture.get();
		}

		//End timer
		myClock.Process();
	}

	system("cls");
	std::cout << "FPS: " << 1000.0f / myClock.GetDeltaTick();

	glClear(GL_COLOR_BUFFER_BIT);

	myCPUFractalShader->Bind();
	
	Texture myFractalTex = Texture(0, reinterpret_cast<unsigned char *>(Utils::pixelData));
	myFractalTex.Bind();
	myCPUFractalShader->SetUniform1i("u_tex1", 0);

	myFractalMesh->Bind();
	glDrawElements(GL_TRIANGLES, myFractalMesh->GetindicesCount(), GL_UNSIGNED_INT, static_cast<void *>(0));

	Mesh::Unbind();
	Texture::Unbind();
	Shader::Unbind();

	glutSwapBuffers();

	if(testFirstFrame)
	{
		std::cout << "\nIt took " << myClock.GetDeltaTick()
				  << " milliseconds to calculate the full mandlebrot fractal with "
				  << Utils::maxIterations << " iterations.\n";
		system("pause");
		testFirstFrame = false;
	}
}

float EaseInOut(float t, float b, float c, float d)
{
	//t = (end value - start value) this is the value range you want to lerp between
	//b = start value, the starting value of the lerp (the offset from 0)
	//c = current value, of the thing you want to lerp
	//d = Max of current value, or the max range of the variable to want to lerp
	// currentValue * (range of lerp)/currentIterationCount + offset
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
	//The max number of times it will iterate before assuming the pixel is in the set
	const unsigned int MAX_ITERATIONS = (testFirstFrame) ? static_cast<unsigned int>(Utils::maxIterations) : static_cast<unsigned int>(Utils::currentIterationCount);

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
		//Check if this is the second iteration and that this pixel is not on the outisde border 
		//(if it's on the outside border then there aren't 4 surrounding pixels)
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
		//Check if it is using the vec2 optimization
		if (Utils::useVec2)
		{
			//Find the point in the complex plane that aligns with the current pixel
			const Utils::Vec2 complexPoint(leftBorder + (x * complexPointPt1), complexPointPt2);

			Utils::Vec2 z(0.0, 0.0);

			//Times the z value by itself as many times as there are iterations, 
			//if the value is greater than 2, then it has moved m
			while (z.absDist() < 2.0 && iterations < MAX_ITERATIONS)
			{
				//z = z^2 + c
				z = Utils::Vec2(z.x * z.x - z.y * z.y, 2 * z.x * z.y) + complexPoint;

				++iterations;
			}
		}
		else
			//Use std::complex instead of vec2 (this has significant performance reductions)
		{
			//Find the point in the complex plane that aligns with the current pixel
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

			//Utils::pixelData[_y][x].r = unsigned char(EaseInOut(100, 155, static_cast<float>(sin(iterations)) * 255, 255));
			//Utils::pixelData[_y][x].g = unsigned char(EaseInOut(100, 155, static_cast<float>(cos(iterations)) * 255, 255));
			//Utils::pixelData[_y][x].b = 255;
			//Utils::pixelData[_y][x].a = 255;

			unsigned char tempRed = unsigned char(EaseInOut(100, 155, static_cast<float>(sin(iterations)) * 255, 255));
			unsigned char tempGreen = unsigned char(EaseInOut(100, 155, static_cast<float>(cos(iterations)) * 255, 255));
			Utils::pixelData[_y][x] = Utils::Pixel(tempRed, tempGreen, 255, 255);
		}
	}
}

void ProcessInput()
{
	const float deltaTime = myClock.GetDeltaTick();
	const long double horizDif = fabs(Utils::rightBorder - Utils::leftBorder) * 0.0005 * deltaTime;
	const long double vertDif = fabs(Utils::topBorder - Utils::bottomBorder) * 0.0005 * deltaTime;
	
	//Keyboard input
	//Move the camera forward with the w button
	if (inputManager.KeyState['w'] == inputManager.INPUT_DOWN || inputManager.KeyState['w'] == inputManager.INPUT_DOWN_FIRST ||
		inputManager.KeyState['W'] == inputManager.INPUT_DOWN || inputManager.KeyState['W'] == inputManager.INPUT_DOWN_FIRST)
	{
		//move fractal down
		Utils::topBorder += vertDif;
		Utils::bottomBorder += vertDif;

		//Print camera position for debugging
		//std::cout << "Camera pos: x: " << myCamera->GetPosition().x << " y: " << myCamera->GetPosition().y << " z: " << myCamera->GetPosition().z  << std::endl;

		//The key is has now been processed for a frame, so set it to the appropriate state
		inputManager.KeyState['w'] = inputManager.INPUT_DOWN;
		inputManager.KeyState['W'] = inputManager.INPUT_DOWN;
	}
	//Move the camera to the left with the a button
	if (inputManager.KeyState['a'] == inputManager.INPUT_DOWN || inputManager.KeyState['a'] == inputManager.INPUT_DOWN_FIRST ||
		inputManager.KeyState['A'] == inputManager.INPUT_DOWN || inputManager.KeyState['A'] == inputManager.INPUT_DOWN_FIRST)
	{
		//move fractal to the right
		Utils::leftBorder -= horizDif;
		Utils::rightBorder -= horizDif;

		//The key is has now been processed for a frame, so set it to the appropriate state
		inputManager.KeyState['a'] = inputManager.INPUT_DOWN;
		inputManager.KeyState['A'] = inputManager.INPUT_DOWN;
	}
	//Move the camera backwards with the s button
	if (inputManager.KeyState['s'] == inputManager.INPUT_DOWN || inputManager.KeyState['s'] == inputManager.INPUT_DOWN_FIRST ||
		inputManager.KeyState['S'] == inputManager.INPUT_DOWN || inputManager.KeyState['S'] == inputManager.INPUT_DOWN_FIRST)
	{
		//move fractal up
		Utils::topBorder -= vertDif;
		Utils::bottomBorder -= vertDif;

		//The key is has now been processed for a frame, so set it to the appropriate state
		inputManager.KeyState['s'] = inputManager.INPUT_DOWN;
		inputManager.KeyState['S'] = inputManager.INPUT_DOWN;
	}
	//Move the camera to the right witht the d button
	if (inputManager.KeyState['d'] == inputManager.INPUT_DOWN || inputManager.KeyState['d'] == inputManager.INPUT_DOWN_FIRST ||
		inputManager.KeyState['D'] == inputManager.INPUT_DOWN || inputManager.KeyState['D'] == inputManager.INPUT_DOWN_FIRST)
	{
		//move fractal to the left
		Utils::leftBorder += horizDif;
		Utils::rightBorder += horizDif;

		//The key is has now been processed for a frame, so set it to the appropriate state
		inputManager.KeyState['d'] = inputManager.INPUT_DOWN;
		inputManager.KeyState['D'] = inputManager.INPUT_DOWN;
	}
	//Move the camera up with the space button
	if (inputManager.KeyState[32] == inputManager.INPUT_DOWN || inputManager.KeyState[32] == inputManager.INPUT_DOWN_FIRST)
	{
		//Zoom out of fractal
		Utils::leftBorder += horizDif;
		Utils::rightBorder -= horizDif;
		Utils::topBorder -= vertDif;
		Utils::bottomBorder += vertDif;
		
		//The key is has now been processed for a frame, so set it to the appropriate state
		inputManager.KeyState[32] = inputManager.INPUT_DOWN;
	}
	//Move the camera down with the left shift button
	if (inputManager.SpecialKeyState[GLUT_KEY_SHIFT_L] == inputManager.INPUT_DOWN || inputManager.SpecialKeyState[GLUT_KEY_SHIFT_L] == inputManager.INPUT_DOWN_FIRST)
	{
		//Zoom into fractal
		Utils::leftBorder -= horizDif;
		Utils::rightBorder += horizDif;
		Utils::topBorder += vertDif;
		Utils::bottomBorder -= vertDif;

		//The key is has now been processed for a frame, so set it to the appropriate state
		inputManager.SpecialKeyState[GLUT_KEY_SHIFT_L] = inputManager.INPUT_DOWN;
	}
}

static void LoadFile()
{
	std::string line;
	std::ifstream configfile;
	configfile.open("Config/Config.txt");

	if(configfile.fail())
	{
		std::cout << "Failed to open the config file!";
		system("pause");
		exit(-1);
	}

	if(configfile.is_open())
	{
		//Error checking of file reading
		try
		{
			while(std::getline(configfile, line))
			{
				std::istringstream lineInput(line.substr(line.find(": ") + 1));
				if(line.find("Thread count") != -1)
				{
					lineInput >> Utils::threadNum;

					if (Utils::threadNum > std::thread::hardware_concurrency())
					{
						Utils::threadNum = std::thread::hardware_concurrency();
						std::cout << "The number of threads you declared in the config file was more than what the hardware supports."
							<< "\nThe number of threads will be set to the max amount of logical threads the hardware actually supports.\n";
					}
					if (Utils::threadNum <= 0)
					{
						std::cout << "\n\nThere was a problem reading the config file! Please make sure you changed only the numbers!\n\n";
						system("pause");
						exit(-1);
					}
				}
				else if(line.find("Max iterations") != -1)
				{
					lineInput >> Utils::maxIterations;

					if(Utils::maxIterations > 1000)
					{
						Utils::maxIterations = 1000;
						std::cout << "\n\nThe Max iteration count you set was too high."
								  << "\nThe number of iterations will be set to 1000,\n" 
								  << "as anything higher will be unseeable because of the 32 bit floating point precision limits.\n";
					}
					if (Utils::maxIterations <= 0)
					{
						std::cout << "\n\nThere was a problem reading the config file! Please make sure you changed only the numbers!\n\n";
						system("pause");
						exit(-1);
					}
				}
				else if(line.find("Use vec2 optimization") != -1)
				{
					unsigned int tempInt;
					lineInput >> tempInt;
					Utils::useVec2 = tempInt;
				}
				else if(line.find("Use pixel skipping optimization") != -1)
				{
					unsigned int tempInt;
					lineInput >> tempInt;
					Utils::skipPixels = tempInt;
				}
			}
		}
		catch (...)
		{
			std::cout << "\n\nThere was a problem reading the config file! Please make sure you changed only the numbers!\n\n";
			system("pause");
			exit(-1);
		}
	}

	std::cout << "Loaded config file!\n\n"
		<< "The program will use " << Utils::threadNum << " threads.\n"
		<< "The max amount of iterations is set to " << Utils::maxIterations << ".\n"
		<< "Using vec2 optimization: " << ((Utils::useVec2 == 1) ? "true\n" : "false\n")
		<< "Using pixel skipping optimization: " << ((Utils::skipPixels == 1) ? "true\n" : "false\n");
	system("pause");
}