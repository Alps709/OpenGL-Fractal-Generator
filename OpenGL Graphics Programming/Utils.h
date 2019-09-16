#pragma once
#include <cmath>
#include <fstream>
#include <string>
#include <thread>
#include "ThreadPool.h"

namespace Utils
{
	//This provides an optimization by using a custom vec2 instead of std::complex
	static bool useVec2 = true;

	//This provides an optimization by setting a pixels colour based on the 4 orthogonally adjacent pixels each having the same colour
	//and thereby skipping the costly calculations needed
	static bool skipPixels = true;

	static unsigned int maxIterations = 500;
	static float currentIterationCount = 1;

	static unsigned int threadNum = std::thread::hardware_concurrency();
	
	const static int SCREEN_WIDTH = 1280;
	const static int SCREEN_HEIGHT = 720;

	static unsigned int skipNum = 0;

	static long double leftBorder = -2.0, rightBorder = 1.0, topBorder = 1.20, bottomBorder = -1.2;

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

	static void LoadFile()
	{
		std::string line;
		std::ifstream configfile;
		configfile.open("Config.txt");
		if(configfile.is_open())
		{
			//Error checking of file reading
			try 
			{
				while(std::getline(configfile, line))
				{
					std::istringstream lineInput(line.substr(line.find(": ") + 1));
					if(line.find("Thread count") != -1)
					{
						lineInput >> threadNum;
					}
					else if(line.find("Max iterations"))
					{
						lineInput >> maxIterations;
					}
					else if(line.find("Use vec2 optimization"))
					{
						unsigned int tempInt;
						lineInput >> tempInt;
						useVec2 = tempInt;
					}
					else if(line.find("Use pixel skipping optimization"))
					{
						unsigned int tempInt;
						lineInput >> tempInt;
						skipPixels = tempInt;
					}
				}
			}
			catch (...)
			{
				std::cout << "\n\n There was a problem reading the config file! Please make sure you changed only the numbers!\n\n";
				system("pause");
				exit(-1);
			}
		}

		std::cout << "Loaded config file!\n\n"
			<< "The program will use " << threadNum << " threads.\n"
			<< "The max amount of iterations is set to " << maxIterations << ".\n"
			<< "Using vec2 optimization: " << ((useVec2 == 1) ? "true\n" : "false\n")
			<< "Using pixel skipping optimization: " << ((skipPixels == 1) ? "true\n" : "false\n");
		system("pause");
	}
}
