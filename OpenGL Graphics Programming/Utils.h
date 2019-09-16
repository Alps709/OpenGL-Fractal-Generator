#pragma once
#include <cmath>
#include <fstream>
#include <string>

namespace Utils
{
	//This provides an optimization by using a custom vec2 instead of std::complex
	const static bool useVec2 = true;

	//This provides an optimization by setting a pixels colour based on the 4 orthogonally adjacent pixels each having the same colour
	//and thereby skipping the costly calculations needed
	const static bool skipPixels = true;

	const static unsigned int maxIterations = 500;
	
	const static int SCREEN_WIDTH = 1280;
	const static int SCREEN_HEIGHT = 720;

	static unsigned int skipNum = 0;

	static double leftBorder = -2.0, rightBorder = 1.0, topBorder = 1.20, bottomBorder = -1.2;

	struct Pixel
	{
		unsigned char r;
		unsigned char g;
		unsigned char b;
		unsigned char a;

		bool operator==(const Pixel &other) const
		{
			return (r == other.r
				&& g == other.g
				&& b == other.b
				&& a == other.a);
		}
	};

	//Array of pixels that is made into a texture
	static Utils::Pixel pixelData[Utils::SCREEN_HEIGHT][Utils::SCREEN_WIDTH];

	struct Vec2
	{
		//This vec2 will be treated as a complex number for the mandlebrot calculation
		//Using a vec2 instead of std::complex increases performance considerably
		double x = 0.0;
		double y = 0.0;

		Vec2(double a, double b)
		{
			x = a;
			y = b;
		}

		Vec2 operator+(const Vec2 _other) const
		{
			return Vec2(x + _other.x, y + _other.y);
		}

		//Finds the absolute distance of the complex number from the origin
		double absDist() const
		{
			//The magnitude of a complex vector is its distance from the origin in the imaginary plane
			return sqrt(x * x + y * y);
		}
	};

	void LoadFile()
	{
		std::string line;
		std::ifstream configfile;
		configfile.open("Config.txt");
		if(configfile.is_open())
		{
			while(std::getline(configfile, line))
			{
				
			}
		}
	}
}
