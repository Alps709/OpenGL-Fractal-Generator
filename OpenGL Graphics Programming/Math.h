#pragma once
#include <iostream>

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

namespace Math
{
	//Remaps a value from one number range to another (used to remap mouse co-ord from world space to screen space)
	static double remap(double _value, double _oldMin, double _oldMax, double _newMin, double _newMax)
	{
		if (_oldMin == _oldMax)
		{
			std::cout << "\nError: Remap input value range is zero!\n";
			return 0;
		}

		if (_newMin == _newMax)
		{
			std::cout << "\nError: Remap output value range is zero!\n";
			return 0;
		}

		//Check for reversed input range
		bool reversedInput = false;
		const double oldMin = glm::min(_oldMin, _oldMax);
		const double oldMax = glm::max(_oldMin, _oldMax);
		if (oldMin != _oldMin)
		{
			reversedInput = true;
		}

		//Check for reversed output range
		bool reversedOutput = false;
		const double newMin = glm::min(_newMin, _newMax);
		const double newMax = glm::max(_newMin, _newMax);
		if (newMin != _newMin)
		{
			reversedOutput = true;
		}

		double portion = (_value - oldMin) * (newMax - newMin) / (oldMax - oldMin);
		if (reversedInput)
		{
			portion = (oldMin - _value) * (newMax - newMin) / (oldMax - oldMin);
		}

		return (reversedOutput) ? (newMax - portion) : (portion + newMin);
	}

	static glm::mat4 Create2DModelMatrix(float _translateX, float _translateY, float _translateZ, float _rotationAngle, float _scaleX, float _scaleY, float _scaleZ)
	{
		//Translation matrix
		const glm::mat4 translationMatrix = glm::translate(glm::mat4(), glm::vec3(_translateX, _translateY, _translateZ));

		//Rotation matrix
		const glm::mat4 rotationMatrixZ = glm::rotate(glm::mat4(), glm::radians(_rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));

		//Scale matrix
		const glm::mat4 scaleMatrix = glm::scale(glm::mat4(), glm::vec3(_scaleX, _scaleY, _scaleZ));

		//Return model matrix
		return translationMatrix * rotationMatrixZ * scaleMatrix;
	}

	inline void LimitVector3D(glm::vec3& _vec, float _maxVal)
	{
		if (glm::length(_vec) > _maxVal)
		{
			_vec = glm::normalize(_vec) * _maxVal;
		}
	}
}
