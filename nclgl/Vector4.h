/*
Class:Vector4
Implements:
Author:Rich Davison
Description:VERY simple Vector4 class. Students are encouraged to modify this as necessary!

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*/
#pragma once

#include "Vector3.h"

#include <glad/glad.h>

class Vector4	{
public:
	Vector4(void) {
		x = y = z = w = 1.0f;
	}
	Vector4(float x, float y, float z, float w) {
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	// Create a Vector4 from 8-bit RGBA values
	static Vector4 rgba(int r, int g, int b, int a) {
		return Vector4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
	}

	Vector3 ToVector3() {
		return Vector3(x, y, z);
	}

	inline void operator+=(const Vector4& a) {
		x += a.x;
		y += a.y;
		z += a.z;
		w += a.w;
	}

	inline void operator-=(const Vector4& a) {
		x -= a.x;
		y -= a.y;
		z -= a.z;
		w -= a.w;
	}

	Vector4 Normalised() const {
		Vector4 n = *this;
		n.Normalise();
		return n;
	}

	void	Normalise() {
		float length = Length();

		if (length != 0.0f) {
			length = 1.0f / length;
			x = x * length;
			y = y * length;
			z = z * length;
			w = w * length;
		}
	}

	float			Length() const {
		return sqrt((x * x) + (y * y) + (z * z) + (w * w));
	}

	inline Vector4  operator*(const float a) const {
		return Vector4(x * a, y * a, z * a, w * a);
	}

	// Bind this vector to a shader program at the specified location
	void bind(int loc) {
		glUniform4fv(loc, 1, (float*)&x);
	}

	~Vector4(void){}

	float x;
	float y;
	float z;
	float w;
};

