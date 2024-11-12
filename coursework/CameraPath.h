#pragma once

#include <vector>

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

    void setActiveFrame(int index, float time = 0) {
        currentFrameIndex = index;
        currentFrameTime = time;
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
