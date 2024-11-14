#include "../nclgl/Window.h"
#include "Renderer.h"

#include <fpng/src/fpng.h>

const float RecordFramerate = 60;

struct Cli {
    // Very primitive CLI parser, only supports flags and not key-value pairs
    Cli(int argc, char** argv) {
        record = false;
        for (int i = 1; i < argc; i++) {
            std::string arg = argv[i];
            if (arg == "-r") {
                record = true;
            }
        }
    }
    bool record;
};

void handleInput(Renderer& renderer, Keyboard* kb, float deltaTime) {
    if (kb->KeyTriggered(SDL_SCANCODE_F5)) {
        Shader::ReloadAllShaders();
    }

    if (kb->KeyDown(SDL_SCANCODE_1)) {
        renderer.setTimeWarpFactor(renderer.getTimeWarpFactor() - deltaTime);
    }
    else if (kb->KeyDown(SDL_SCANCODE_2)) {
        renderer.setTimeWarpFactor(renderer.getTimeWarpFactor() + deltaTime);
    }

    if (kb->KeyTriggered(SDL_SCANCODE_3)) {
        renderer.setLockFrustum(!renderer.isFrustumLocked());
    }
    if (kb->KeyTriggered(SDL_SCANCODE_4)) {
        renderer.summonLight();
    }
    if (kb->KeyTriggered(SDL_SCANCODE_5)) {
        renderer.toggleSun();
    }

    if (kb->KeyTriggered(SDL_SCANCODE_O)) {
        renderer.getCameraPath()->skipToEnd();
    }
    if (kb->KeyTriggered(SDL_SCANCODE_L)) {
        auto path = renderer.getCameraPath();
        path->setActive(!path->getActive());
    }
    if (kb->KeyTriggered(SDL_SCANCODE_N)) {
        auto path = renderer.getCameraPath();
        path->setActiveFrame(path->getActiveFrame() + 1);
    }
}

int main(int argc, char** argv) {
    // This detects SSE support for faster compression
    fpng::fpng_init();
    Cli cli(argc, argv);


    Window w("Make your own project!", 1920, 1200, true);
    Renderer renderer(w, cli.record);

    w.LockMouseToWindow(true);
    w.ShowOSPointer(false);

    int frame = 0;
    float timeSinceLastLog = 0.0f;

    w.GetTimer()->Tick(); // Clear delta time to exclude loading time
    while(w.UpdateWindow()  && !w.GetKeyboard()->KeyDown(SDL_SCANCODE_ESCAPE)){
        float dt = cli.record ? (1.0 / RecordFramerate) : w.GetTimer()->GetTimeDeltaSeconds();
        renderer.UpdateScene(dt);
        renderer.RenderScene();
        w.swapBuffers();

        if (cli.record) {
            std::ostringstream ss;
            ss << "recording/frame_" << std::setfill('0') << std::setw(5) << frame << ".png";
            renderer.saveCurrentFrame(ss.str());
        }
        // Disable input when recording, exception: ESC to quit
        if (!cli.record) {
			handleInput(renderer, w.GetKeyboard(), dt);
		}

        frame++;
        timeSinceLastLog += w.GetTimer()->GetTimeDeltaSeconds();
        if (frame % 120 == 0) {
			std::cout << "FPS: " << 120 / timeSinceLastLog << std::endl;
			timeSinceLastLog = 0.0f;
		}

        // When recording, stop after the camera path is done
        if (cli.record && !renderer.getCameraPath()->getActive()) {
            std::cout << "Recording done!" << std::endl;
            break;
        }
    }
    return 0;
}
