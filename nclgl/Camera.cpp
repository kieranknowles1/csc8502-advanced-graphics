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

Matrix4 Camera::buildRotationMatrix(bool negate) {
	// When building a view matrix, we also need to reverse order
	// of operations, otherwise look directions will be funky
	// with a rotated camera
	if (negate) {
		return Matrix4::Rotation(-pitch, Vector3(1, 0, 0)) *
			Matrix4::Rotation(-roll, Vector3(0, 0, 1)) *
			Matrix4::Rotation(-yaw, Vector3(0, 1, 0));
	}
	else {
		return Matrix4::Rotation(yaw, Vector3(0, 1, 0)) *
			Matrix4::Rotation(roll, Vector3(0, 0, 1)) *
			Matrix4::Rotation(pitch, Vector3(1, 0, 0));
	}
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

	Matrix4 rotation = buildRotationMatrix(false);
	// Calculate the forward and right vectors relative to the camera
	Vector3 forward = rotation * Vector3(0, 0, -1);
	Vector3 right = rotation * Vector3(1, 0, 0);
	Vector3 up = rotation * Vector3(0, 1, 0);

	float distance = speed * dt;
	// You've got to have afterburners on your spaceship
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
	// Up/down with proper space sim controls
	// (Freespace best sim, great gameplay and banging soundtrack)
	if (keyboard->KeyDown(SDL_SCANCODE_SPACE)) {
		position += up * distance;
	}
	if (keyboard->KeyDown(SDL_SCANCODE_C)) {
		position -= up * distance;
	}

	// What good is freedom if you don't have 6 degrees of it?
	// TODO: Roll should also affect pitch/yaw controls
	//if (keyboard->KeyDown(KEYBOARD_Q)) {
	//	roll += rollSpeed * dt;
	//}
	//if (keyboard->KeyDown(KEYBOARD_E)) {
	//	roll -= rollSpeed * dt;
	//}

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
	return
		buildRotationMatrix(true)
		* Matrix4::Translation(-position);
}