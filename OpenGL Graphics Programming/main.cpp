//#include "Debug.h"
//#include <freeglut.h>
//#include <iostream>
//#include <vector>
//#include <complex>
////#include "vld.h"
//
//#include "Utils.h"
//#include "Shader.h"
//#include "ShaderLoader.h"
//#include "Mesh.h"
//
//glm::mat4 modelMatrix;
//
//Mesh* myFractalMesh = nullptr;
//
////Second quad
//Shader* myFractalShader = nullptr;
//
////Projection matrix
//glm::mat4 PV;
//glm::mat4 PVM;
//
////Uniforms
//GLfloat u_currentTime = 0;
//int u_frameNum = 0;
//
////Animated texture
//std::vector<float> fractalPoints;
//
//void Render();
//void Update();
//
////void ProcessInput();
//
//void SetGlobalGLSettings();
//
//void compute_mandelbrot(double left, double right, double top, double bottom);
//
//int main(int argc, char ** argv)
//{
//	//Setup and create at glut controlled window
//	glutInit(&argc, argv);
//	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
//	glutInitWindowPosition(100, 100);
//	glutInitWindowSize(Utils::SCREEN_WIDTH, Utils::SCREEN_HEIGHT);
//	glutCreateWindow("Anton's OpenGL Window");
//
//	//Print openGL version
//	std::cout << "Currently using OpenGL version: " << glGetString(GL_VERSION) << std::endl;
//
//	//Set up all gl function callbacks based on pc hardware
//	if (glewInit() != GLEW_OK)
//	{
//		//If glew setup failed then application will not run graphics correctly
//		std::cout << "Glew Initialization Failed. Aborting Application." << std::endl;
//		system("pause");
//		return 0;
//	}
//
//	SetGlobalGLSettings();
//
//	//Calculate mandlebrot fractal here
//
//	//After calculating all the pixel colours add it to a mesh
//	//myFractalMesh = new Mesh(sizeof(vertices1), vertices1, sizeof(indices1), indices1);
//
//	//myFractalShader = new Shader("Shaders/TextureAnimVertexShader.shader",
//	//                                "Shaders/TextureAnimFragmentShader.shader");
//
//	//Update and render function callbacks
//	glutDisplayFunc(Render);
//	glutIdleFunc(Update);
//
//	
//
//	glutMainLoop();
//
//	//Delete all the heap allocated objects
//	//delete myFractalShader;
//	//delete myFractalMesh;
//	return 0;
//}
//void Update()
//{
//	//Update game information
//	u_currentTime = static_cast<float>(glutGet(GLUT_ELAPSED_TIME)); //Gets current time
//	u_currentTime = u_currentTime * 0.001f; //Convert milliseconds to seconds
//	glutPostRedisplay();
//	//ProcessInput();
//}
//
////void ProcessInput()
////{
////	//Keyboard input
////	//Move the camera forward with the w button
////	if (
////		Utils::KeyState['w'] == Utils::INPUT_DOWN || Utils::KeyState['w'] == Utils::INPUT_DOWN_FIRST ||
////		Utils::KeyState['W'] == Utils::INPUT_DOWN || Utils::KeyState['W'] == Utils::INPUT_DOWN_FIRST
////		)
////	{
////		//move camera forward
////		myCamera->SetPosition(myCamera->GetPosition() + myCamera->GetLookDirection() * myCamera->GetCamSpeed());
////
////		//Print camera position for debugging
////		//std::cout << "Camera pos: x: " << myCamera->GetPosition().x << " y: " << myCamera->GetPosition().y << " z: " << myCamera->GetPosition().z  << std::endl;
////
////		//The key is has now been processed for a frame so next frame if it hasn't been set to up,
////		//change the enum state
////		Utils::KeyState['w'] = Utils::INPUT_DOWN;
////		Utils::KeyState['W'] = Utils::INPUT_DOWN;
////	}
////	//Move the camera to the left with the a button
////	if (
////		Utils::KeyState['a'] == Utils::INPUT_DOWN || Utils::KeyState['a'] == Utils::INPUT_DOWN_FIRST ||
////		Utils::KeyState['A'] == Utils::INPUT_DOWN || Utils::KeyState['A'] == Utils::INPUT_DOWN_FIRST
////		)
////	{
////		//move camera left
////		myCamera->SetPosition(myCamera->GetPosition() - myCamera->GetCamRight() * myCamera->GetCamSpeed());
////
////		//The key is has now been processed for a frame so next frame if it hasn't been set to up,
////		//change the enum state
////		Utils::KeyState['a'] = Utils::INPUT_DOWN;
////		Utils::KeyState['A'] = Utils::INPUT_DOWN;
////	}
////	//Move the camera backwards with the s button
////	if (
////		Utils::KeyState['s'] == Utils::INPUT_DOWN || Utils::KeyState['s'] == Utils::INPUT_DOWN_FIRST ||
////		Utils::KeyState['S'] == Utils::INPUT_DOWN || Utils::KeyState['S'] == Utils::INPUT_DOWN_FIRST
////		)
////	{
////		//move camera backward
////		myCamera->SetPosition(myCamera->GetPosition() - myCamera->GetLookDirection() * myCamera->GetCamSpeed());
////
////		//The key is has now been processed for a frame so next frame if it hasn't been set to up,
////		//change the enum state
////		Utils::KeyState['s'] = Utils::INPUT_DOWN;
////		Utils::KeyState['S'] = Utils::INPUT_DOWN;
////	}
////	//Move the camera to the right witht the d button
////	if (
////		Utils::KeyState['d'] == Utils::INPUT_DOWN || Utils::KeyState['d'] == Utils::INPUT_DOWN_FIRST ||
////		Utils::KeyState['D'] == Utils::INPUT_DOWN || Utils::KeyState['D'] == Utils::INPUT_DOWN_FIRST
////		)
////	{
////		//move camera right
////		myCamera->SetPosition(myCamera->GetPosition() + myCamera->GetCamRight() * myCamera->GetCamSpeed());
////
////		//The key is has now been processed for a frame so next frame if it hasn't been set to up,
////		//change the enum state
////		Utils::KeyState['d'] = Utils::INPUT_DOWN;
////		Utils::KeyState['D'] = Utils::INPUT_DOWN;
////	}
////	//Move the camera up with the space button
////	if (Utils::KeyState[32] == Utils::INPUT_DOWN || Utils::KeyState[32] == Utils::INPUT_DOWN_FIRST)
////	{
////		glm::vec3 tempVec = myCamera->GetPosition();
////		tempVec.y += 2.5f;
////		myCamera->SetPosition(glm::vec3(tempVec));
////
////		//The key is has now been processed for a frame so next frame if it hasn't been set to up,
////		//change the enum state
////		Utils::KeyState[32] = Utils::INPUT_DOWN;
////	}
////	//Move the camera down with the left shift button
////	if (Utils::SpecialKeyState[GLUT_KEY_SHIFT_L] == Utils::INPUT_DOWN || Utils::SpecialKeyState[GLUT_KEY_SHIFT_L] == Utils::INPUT_DOWN_FIRST)
////	{
////		glm::vec3 tempVec = myCamera->GetPosition();
////		tempVec.y -= 2.5f;
////		myCamera->SetPosition(glm::vec3(tempVec));
////
////		//The key is has now been processed for a frame so next frame if it hasn't been set to up,
////		//change the enum state
////		Utils::SpecialKeyState[GLUT_KEY_SHIFT_L] = Utils::INPUT_DOWN;
////	}
////
////	//Mouse Input
////	if (Utils::MouseState[0] == Utils::INPUT_DOWN)
////	{
////		modelMatrix = CreateModelMatrix(100.0f, 100.0f, 0.0f, 2, 2);
////	}
////	if (Utils::MouseState[2] == Utils::INPUT_DOWN)
////	{
////		modelMatrix = CreateModelMatrix(-100.0f, -100.0f, 0.0f, 2, 2);
////	}
////	if(Utils::g_mousePosDifX != 0 || Utils::g_mousePosDifY != 0)
////	{
////		myCamera->ProcessMouseInput(static_cast<float>(Utils::g_mousePosDifX), static_cast<float>(Utils::g_mousePosDifY), true);
////		Utils::g_mousePosDifX = 0;
////		Utils::g_mousePosDifY = 0;
////	}
////}

#include "Debug.h"
#include <freeglut.h>
#include <iostream>
#include <vector>
#include <complex>
//#include "vld.h"

#include "Utils.h"
#include "Shader.h"
#include "ShaderLoader.h"
#include "Mesh.h"
#include "Utils.h"

glm::mat4 modelMatrix;

Mesh* myFractalMesh = nullptr;

//Second quad
Shader* myFractalShader = nullptr;
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

	/////Input function callbacks
	////KeyBoard
	//glutKeyboardFunc(Utils::KeyBoardDown);
	//glutKeyboardUpFunc(Utils::KeyBoardUp);
	//glutSpecialFunc(Utils::SpecialKeyBoardDown);
	//glutSpecialUpFunc(Utils::SpecialKeyBoardUp);

	////Mouse
	//glutMouseFunc(Utils::MouseClick);
	//glutMotionFunc(Utils::MouseMove);
	//glutPassiveMotionFunc(Utils::MousePassiveMove);
	
	
	glutDisplayFunc(display);
	glutMainLoop();
	return 0;
}

void display()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	const int width = glutGet(GLUT_WINDOW_WIDTH);
	const int height = glutGet(GLUT_WINDOW_HEIGHT);
	glOrtho(0, width, 0, height, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	compute_mandelbrot(-1.0, 0.5, 0.625, -1.2);
	glutSwapBuffers();
}

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
				glColor3f(EaseInOut(0.0 - 0.5, 0.5, iterations, MAX_ITERATIONS), EaseInOut(0.0 - 0.5, 0.5, iterations, MAX_ITERATIONS), EaseInOut(0.0 - 0.5, 0.5, iterations, MAX_ITERATIONS));
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