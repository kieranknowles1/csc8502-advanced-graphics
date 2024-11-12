#include "Camera.h"

#include <algorithm>

#include "Keyboard.h"
#include "Mouse.h"

Camera::Camera(Keyboard* keyboard, Mouse* mouse, float pitch, float yaw, float roll, Vector3 position)
	: pitch(pitch)
	, keyboard(keyboard)
	, mouse(mouse)
	, yaw(yaw)
	, roll(roll)
	, position(position) {
}

Camera::~Camera() {
}

void Camera::update(float dt) {
	// Update our pitch and yaw from the mouse
	// TODO: Rotation borks pitch/yaw controls, we need to turn
	// relative to the rotated angle
	pitch -= mouse->GetRelativePosition().y;
	yaw -= mouse->GetRelativePosition().x;

	// Clamp pitch to stop us going upside down
	pitch = std::min(pitch, 90.0f);
	pitch = std::max(pitch, -90.0f);

	// Wrap yaw to keep it in the range 0-360
	while (yaw < 0) {
		yaw += 360;
	}
	while (yaw >= 360) {
		yaw -= 360;
	}

	Matrix4 rotation = Matrix4::pitchYawRoll(false, pitch, yaw, roll);
	// Calculate the forward and right vectors relative to the camera
	Vector3 forward = rotation * Vector3(0, 0, -1);
	Vector3 right = rotation * Vector3(1, 0, 0);
	Vector3 up = rotation * Vector3(0, 1, 0);

	float distance = speed * dt;
	// You've got to have afterburners on your spaceship
	// Tab, because that's what Freespace did
	if (keyboard->KeyDown(SDL_SCANCODE_TAB)) {
		distance *= 2;
	}

	// Move the camera with FPS-style controls
	if (keyboard->KeyDown(SDL_SCANCODE_W)) {
		position += forward * distance;
	}
	if (keyboard->KeyDown(SDL_SCANCODE_S)) {
		position -= forward * distance;
	}
	if (keyboard->KeyDown(SDL_SCANCODE_A)) {
		position -= right * distance;
	}
	if (keyboard->KeyDown(SDL_SCANCODE_D)) {
		position += right * distance;
	}
	// Up/down with Space Engineers bindings
	if (keyboard->KeyDown(SDL_SCANCODE_SPACE)) {
		position += up * distance;
	}
	if (keyboard->KeyDown(SDL_SCANCODE_C)) {
		position -= up * distance;
	}

	if (keyboard->KeyTriggered(SDL_SCANCODE_P)) {
		std::cout << "Camera at: " << position << ", " << pitch << ", " << yaw << ", " << roll << std::endl;
	}
}

Matrix4 Camera::buildViewMatrix() {
	return Matrix4::view(position, pitch, yaw, roll);
}
