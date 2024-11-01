#include "../nclgl/Window.h"
#include "Renderer.h"

int main()	{
    Window w("Make your own project!", 1280, 720, false);
    Renderer renderer(w);

    w.LockMouseToWindow(true);
    w.ShowOSPointer(false);

    while(w.UpdateWindow()  && !w.GetKeyboard()->KeyDown(SDL_SCANCODE_ESCAPE)){
        renderer.UpdateScene(w.GetTimer()->GetTimeDeltaSeconds());
        renderer.RenderScene();
        w.swapBuffers();
        if (w.GetKeyboard()->KeyDown(SDL_SCANCODE_F5)) {
            Shader::ReloadAllShaders();
        }
    }
    return 0;
}
