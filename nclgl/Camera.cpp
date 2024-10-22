#include "Camera.h"

#include <algorithm>

#include "Window.h"

Camera::Camera(float pitch, float yaw, float roll, Vector3 position)
	: pitch(pitch)
	, yaw(yaw)
	, roll(roll)
	, position(position) {
}

Camera::~Camera() {
}

void Camera::update(float dt) {
	// Update our pitch and yaw from the mouse
	pitch -= Window::GetMouse()->GetRelativePosition().y;
	yaw -= Window::GetMouse()->GetRelativePosition().x;

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

	Matrix4 rotation =
		Matrix4::Rotation(yaw, Vector3(0, 1, 0))
		* Matrix4::Rotation(pitch, Vector3(1, 0, 0))
		* Matrix4::Rotation(roll, Vector3(0, 0, 1));
	// Calculate the forward and right vectors relative to the camera
	Vector3 forward = rotation * Vector3(0, 0, -1);
	Vector3 right = rotation * Vector3(1, 0, 0);
	Vector3 up = rotation * Vector3(0, 1, 0);

	float distance = speed * dt;
	// You've got to have afterburners on your spaceship
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_TAB)) {
		distance *= 2;
	}

	// Move the camera with FPS-style controls
	auto keyboard = Window::GetKeyboard();
	if (keyboard->KeyDown(KEYBOARD_W)) {
		position += forward * distance;
	}
	if (keyboard->KeyDown(KEYBOARD_S)) {
		position -= forward * distance;
	}
	if (keyboard->KeyDown(KEYBOARD_A)) {
		position -= right * distance;
	}
	if (keyboard->KeyDown(KEYBOARD_D)) {
		position += right * distance;
	}
	// Up/down with proper space sim controls
	// (Freespace best sim, great gameplay and banging soundtrack)
	if (keyboard->KeyDown(KEYBOARD_SPACE)) {
		position += up * distance;
	}
	if (keyboard->KeyDown(KEYBOARD_C)) {
		position -= up * distance;
	}

	// What good is freedom if you don't have 6 degrees of it?
	if (keyboard->KeyDown(KEYBOARD_1)) {
		roll += rollSpeed * dt;
	}
	if (keyboard->KeyDown(KEYBOARD_2)) {
		roll -= rollSpeed * dt;
	}

	// If you think these controls are complicated, you should try
	// Freespace, where you you'll be using the whole keyboard
	// I still use A+Z for throttle, space for missiles, and ,/. for weapon select
	// (I don't however, use semicolon to reload like in Deus Ex, that's just silly)
	// Still the best space sim, and it's beutifel with the SCP mods, some of
	// which make modern hardware struggle which doesn't matter because you
	// just called in 50 allies to the beat of the music and now your job
	// is to watch the fireworks after strafe-sniping the enemy cruisers
	// TLDR: It's £7 on Steam
}

Matrix4 Camera::buildViewMatrix() {
	// Everything is negated to move the world opposite to the camera's movement,
	// creating the illusion of a moving camera
	return Matrix4::Rotation(-pitch, Vector3(1, 0, 0))
		* Matrix4::Rotation(-yaw, Vector3(0, 1, 0))
		* Matrix4::Rotation(-roll, Vector3(0, 0, 1))
		* Matrix4::Translation(-position);
}