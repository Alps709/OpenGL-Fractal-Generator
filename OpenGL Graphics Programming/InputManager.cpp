#include "InputManager.h"

#define GLUT_KEY_SHIFT_L 0x0070

void InputManager::Update()
{

	if (KeyState['p'] == INPUT_DOWN_FIRST ||
		KeyState['P'] == INPUT_DOWN_FIRST)
	{
		//The key is has now been processed for a frame, so set it to the appropriate state
		KeyState['p'] = INPUT_DOWN;
		KeyState['P'] = INPUT_DOWN;
	}


	if (KeyState['w'] ==  INPUT_DOWN_FIRST ||
		KeyState['W'] ==  INPUT_DOWN_FIRST)
	{
		//The key is has now been processed for a frame, so set it to the appropriate state
		 KeyState['w'] =  INPUT_DOWN;
		 KeyState['W'] =  INPUT_DOWN;
	}

	if (KeyState['a'] ==  INPUT_DOWN_FIRST ||
		KeyState['A'] ==  INPUT_DOWN_FIRST)
	{
		//The key is has now been processed for a frame, so set it to the appropriate state
		 KeyState['a'] =  INPUT_DOWN;
		 KeyState['A'] =  INPUT_DOWN;
	}

	if (KeyState['s'] ==  INPUT_DOWN_FIRST ||
		KeyState['S'] ==  INPUT_DOWN_FIRST)
	{
		//The key is has now been processed for a frame, so set it to the appropriate state
		 KeyState['s'] =  INPUT_DOWN;
		 KeyState['S'] =  INPUT_DOWN;
	}

	if (KeyState['d'] ==  INPUT_DOWN_FIRST ||
		KeyState['D'] ==  INPUT_DOWN_FIRST)
	{
		//The key is has now been processed for a frame, so set it to the appropriate state
		 KeyState['d'] =  INPUT_DOWN;
		 KeyState['D'] =  INPUT_DOWN;
	}

	//Enter key
	if (KeyState[13] == INPUT_DOWN_FIRST)
	{
		//The key is has now been processed for a frame, so set it to the appropriate state
		 KeyState[13] =  INPUT_DOWN;
	}

	//Space key
	if (KeyState[32] == INPUT_DOWN_FIRST)
	{
		//The key is has now been processed for a frame, so set it to the appropriate state
		 KeyState[32] =  INPUT_DOWN;
	}
	//Left shift key
	if (SpecialKeyState[GLUT_KEY_SHIFT_L] ==  INPUT_DOWN_FIRST)
	{
		//The key is has now been processed for a frame, so set it to the appropriate state
		 SpecialKeyState[GLUT_KEY_SHIFT_L] =  INPUT_DOWN;
	}

	//Mouse Input
	if ( MouseState[0] ==  INPUT_DOWN_FIRST)
	{	//Left click

		 MouseState[0] =  INPUT_DOWN;
	}
	if ( MouseState[2] ==  INPUT_DOWN_FIRST)
	{	//Right click

		 MouseState[2] =  INPUT_DOWN;
	}
}
