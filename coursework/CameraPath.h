#pragma once

#include <vector>
#include <functional>

#include "../nclgl/Camera.h"

struct KeyFrame {
    Vector3 position;
    float pitch;
    float yaw;
    float roll;

    // Seconds to reach the next frame from this one
    // For the last frame, stay at the end for this duration
    // To hold for a duration, use two points with the same position and orientation
    float duration;

    // Callback to trigger when this frame becomes active, either
    // from natural progression or setActiveFrame.
    // Use std::bind to pass arguments to the callback
    // Will not be triggered if this frame is skipped by setActiveFrame
    // This is the case when manually setting the frame, but not when
    // there is lag.
    std::function<void()> onBegin;
};

class CameraPath {
public:
    CameraPath(Camera* camera);

    void addKeyFrame(const KeyFrame& keyFrame) {
        keyFrames.push_back(keyFrame);
    }

    // Reset the path to the beginning
    void reset();
    
    void setActive(bool active) {
		this->active = active;
	}
    bool getActive() const {
        return active;
    }

    // Set the currently active frame, possibly triggering the onBegin callback
    void setActiveFrame(int index, float time = 0) {
        currentFrameIndex = index;
        currentFrameTime = time;

        auto& frame = getFrame(currentFrameIndex);
        if (frame.onBegin) {
			frame.onBegin();
		}
    }
    int getActiveFrame() const {
		return currentFrameIndex;
	}

    void skipToEnd();
    void update(float dt);
private:
    Camera* camera;
    std::vector<KeyFrame> keyFrames;

    // Get the keyframe at the given index, clamped to the bounds of the vector
    KeyFrame& getFrame(int index);

    void updateCameraPosition();

    bool active;
    int currentFrameIndex;
    float currentFrameTime;
};
