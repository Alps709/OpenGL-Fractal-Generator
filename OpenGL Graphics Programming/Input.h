#pragma once
#include "Debug.h"
#include <freeglut.h>

//Callback functions for glut input
namespace
{
	InputManager& inputManager = inputManager.getInstance();

	enum InputState
	{
		INPUT_NULL = 0, //Default key state (can be used to tell is a key has ever been pressed)
		INPUT_UP = 1,
		INPUT_DOWN = 2,
		INPUT_UP_FIRST = 3,
		INPUT_DOWN_FIRST = 4,
	};

	//Declared inputs to be handled that have an uppercase and lowercase version
	unsigned char inputs[10] = { 'w', 'a', 's', 'd', 'p', 'W', 'A', 'S', 'D', 'P' };

	void MouseClick(int _button, int _state, int _x, int _y)
	{
		//Move co-ords from (0, 0) at top left, to (0, 0) at middle of screen
		_x -= (int)(inputManager.HSCREEN_WIDTH);
		_y -= (int)(inputManager.HSCREEN_HEIGHT);

		//Invert y axis
		_y *= -1;

		//We are not using any buttons higher than 2, so don't bother doing anything
		if (_button > 2)
		{
			return;
		}

		//Get the x and y difference since last frame
		inputManager.g_mousePosDifX = _x - inputManager.g_mousePosX;
		inputManager.g_mousePosDifY = _y - inputManager.g_mousePosY;


		if (!inputManager.CAMERA_FREEEVIEW_MODE)
		{
			inputManager.g_mousePosX = _x;

			inputManager.g_mousePosY = _y;
		}
		else
		{
			glutWarpPointer((int)inputManager.HSCREEN_WIDTH, (int)inputManager.HSCREEN_HEIGHT);
			inputManager.g_mousePosX = 0;
			inputManager.g_mousePosY = 0;
		}

		inputManager.MouseState[_button] = (GLUT_DOWN == _state) ? inputManager.INPUT_DOWN_FIRST : inputManager.INPUT_UP_FIRST;
		//Debug logging
		//std::cout << "Mouse clicked on - x: " << _x << " | y: " << _y << std::endl;
	}

	void MousePassiveMove(int _x, int _y)
	{
		if (!inputManager.CAMERA_FREEEVIEW_MODE)
		{
			//Convert current screen width and height mouse co-ords to 
			//Move co-ords from (0, 0) at top left, to (0, 0) at middle of screen
			//(remaps screen size mouse coords to opengl pixel coords)
			_x = (int)(Math::remap(_x, -inputManager.TRUE_HSCREEN_WIDTH, inputManager.TRUE_HSCREEN_WIDTH, -inputManager.HSCREEN_WIDTH, inputManager.HSCREEN_WIDTH) - inputManager.HSCREEN_WIDTH);
			_y = (int)(Math::remap(_y, -inputManager.TRUE_HSCREEN_HEIGHT, inputManager.TRUE_HSCREEN_HEIGHT, -inputManager.HSCREEN_HEIGHT, inputManager.HSCREEN_HEIGHT) - inputManager.HSCREEN_HEIGHT);

			//Invert y axis
			_y *= -1;

			inputManager.g_mousePosX = _x;
			inputManager.g_mousePosY = _y;
		}
		else
		{
			//Move co-ords from (0, 0) at top left, to (0, 0) at middle of screen
			_x -= (int)(inputManager.HSCREEN_WIDTH);
			_y -= (int)(inputManager.HSCREEN_HEIGHT);

			//Invert y axis
			_y *= -1;

			inputManager.g_mousePosDifX = _x - inputManager.g_mousePosX;
			inputManager.g_mousePosDifY = _y - inputManager.g_mousePosY;

			glutWarpPointer((int)inputManager.HSCREEN_WIDTH, (int)inputManager.HSCREEN_HEIGHT);
			inputManager.g_mousePosX = 0;
			inputManager.g_mousePosY = 0;
		}
		////Debug logging
		//std::cout << "Mouse moved to - x: " << _x << " | y: " << _y << std::endl;
		//std::cout << "Mouse change in - x: " << inputManager.g_mousePosDifX << " | y: " << inputManager.g_mousePosDifY << std::endl;
	}

	void MouseMove(int _x, int _y)
	{
		if (!inputManager.CAMERA_FREEEVIEW_MODE)
		{
			//Convert current screen width and height mouse co-ords to 
			//Move co-ords from (0, 0) at top left, to (0, 0) at middle of screen
			//(remaps screen size mouse coords to opengl pixel coords)
			_x = (int)(Math::remap(_x, -inputManager.TRUE_HSCREEN_WIDTH, inputManager.TRUE_HSCREEN_WIDTH, -inputManager.HSCREEN_WIDTH, inputManager.HSCREEN_WIDTH) - inputManager.HSCREEN_WIDTH);
			_y = (int)(Math::remap(_y, -inputManager.TRUE_HSCREEN_HEIGHT, inputManager.TRUE_HSCREEN_HEIGHT, -inputManager.HSCREEN_HEIGHT, inputManager.HSCREEN_HEIGHT) - inputManager.HSCREEN_HEIGHT);

			//Invert y axis
			_y *= -1;

			inputManager.g_mousePosX = _x;
			inputManager.g_mousePosY = _y;
		}
		else
		{
			//Move co-ords from (0, 0) at top left, to (0, 0) at middle of screen
			_x -= (int)(inputManager.HSCREEN_WIDTH);
			_y -= (int)(inputManager.HSCREEN_HEIGHT);

			//Invert y axis
			_y *= -1;

			inputManager.g_mousePosDifX = _x - inputManager.g_mousePosX;
			inputManager.g_mousePosDifY = _y - inputManager.g_mousePosY;

			glutWarpPointer((int)inputManager.HSCREEN_WIDTH, (int)inputManager.HSCREEN_HEIGHT);
			inputManager.g_mousePosX = 0;
			inputManager.g_mousePosY = 0;
		}
		//Debug logging
		//std::cout << "Mouse clicked on - x: " << _x << " | y: " << _y << std::endl;
		//std::cout << "Mouse change in - x: " << g_mousePosDifX << " | y: " << g_mousePosDifY << std::endl;
	}

	void KeyBoardUp(unsigned char _key, int _x, int _y)
	{
		const int tempState = inputManager.KeyState[_key];

		//Find offset of the corresponding uppercase or lowercase key
		const int offset = (_key < 97) ? 32 : -32;

		//If the key is down
		if (tempState != inputManager.INPUT_UP && tempState != inputManager.INPUT_UP_FIRST)
		{
			//Loop through the currently handled input keys
			for (unsigned char input : inputs)
			{
				//If the key is declared to be handled
				if (_key == input)
				{
					//Set the the corresponding uppercase or lowercase key aswell
					inputManager.KeyState[_key + offset] = inputManager.INPUT_UP_FIRST;
					break;
				}
			}
			//Continue to change keystate as normal
			inputManager.KeyState[_key] = inputManager.INPUT_UP_FIRST;
		}
	}

	void KeyBoardDown(unsigned char _key, int _x, int _y)
	{
		const int tempState = inputManager.KeyState[_key];
		const int offset = (_key < 97) ? 32 : -32;

		//Key is up
		if (tempState != inputManager.INPUT_DOWN && tempState != inputManager.INPUT_DOWN_FIRST)
		{
			for (unsigned char input : inputs)
			{
				if (_key == input)
				{
					inputManager.KeyState[_key + offset] = inputManager.INPUT_DOWN_FIRST;
					break;
				}
			}
			inputManager.KeyState[_key] = inputManager.INPUT_DOWN_FIRST;
		}
	}

	//Special key up function
	void SpecialKeyBoardUp(int _key, int _x, int _y)
	{
		const int tempState = inputManager.SpecialKeyState[_key];

		if (tempState != inputManager.INPUT_UP && tempState != inputManager.INPUT_UP_FIRST)
		{
			inputManager.SpecialKeyState[_key] = inputManager.INPUT_UP_FIRST;
		}
	}

	//Special key down function
	void SpecialKeyBoardDown(int _key, int _x, int _y)
	{
		const int tempState = inputManager.SpecialKeyState[_key];

		if (tempState != inputManager.INPUT_DOWN && tempState != inputManager.INPUT_DOWN_FIRST)
		{
			inputManager.SpecialKeyState[_key] = inputManager.INPUT_DOWN_FIRST;
		}
	}

	void WindowResize(int _x, int _y)
	{
		inputManager.TRUE_SCREEN_WIDTH = (float)_x;
		inputManager.TRUE_SCREEN_HEIGHT = (float)_y;
		inputManager.TRUE_HSCREEN_HEIGHT = inputManager.TRUE_SCREEN_HEIGHT / 2;
		inputManager.TRUE_HSCREEN_WIDTH = inputManager.TRUE_SCREEN_WIDTH / 2;

		glViewport((GLsizei)0, (GLsizei)0, (GLsizei)inputManager.TRUE_SCREEN_WIDTH, (GLsizei)inputManager.TRUE_SCREEN_HEIGHT);
	}
}
