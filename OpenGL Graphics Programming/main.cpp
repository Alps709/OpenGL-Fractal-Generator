#include "Debug.h"
#include <freeglut.h>
#include <iostream>
//#include "vld.h"

#include "Utils.h"
#include "Shader.h"
#include "ShaderLoader.h"
#include "Texture.h"
#include "Renderer.h"
#include "Camera.h"

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include "Mesh.h"


Camera* myCamera = nullptr;
glm::mat4 modelMatrix;

Mesh* myMesh1 = nullptr;
Mesh* myMesh2 = nullptr;
Mesh* myMesh3 = nullptr;


//First quad
Shader* myShaderTexScroll = nullptr;
Texture* myTexture = nullptr;

//Second quad
Shader* myShaderTexAnim = nullptr;
Texture* myTexture2;

//Hexagon
Shader* myShaderHex = nullptr;
Texture* myTexture3 = nullptr;
Texture* myTexture4 = nullptr;

//Projection matrix
glm::mat4 PV;
glm::mat4 PVM;

//Uniforms
GLfloat u_currentTime = 0;
int u_frameNum = 0;

//Scrolling texture
float vertices1[] =
{
	///Bottom right quad
	//Position			     //Colour			        //Texture co-ords
	760.0f,    0.0f, 0.0f,	 1.0f, 0.0f, 0.0f, 1.0f,   0.0f,   1.0f,    //Bottom left
	760.0f,  200.0f, 0.0f,	 0.0f, 1.0f, 0.0f, 1.0f,   0.0f,   0.0f,    //Top left
    960.0f,  200.0f, 0.0f,	 0.0f, 1.0f, 0.0f, 1.0f,   0.125f, 0.0f,    //Top right
    960.0f,    0.0f, 0.0f,	 0.0f, 0.0f, 1.0f, 1.0f,   0.125f, 1.0f,    //Bottom right
};											
											
GLuint indices1[] =							
{											
	//First quad							
	0, 2, 1,								
	0, 3, 2,								
};											

//Animated texture
float vertices2[] =							
{											
	///Bottom left quad						
	//Position			    //Colour				   //Texture co-ords
	  0.0f,   0.0f, 0.0f,   1.0f, 0.0f, 0.0f, 1.0f,   0.0f,  1.0f,    //Bottom left
	  0.0f, 200.0f, 0.0f,   0.0f, 1.0f, 0.0f, 1.0f,   0.0f,  0.0f,    //Top left
	760.0f, 200.0f, 0.0f,   0.0f, 1.0f, 0.0f, 1.0f,   1.0f,  0.0f,    //Top right
    760.0f,   0.0f, 0.0f,   0.0f, 0.0f, 1.0f, 1.0f,   1.0f,  1.0f,    //Bottom right
};

GLuint indices2[] =
{
	//Second quad
	0, 2, 1,
	0, 3, 2,
};

//First hexagon
float vertices3[] =
{				
	//Position			        //Colour					//Texture co-ords
	-110,   0, 0.0f,	    1.0f, 0.0f, 0.0f, 1.0f,   0.0f,  0.5f,    //Middle left
	 -60,  80, 0.0f,	    0.0f, 1.0f, 0.0f, 1.0f,   0.25f, 0.0f,    //Top left
	 -60, -80, 0.0f,	    0.0f, 1.0f, 0.0f, 1.0f,   0.25f, 1.0f,    //Bottom left
	  60,  80, 0.0f,	    0.0f, 0.0f, 1.0f, 1.0f,   0.75f, 0.0f,    //Top right
	  60, -80, 0.0f,        0.0f, 0.0f, 1.0f, 1.0f,   0.75f, 1.0f,	   //Bottom right
	 110,   0, 0.0f,	    1.0f, 0.0f, 0.0f, 1.0f,   1.0f,  0.5f,	   //Middle right
};

GLuint indices3[] =
{
	//Left Triangle							
	0, 2, 1,

	//Middle quad
	1, 2, 3,
	3, 2, 4,

	//Right triangle
	3, 4, 5,
};

void Render();
void Update();

void ProcessInput();

void SetGlobalGLSettings();

glm::mat4 CreateModelMatrix(float _translateX, float _translateY, float _rotationAngle, float _scaleX, float _scaleY);

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

	///Quad 1
	myMesh1 = new Mesh(sizeof(vertices1), vertices1, sizeof(indices1), indices1);

	myShaderTexAnim = new Shader("Shaders/TextureAnimVertexShader.shader",
                                "Shaders/TextureAnimFragmentShader.shader");

	myTexture = new Texture("img/Capguy_Walk.png", 0);


	///Quad 2
	myMesh2 = new Mesh(sizeof(vertices2), vertices2, sizeof(indices2), indices2);

	myShaderTexScroll = new Shader("Shaders/TextureScrollVertexShader.shader",
                                 "Shaders/TextureScrollFragmentShader.shader");

	myTexture2 = new Texture("img/Capguy_Walk.png", 0);


	///Hexagon
	myMesh3 = new Mesh(sizeof(vertices3), vertices3, sizeof(indices3), indices3);

	myShaderHex = new Shader("Shaders/TextureHexagonVertex.shader",
                            "Shaders/TextureHexagonFragment.shader");

	myTexture3 = new Texture("img/idk.png", 0);
	myTexture4 = new Texture("img/idk2.png", 1);


	//Create the camera
	myCamera = new Camera();

	//Create the model matrix for the quads
	modelMatrix = CreateModelMatrix(0, 0, 0, 1, 1);

	//Update and render function callbacks
	glutDisplayFunc(Render);
	glutIdleFunc(Update);

	///Input function callbacks
	//KeyBoard
	glutKeyboardFunc(Utils::KeyBoardDown);
	glutKeyboardUpFunc(Utils::KeyBoardUp);
	glutSpecialFunc(Utils::SpecialKeyBoardDown);
	glutSpecialUpFunc(Utils::SpecialKeyBoardUp);

	//Mouse
	glutMouseFunc(Utils::MouseClick);
	glutMotionFunc(Utils::MouseMove);
	glutPassiveMotionFunc(Utils::MousePassiveMove);
	

	glutMainLoop();

	//Delete all the heap allocated objects
	delete myCamera;
	delete myShaderTexScroll;
	delete myShaderTexAnim;
	delete myShaderHex;
	delete myTexture;
	delete myTexture2;
	delete myTexture3;
	delete myTexture4;
	delete myMesh1;
	delete myMesh2;
	delete myMesh3;
	return 0;
}

void Render()
{
	//Clear the screen before every frame
	Renderer::Clear();

	PV = myCamera->GetProjection() * myCamera->GetView();
	PVM = PV * modelMatrix;

	{///Animated texture
		//Bind the shader program and pass in uniforms
		myShaderTexAnim->Bind();
		myShaderTexAnim->SetUniform1i("u_frameNum", u_frameNum);
		myShaderTexAnim->SetUniformMat4f("u_PVM", PVM);

		//Bind Texture and set its uniform
		myTexture->Bind();
		myShaderTexAnim->SetUniform1i("tex1", 0);

		//Draw call
		myMesh1->Bind();
		glDrawElements(GL_TRIANGLES, myMesh1->GetindicesCount(), GL_UNSIGNED_INT, 0);
		Mesh::Unbind();
	}

	{///Scrolling texture
		//Set program and pass in uniforms
		myShaderTexScroll->Bind();
		myShaderTexScroll->SetUniform1f("u_currentTime", u_currentTime);
		myShaderTexScroll->SetUniform1i("u_frameNum", u_frameNum);
		myShaderTexScroll->SetUniformMat4f("u_PVM", PVM);

		//Bind Texture
		myTexture2->Bind();
		myShaderTexScroll->SetUniform1i("tex2", 0);

		myMesh2->Bind();
		glDrawElements(GL_TRIANGLES, myMesh2->GetindicesCount(), GL_UNSIGNED_INT, 0);
		Mesh::Unbind();
	}

	{///Hex 1
		//Create new model matrix
		const glm::mat4 modelMatrix2 = CreateModelMatrix(275.0f, 390.0f, 0, 1.0f, 1.0f);

		glm::mat4 PVM2 = PV * modelMatrix2;

		//Bind the shader program and pass in uniforms
		myShaderHex->Bind();
		myShaderHex->SetUniform1f("u_currentTime", u_currentTime);
		myShaderHex->SetUniformMat4f("u_PVM", PVM2);

		//Bind Texture and set its uniform
		myTexture3->Bind();
		myShaderHex->SetUniform1i("tex3", 0);

		myTexture4->Bind();
		myShaderHex->SetUniform1i("tex4", 1);

		//Draw call
		myMesh3->Bind();
		glDrawElements(GL_TRIANGLES, myMesh3->GetindicesCount(), GL_UNSIGNED_INT, 0);
		Mesh::Unbind();
	}

	{///Hex 2 (rotating)
		//Create the new model matrix for Hex2
		const float rotationAngle2 = static_cast<float>(static_cast<int>(u_currentTime * 50) % 360);
		const glm::mat4 modelMatrix3 = CreateModelMatrix(700.0f, 390.0f, rotationAngle2, 1.0f, 1.0f);

		glm::mat4 PVM3 = PV * modelMatrix3;

		//Bind the shader program and pass in uniforms
		myShaderHex->Bind();
		myShaderHex->SetUniform1f("u_currentTime", u_currentTime);
		myShaderHex->SetUniformMat4f("u_PVM", PVM3);

		//Bind Texture and set its uniform
		myTexture3->Bind();
		myShaderHex->SetUniform1i("tex3", 0);

		myTexture4->Bind();
		myShaderHex->SetUniform1i("tex4", 1);

		//Draw call
		myMesh3->Bind();
		glDrawElements(GL_TRIANGLES, myMesh3->GetindicesCount(), GL_UNSIGNED_INT, 0);
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
	ProcessInput();
}

void ProcessInput()
{
	//Keyboard input
	//Move the camera forward with the w button
	if (
		Utils::KeyState['w'] == Utils::INPUT_DOWN || Utils::KeyState['w'] == Utils::INPUT_DOWN_FIRST ||
		Utils::KeyState['W'] == Utils::INPUT_DOWN || Utils::KeyState['W'] == Utils::INPUT_DOWN_FIRST
		)
	{
		//move camera forward
		myCamera->SetPosition(myCamera->GetPosition() + myCamera->GetLookDirection() * myCamera->GetCamSpeed());

		//Print camera position for debugging
		//std::cout << "Camera pos: x: " << myCamera->GetPosition().x << " y: " << myCamera->GetPosition().y << " z: " << myCamera->GetPosition().z  << std::endl;

		//The key is has now been processed for a frame so next frame if it hasn't been set to up,
		//change the enum state
		Utils::KeyState['w'] = Utils::INPUT_DOWN;
		Utils::KeyState['W'] = Utils::INPUT_DOWN;
	}
	//Move the camera to the left with the a button
	if (
		Utils::KeyState['a'] == Utils::INPUT_DOWN || Utils::KeyState['a'] == Utils::INPUT_DOWN_FIRST ||
		Utils::KeyState['A'] == Utils::INPUT_DOWN || Utils::KeyState['A'] == Utils::INPUT_DOWN_FIRST
		)
	{
		//move camera left
		myCamera->SetPosition(myCamera->GetPosition() - myCamera->GetCamRight() * myCamera->GetCamSpeed());

		//The key is has now been processed for a frame so next frame if it hasn't been set to up,
		//change the enum state
		Utils::KeyState['a'] = Utils::INPUT_DOWN;
		Utils::KeyState['A'] = Utils::INPUT_DOWN;
	}
	//Move the camera backwards with the s button
	if (
		Utils::KeyState['s'] == Utils::INPUT_DOWN || Utils::KeyState['s'] == Utils::INPUT_DOWN_FIRST ||
		Utils::KeyState['S'] == Utils::INPUT_DOWN || Utils::KeyState['S'] == Utils::INPUT_DOWN_FIRST
		)
	{
		//move camera backward
		myCamera->SetPosition(myCamera->GetPosition() - myCamera->GetLookDirection() * myCamera->GetCamSpeed());

		//The key is has now been processed for a frame so next frame if it hasn't been set to up,
		//change the enum state
		Utils::KeyState['s'] = Utils::INPUT_DOWN;
		Utils::KeyState['S'] = Utils::INPUT_DOWN;
	}
	//Move the camera to the right witht the d button
	if (
		Utils::KeyState['d'] == Utils::INPUT_DOWN || Utils::KeyState['d'] == Utils::INPUT_DOWN_FIRST ||
		Utils::KeyState['D'] == Utils::INPUT_DOWN || Utils::KeyState['D'] == Utils::INPUT_DOWN_FIRST
		)
	{
		//move camera right
		myCamera->SetPosition(myCamera->GetPosition() + myCamera->GetCamRight() * myCamera->GetCamSpeed());

		//The key is has now been processed for a frame so next frame if it hasn't been set to up,
		//change the enum state
		Utils::KeyState['d'] = Utils::INPUT_DOWN;
		Utils::KeyState['D'] = Utils::INPUT_DOWN;
	}
	//Move the camera up with the space button
	if (Utils::KeyState[32] == Utils::INPUT_DOWN || Utils::KeyState[32] == Utils::INPUT_DOWN_FIRST)
	{
		glm::vec3 tempVec = myCamera->GetPosition();
		tempVec.y += 2.5f;
		myCamera->SetPosition(glm::vec3(tempVec));

		//The key is has now been processed for a frame so next frame if it hasn't been set to up,
		//change the enum state
		Utils::KeyState[32] = Utils::INPUT_DOWN;
	}
	//Move the camera down with the left shift button
	if (Utils::SpecialKeyState[GLUT_KEY_SHIFT_L] == Utils::INPUT_DOWN || Utils::SpecialKeyState[GLUT_KEY_SHIFT_L] == Utils::INPUT_DOWN_FIRST)
	{
		glm::vec3 tempVec = myCamera->GetPosition();
		tempVec.y -= 2.5f;
		myCamera->SetPosition(glm::vec3(tempVec));

		//The key is has now been processed for a frame so next frame if it hasn't been set to up,
		//change the enum state
		Utils::SpecialKeyState[GLUT_KEY_SHIFT_L] = Utils::INPUT_DOWN;
	}

	//Mouse Input
	if (Utils::MouseState[0] == Utils::INPUT_DOWN)
	{
		modelMatrix = CreateModelMatrix(100.0f, 100.0f, 0.0f, 2, 2);
	}
	if (Utils::MouseState[2] == Utils::INPUT_DOWN)
	{
		modelMatrix = CreateModelMatrix(-100.0f, -100.0f, 0.0f, 2, 2);
	}
	if(Utils::g_mousePosDifX != 0 || Utils::g_mousePosDifY != 0)
	{
		myCamera->ProcessMouseInput(static_cast<float>(Utils::g_mousePosDifX), static_cast<float>(Utils::g_mousePosDifY), true);
		Utils::g_mousePosDifX = 0;
		Utils::g_mousePosDifY = 0;
	}
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

	//Setup alpha blending
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	GLCall(glEnable(GL_BLEND));

	//Allows the program to continue executing after the glutMainLoop()
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);

	//Sets the clear colour when calling glClear()
	GLCall(glClearColor(0.0f, 0.0f, 1.0f, 1.0f));
}


glm::mat4 CreateModelMatrix(float _translateX, float _translateY, float _rotationAngle, float _scaleX, float _scaleY)
{
	//Translationmatrix
	const glm::vec3 objPosition = glm::vec3(_translateX, _translateY, 0.0f);
	const glm::mat4 translationMatrix = glm::translate(glm::mat4(), objPosition);

	//Rotation matrix
	const glm::vec3 rotationAxisZ = glm::vec3(0.0f, 0.0f, 1.0f);
	const glm::mat4 rotationMatrixZ = glm::rotate(glm::mat4(), glm::radians(_rotationAngle), rotationAxisZ);

	//Scale matrix
	const glm::vec3 objScale = glm::vec3(_scaleX, _scaleY, 1.0f);
	const glm::mat4 scaleMatrix = glm::scale(glm::mat4(), objScale);

	//Return model matrix
	return translationMatrix * rotationMatrixZ * scaleMatrix;
}