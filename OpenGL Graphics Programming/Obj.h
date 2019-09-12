#pragma once
#include <vector>

namespace Objects
{
	//Scrolling texture
	std::vector<float> vertices1 =
	{
		///Bottom right quad
		//Position			//Texture co-ords
		-1.0f,-1.0f, 0.0f,	0.0f, 1.0f, //Bottom left
		-1.0f, 1.0f, 0.0f,	0.0f, 0.0f, //Top left
		 1.0f, 1.0f, 0.0f,	1.0f, 0.0f, //Top right
		 1.0f,-1.0f, 0.0f,	1.0f, 1.0f, //Bottom right
	};

	std::vector<unsigned int> indices1 =
	{
		//First quad							
		0, 2, 1,
		0, 3, 2,
	};
}
