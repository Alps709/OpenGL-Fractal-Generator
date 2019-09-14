#pragma once

namespace Utils
{
	bool static useOptimised = false;
	unsigned static int g_maxIterations = 500;
	const static int g_SCREEN_WIDTH = 1000;
	const static int g_SCREEN_HEIGHT = 1000;

	//enum InputState
	//{
	//	INPUT_UP = 1,
	//	INPUT_DOWN = 2,
	//	INPUT_UP_FIRST = 3,
	//	INPUT_DOWN_FIRST = 4,
	//};

	////Mouse Input
	//InputState MouseState[3];
	//static int g_mousePosX = 0;
	//static int g_mousePosY = 0;
	//static int g_mousePosDifX = 0;
	//static int g_mousePosDifY = 0;

	//void MouseClick(int _button, int _state, int _x, int _y)
	//{
	//	if (_button >= 3)
	//		return;
	//	g_mousePosDifX = _x - g_mousePosX;
	//	g_mousePosDifY = _y - g_mousePosY;
	//	g_mousePosX = _x;
	//	g_mousePosY = _y;
	//	
	//	MouseState[_button] = (GLUT_DOWN == _state) ? INPUT_DOWN : INPUT_UP;
	//	//std::cout << "Mouse clicked on - x: " << _x << " | y: " << _y << std::endl;
	//}

	//void MousePassiveMove(int _x, int _y)
	//{
	//	g_mousePosDifX = _x - g_mousePosX;
	//	g_mousePosDifY = _y - g_mousePosY;
	//	g_mousePosX = _x;
	//	g_mousePosY = _y;
	//	std::cout << "Mouse moved to - x: " << _x << " | y: " << _y << std::endl;
	//	std::cout << "Mouse change in - x: " << g_mousePosDifX << " | y: " << g_mousePosDifY << std::endl;
	//}

	//void MouseMove(int _x, int _y)
	//{
	//	g_mousePosDifX = _x - g_mousePosX;
	//	g_mousePosDifY = _y - g_mousePosY;
	//	g_mousePosX = _x;
	//	g_mousePosY = _y;
	//	//std::cout << "Mouse clicked on - x: " << _x << " | y: " << _y << std::endl;
	//	//std::cout << "Mouse change in - x: " << g_mousePosDifX << " | y: " << g_mousePosDifY << std::endl;
	//}
	//

	////Keyboard input
	//InputState KeyState[255];
	//InputState SpecialKeyState[255];
	//const int inputNum = 8;
	//unsigned char inputs[inputNum] = { 'w', 'a', 's', 'd', 'W', 'A', 'S', 'D' };

	//void KeyBoardUp(unsigned char _key, int _x, int _y)
	//{
	//	InputState tempState = KeyState[_key];
	//	int offset = (_key < 97) ? 32 : -32;

	//	if(tempState != INPUT_UP && tempState != INPUT_UP_FIRST)
	//	{
	//		for (int i = 0; i < inputNum; ++i)
	//		{
	//			if (_key == inputs[i])
	//			{
	//				KeyState[_key + offset] = INPUT_UP_FIRST;
	//				break;
	//			}
	//		}
	//		KeyState[_key] = INPUT_UP_FIRST;
	//	}
	//}

	//void KeyBoardDown(unsigned char _key, int _x, int _y)
	//{
	//	InputState tempState = KeyState[_key];
	//	int offset = (_key < 97) ? 32 : -32;

	//	if (tempState != INPUT_DOWN && tempState != INPUT_DOWN_FIRST)
	//	{
	//		for (int i = 0; i < inputNum; ++i)
	//		{
	//			if (_key == inputs[i])
	//			{
	//				KeyState[_key + offset] = INPUT_DOWN_FIRST;
	//				break;
	//			}
	//		}
	//		KeyState[_key] = INPUT_DOWN_FIRST;
	//	}
	//}

	////Special key up function
	//void SpecialKeyBoardUp(int _key, int _x, int _y)
	//{
	//	InputState tempState = SpecialKeyState[_key];
	//	if (tempState != INPUT_UP && tempState != INPUT_UP_FIRST)
	//	{
	//		SpecialKeyState[_key] = INPUT_UP_FIRST;
	//	}
	//}
	//
	////Special key down function
	//void SpecialKeyBoardDown(int _key, int _x, int _y)
	//{
	//	InputState tempState = SpecialKeyState[_key];

	//	if (tempState != INPUT_DOWN && tempState != INPUT_DOWN_FIRST)
	//	{
	//		SpecialKeyState[_key] = INPUT_DOWN_FIRST;
	//	}
	//}
}