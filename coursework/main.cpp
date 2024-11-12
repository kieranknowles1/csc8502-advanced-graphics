#include "../nclgl/Window.h"
#include "Renderer.h"

int main()	{
    Window w("Make your own project!", 1280, 720, false);
    Renderer renderer(w);

    w.LockMouseToWindow(true);
    w.ShowOSPointer(false);

    w.GetTimer()->Tick(); // Clear delta time to exclude loading time
    while(w.UpdateWindow()  && !w.GetKeyboard()->KeyDown(SDL_SCANCODE_ESCAPE)){
        float dt = w.GetTimer()->GetTimeDeltaSeconds();
        renderer.UpdateScene(dt);
        renderer.RenderScene();
        w.swapBuffers();
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
    }
    return 0;
}
