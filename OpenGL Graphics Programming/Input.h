#pragma once
#include "Utils.h"
#include <freeglut.h>

namespace Input
{
	static int hSCREEN_WIDTH = Utils::SCREEN_WIDTH / 2;
	static int hSCREEN_HEIGHT = Utils::SCREEN_HEIGHT / 2;

	enum InputState
	{
		INPUT_NULL = 0,
		INPUT_UP = 1,
		INPUT_DOWN = 2,
		INPUT_UP_FIRST = 3,
		INPUT_DOWN_FIRST = 4,
	};

	//Keyboard input
	InputState KeyState[255];
	InputState SpecialKeyState[255];

	//Declared inputs to be handled, that have an uppercase and lowercase version
	const int inputNum = 10;
	unsigned char inputs[inputNum] = { 'w', 'a', 's', 'd', 'p', 'W', 'A', 'S', 'D', 'P' };

	void KeyBoardUp(unsigned char _key, int _x, int _y)
	{
		const InputState tempState = KeyState[_key];

		//Find offset of the corresponding uppercase or lowercase key
		const int offset = (_key < 97) ? 32 : -32;

		//If the key is down
		if (tempState != INPUT_UP && tempState != INPUT_UP_FIRST)
		{
			//Loop through the currently handled input keys
			for (unsigned char input : inputs)
			{
				//If the key is declared to be handled
				if (_key == input)
				{
					//Set the the corresponding uppercase or lowercase key aswell
					KeyState[_key + offset] = INPUT_UP_FIRST;
					break;
				}
			}
			//Continue to change keystate as normal
			KeyState[_key] = INPUT_UP_FIRST;
		}
	}

	void KeyBoardDown(unsigned char _key, int _x, int _y)
	{
		const InputState tempState = KeyState[_key];
		const int offset = (_key < 97) ? 32 : -32;

		if (tempState != INPUT_DOWN && tempState != INPUT_DOWN_FIRST)
		{
			for (unsigned char input : inputs)
			{
				if (_key == input)
				{
					KeyState[_key + offset] = INPUT_DOWN_FIRST;
					break;
				}
			}
			KeyState[_key] = INPUT_DOWN_FIRST;
		}
	}

	//Special key up function
	void SpecialKeyBoardUp(int _key, int _x, int _y)
	{
		const InputState tempState = SpecialKeyState[_key];
		if (tempState != INPUT_UP && tempState != INPUT_UP_FIRST)
		{
			SpecialKeyState[_key] = INPUT_UP_FIRST;
		}
	}

	//Special key down function
	void SpecialKeyBoardDown(int _key, int _x, int _y)
	{
		const InputState tempState = SpecialKeyState[_key];

		if (tempState != INPUT_DOWN && tempState != INPUT_DOWN_FIRST)
		{
			SpecialKeyState[_key] = INPUT_DOWN_FIRST;
		}
	}
}