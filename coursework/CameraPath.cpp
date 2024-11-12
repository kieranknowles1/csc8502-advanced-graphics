#include "CameraPath.h"

#include <algorithm>

template <typename T>
static T lerp(const T& a, const T& b, float ratio)
{
	return a + ((b - a) * ratio);
}

CameraPath::CameraPath(Camera* camera)
	: camera(camera)
	, active(false)
	, currentFrameIndex(0)
	, currentFrameTime(0.0f)
{
}

void CameraPath::reset()
{
	currentFrameIndex = 0;
	currentFrameTime = 0.0f;
}

void CameraPath::skipToEnd()
{
	setActive(false);
	currentFrameIndex = keyFrames.size() - 1;
	currentFrameTime = getFrame(currentFrameIndex).duration;
	updateCameraPosition();
}

void CameraPath::updateCameraPosition()
{
	auto& current = getFrame(currentFrameIndex);
	auto& next = getFrame(currentFrameIndex + 1);
	float ratio = currentFrameTime / current.duration;

	camera->setPosition(lerp(current.position, next.position, ratio));
	camera->setPitch(lerp(current.pitch, next.pitch, ratio));
	camera->setYaw(lerp(current.yaw, next.yaw, ratio));
	camera->setRoll(lerp(current.roll, next.roll, ratio));
}

KeyFrame& CameraPath::getFrame(int index) {
	index = std::max(0, std::min(index, (int)keyFrames.size() - 1));

	return keyFrames[index];
}

void CameraPath::update(float dt)
{
	if (!active) {
		return;
	}

	currentFrameTime += dt;
	while (currentFrameTime > getFrame(currentFrameIndex).duration) {
		currentFrameTime -= getFrame(currentFrameIndex).duration;
		currentFrameIndex++;
	}

	updateCameraPosition();

	// We have reached the end of the path, and held for the duration of
	// the last frame
	if (currentFrameIndex > keyFrames.size()) {
		setActive(false);
	}
}
