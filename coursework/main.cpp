#include "../nclgl/Window.h"
#include "Renderer.h"

int main()	{
    Window w("Make your own project!", 1280, 720, false);
    Renderer renderer(w);

    w.LockMouseToWindow(true);
    w.ShowOSPointer(false);

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
    }
    return 0;
}
