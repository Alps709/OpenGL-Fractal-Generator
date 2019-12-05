#pragma once
#include "Math.h"

class InputManager
{

public:
	static InputManager& getInstance()
	{
		static InputManager instance; // Guaranteed to be destroyed.
		return instance;
	}
	
	void Update();

	enum InputState
	{
		INPUT_NULL = 0,
		INPUT_UP = 1,
		INPUT_DOWN = 2,
		INPUT_UP_FIRST = 3,
		INPUT_DOWN_FIRST = 4,
	};

	int g_mousePosX = 0;
	int g_mousePosY = 0;
	int g_mousePosDifX = 0;
	int g_mousePosDifY = 0;

	bool CAMERA_FREEEVIEW_MODE = false;

	//Top down world space size
	float HWORLD_WIDTH = 4200.0f;
	//Height in this scenario is the Z axis, as the camera is looking from top down
	//Moving to the right in this top down view is also moving in the negative Z direction
	float HWORLD_HEIGHT = -2340.0f; 

	//Game window size
	float SCREEN_WIDTH = 1280.0f;
	float SCREEN_HEIGHT = 720.0f;
	float HSCREEN_WIDTH = SCREEN_WIDTH / 2.0f;
	float HSCREEN_HEIGHT = SCREEN_HEIGHT / 2.0f;

	//True window size (set in the windowResize function callback)
	float TRUE_SCREEN_WIDTH = SCREEN_WIDTH;
	float TRUE_SCREEN_HEIGHT = SCREEN_HEIGHT;
	float TRUE_HSCREEN_WIDTH = HSCREEN_WIDTH;
	float TRUE_HSCREEN_HEIGHT = HSCREEN_HEIGHT;

	//Mouse Input
	InputState MouseState[3];

	//Keyboard input
	InputState KeyState[255];
	InputState SpecialKeyState[255];
private:
	InputManager() {}                  

public:
	InputManager(InputManager const&) = delete;
	void operator=(InputManager const&) = delete;
};

