#include "../nclgl/Window.h"
#include "Renderer.h"

#include <fpng/src/fpng.h>

// Easier than CLI arguments
const bool Record = true;
const float RecordFramerate = 60;

int main()	{
    // This detects SSE support for faster compression
    fpng::fpng_init();
    Window w("Make your own project!", 1920, 1200, true);
    Renderer renderer(w, Record);

    w.LockMouseToWindow(true);
    w.ShowOSPointer(false);

    int frame = 0;
    float timeSinceLastLog = 0.0f;

    w.GetTimer()->Tick(); // Clear delta time to exclude loading time
    while(w.UpdateWindow()  && !w.GetKeyboard()->KeyDown(SDL_SCANCODE_ESCAPE)){
        float dt = Record ? (1.0 / RecordFramerate) : w.GetTimer()->GetTimeDeltaSeconds();
        renderer.UpdateScene(dt);
        renderer.RenderScene();
        w.swapBuffers();

        if (Record) {
            std::ostringstream ss;
            ss << "recording/frame_" << std::setfill('0') << std::setw(5) << frame << ".png";
            renderer.saveCurrentFrame(ss.str());
        }
        if (w.GetKeyboard()->KeyTriggered(SDL_SCANCODE_F5)) {
            Shader::ReloadAllShaders();
        }

        if (w.GetKeyboard()->KeyDown(SDL_SCANCODE_1)) {
            renderer.setTimeWarpFactor(renderer.getTimeWarpFactor() - dt);
        } else if (w.GetKeyboard()->KeyDown(SDL_SCANCODE_2)) {
			renderer.setTimeWarpFactor(renderer.getTimeWarpFactor() + dt);
		}

        if (w.GetKeyboard()->KeyTriggered(SDL_SCANCODE_3)) {
            renderer.setLockFrustum(!renderer.isFrustumLocked());
        }
        if (w.GetKeyboard()->KeyTriggered(SDL_SCANCODE_4)) {
            renderer.summonLight();
		}
        if (w.GetKeyboard()->KeyTriggered(SDL_SCANCODE_5)) {
            renderer.toggleSun();
        }

        if (w.GetKeyboard()->KeyTriggered(SDL_SCANCODE_O)) {
            renderer.getCameraPath()->skipToEnd();
        }
        if (w.GetKeyboard()->KeyTriggered(SDL_SCANCODE_L)) {
			auto path = renderer.getCameraPath();
            path->setActive(!path->getActive());
		}
        if (w.GetKeyboard()->KeyTriggered(SDL_SCANCODE_N)) {
            auto path = renderer.getCameraPath();
            path->setActiveFrame(path->getActiveFrame() + 1);
        }

        frame++;
        timeSinceLastLog += w.GetTimer()->GetTimeDeltaSeconds();
        if (frame % 120 == 0) {
			std::cout << "FPS: " << 120 / timeSinceLastLog << std::endl;
			timeSinceLastLog = 0.0f;
		}

        if (Record && !renderer.getCameraPath()->getActive()) {
            std::cout << "Recording done!" << std::endl;
            break;
        }
    }
    return 0;
}
