#pragma once

#include "Matrix4.h"
#include "Vector3.h"

class Keyboard;
class Mouse;

class Camera
{
public:
	Camera(Keyboard* keyboard, Mouse* mouse, float pitch = 0, float yaw = 0, float roll = 0, Vector3 position = Vector3(0, 0, 0));

	~Camera();

	void update(float dt);

	Matrix4 buildViewMatrix();

	const Vector3& getPosition() const { return position; }
	void setPosition(Vector3 position) { this->position = position; }

	float getPitch() const { return pitch; }
	void setPitch(float pitch) { this->pitch = pitch; }

	float getYaw() const { return yaw; }
	void setYaw(float yaw) { this->yaw = yaw; }

	float getRoll() const { return roll; }
	void setRoll(float roll) { this->roll = roll; }

	void setSpeed(float value) { speed = value; }
private:
	Matrix4 buildRotationMatrix(bool negate);

	Keyboard* keyboard;
	Mouse* mouse;

	float speed = 300.0f;

	float rollSpeed = 200.0f;

	float pitch;
	float yaw;
	float roll;
	Vector3 position;
};
