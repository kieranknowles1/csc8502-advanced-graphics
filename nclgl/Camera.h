#pragma once

#include "Matrix4.h"
#include "Vector3.h"

class Camera
{
public:
	Camera(float pitch, float yaw, float roll, Vector3 position);

	~Camera();

	void update(float dt);

	Matrix4 buildViewMatrix();

	Vector3 getPosition() const { return position; }
	void setPosition(Vector3 position) { this->position = position; }

	float getPitch() const { return pitch; }
	void setPitch(float pitch) { this->pitch = pitch; }

	float getYaw() const { return yaw; }
	void setYaw(float yaw) { this->yaw = yaw; }

	float getRoll() const { return roll; }
	void setRoll(float roll) { this->roll = roll; }
private:
	float speed = 300.0f;

	float rollSpeed = 200.0f;

	float pitch;
	float yaw;
	float roll;
	Vector3 position;
};
