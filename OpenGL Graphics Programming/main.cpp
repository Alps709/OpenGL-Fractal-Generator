#include "Debug.h"
#include <freeglut.h>
#include <iostream>
#include <vector>
//#include "vld.h"

#include "Utils.h"
#include "Shader.h"
#include "ShaderLoader.h"
#include "Mesh.h"

glm::mat4 modelMatrix;

Mesh* myFractalMesh = nullptr;

//Second quad
Shader* myFractalShader = nullptr;

//Projection matrix
glm::mat4 PV;
glm::mat4 PVM;

//Uniforms
GLfloat u_currentTime = 0;
int u_frameNum = 0;

//Animated texture
std::vector<float> fractalPoints;

void Render();
void Update();

//void ProcessInput();

void SetGlobalGLSettings();

int main(int argc, char ** argv)
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

	//Calcumate mandlebrot fractal here

	//After calculating all the pixel colours add it to a mesh
	//myFractalMesh = new Mesh(sizeof(vertices1), vertices1, sizeof(indices1), indices1);

	myFractalShader = new Shader("Shaders/TextureAnimVertexShader.shader",
                                "Shaders/TextureAnimFragmentShader.shader");

	//Update and render function callbacks
	glutDisplayFunc(Render);
	glutIdleFunc(Update);

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
	

	glutMainLoop();

	//Delete all the heap allocated objects
	delete myFractalShader;
	delete myFractalMesh;
	return 0;
}

void Render()
{
	//Clear the screen before every frame
	glClear(GL_COLOR_BUFFER_BIT);

	{///Mandlebrot factal
		//Bind the shader program and pass in uniforms
		myFractalShader->Bind();

		//Draw call
		myFractalMesh->Bind();
		glDrawElements(GL_TRIANGLES, myFractalMesh->GetindicesCount(), GL_UNSIGNED_INT, 0);
		Mesh::Unbind();
	}

	glutSwapBuffers();
	u_frameNum++;
}

void Update()
{
	//Update game information
	u_currentTime = static_cast<float>(glutGet(GLUT_ELAPSED_TIME)); //Gets current time
	u_currentTime = u_currentTime * 0.001f; //Convert milliseconds to seconds
	glutPostRedisplay();
	//ProcessInput();
}

//void ProcessInput()
//{
//	//Keyboard input
//	//Move the camera forward with the w button
//	if (
//		Utils::KeyState['w'] == Utils::INPUT_DOWN || Utils::KeyState['w'] == Utils::INPUT_DOWN_FIRST ||
//		Utils::KeyState['W'] == Utils::INPUT_DOWN || Utils::KeyState['W'] == Utils::INPUT_DOWN_FIRST
//		)
//	{
//		//move camera forward
//		myCamera->SetPosition(myCamera->GetPosition() + myCamera->GetLookDirection() * myCamera->GetCamSpeed());
//
//		//Print camera position for debugging
//		//std::cout << "Camera pos: x: " << myCamera->GetPosition().x << " y: " << myCamera->GetPosition().y << " z: " << myCamera->GetPosition().z  << std::endl;
//
//		//The key is has now been processed for a frame so next frame if it hasn't been set to up,
//		//change the enum state
//		Utils::KeyState['w'] = Utils::INPUT_DOWN;
//		Utils::KeyState['W'] = Utils::INPUT_DOWN;
//	}
//	//Move the camera to the left with the a button
//	if (
//		Utils::KeyState['a'] == Utils::INPUT_DOWN || Utils::KeyState['a'] == Utils::INPUT_DOWN_FIRST ||
//		Utils::KeyState['A'] == Utils::INPUT_DOWN || Utils::KeyState['A'] == Utils::INPUT_DOWN_FIRST
//		)
//	{
//		//move camera left
//		myCamera->SetPosition(myCamera->GetPosition() - myCamera->GetCamRight() * myCamera->GetCamSpeed());
//
//		//The key is has now been processed for a frame so next frame if it hasn't been set to up,
//		//change the enum state
//		Utils::KeyState['a'] = Utils::INPUT_DOWN;
//		Utils::KeyState['A'] = Utils::INPUT_DOWN;
//	}
//	//Move the camera backwards with the s button
//	if (
//		Utils::KeyState['s'] == Utils::INPUT_DOWN || Utils::KeyState['s'] == Utils::INPUT_DOWN_FIRST ||
//		Utils::KeyState['S'] == Utils::INPUT_DOWN || Utils::KeyState['S'] == Utils::INPUT_DOWN_FIRST
//		)
//	{
//		//move camera backward
//		myCamera->SetPosition(myCamera->GetPosition() - myCamera->GetLookDirection() * myCamera->GetCamSpeed());
//
//		//The key is has now been processed for a frame so next frame if it hasn't been set to up,
//		//change the enum state
//		Utils::KeyState['s'] = Utils::INPUT_DOWN;
//		Utils::KeyState['S'] = Utils::INPUT_DOWN;
//	}
//	//Move the camera to the right witht the d button
//	if (
//		Utils::KeyState['d'] == Utils::INPUT_DOWN || Utils::KeyState['d'] == Utils::INPUT_DOWN_FIRST ||
//		Utils::KeyState['D'] == Utils::INPUT_DOWN || Utils::KeyState['D'] == Utils::INPUT_DOWN_FIRST
//		)
//	{
//		//move camera right
//		myCamera->SetPosition(myCamera->GetPosition() + myCamera->GetCamRight() * myCamera->GetCamSpeed());
//
//		//The key is has now been processed for a frame so next frame if it hasn't been set to up,
//		//change the enum state
//		Utils::KeyState['d'] = Utils::INPUT_DOWN;
//		Utils::KeyState['D'] = Utils::INPUT_DOWN;
//	}
//	//Move the camera up with the space button
//	if (Utils::KeyState[32] == Utils::INPUT_DOWN || Utils::KeyState[32] == Utils::INPUT_DOWN_FIRST)
//	{
//		glm::vec3 tempVec = myCamera->GetPosition();
//		tempVec.y += 2.5f;
//		myCamera->SetPosition(glm::vec3(tempVec));
//
//		//The key is has now been processed for a frame so next frame if it hasn't been set to up,
//		//change the enum state
//		Utils::KeyState[32] = Utils::INPUT_DOWN;
//	}
//	//Move the camera down with the left shift button
//	if (Utils::SpecialKeyState[GLUT_KEY_SHIFT_L] == Utils::INPUT_DOWN || Utils::SpecialKeyState[GLUT_KEY_SHIFT_L] == Utils::INPUT_DOWN_FIRST)
//	{
//		glm::vec3 tempVec = myCamera->GetPosition();
//		tempVec.y -= 2.5f;
//		myCamera->SetPosition(glm::vec3(tempVec));
//
//		//The key is has now been processed for a frame so next frame if it hasn't been set to up,
//		//change the enum state
//		Utils::SpecialKeyState[GLUT_KEY_SHIFT_L] = Utils::INPUT_DOWN;
//	}
//
//	//Mouse Input
//	if (Utils::MouseState[0] == Utils::INPUT_DOWN)
//	{
//		modelMatrix = CreateModelMatrix(100.0f, 100.0f, 0.0f, 2, 2);
//	}
//	if (Utils::MouseState[2] == Utils::INPUT_DOWN)
//	{
//		modelMatrix = CreateModelMatrix(-100.0f, -100.0f, 0.0f, 2, 2);
//	}
//	if(Utils::g_mousePosDifX != 0 || Utils::g_mousePosDifY != 0)
//	{
//		myCamera->ProcessMouseInput(static_cast<float>(Utils::g_mousePosDifX), static_cast<float>(Utils::g_mousePosDifY), true);
//		Utils::g_mousePosDifX = 0;
//		Utils::g_mousePosDifY = 0;
//	}
//}

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