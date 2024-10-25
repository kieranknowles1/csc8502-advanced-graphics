#include "../nclgl/window.h"
#include "Renderer.h"

int main() {	
	Window w("Index Buffers!", 1280, 720,false);
	if(!w.HasInitialised()) {
		return -1;
	}

	Renderer renderer(w);
	if(!renderer.HasInitialised()) {
		return -1;
	}

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);

	while(w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)){
		float timestep = w.GetTimer()->GetTimeDeltaSeconds();
		renderer.UpdateScene(timestep);
		renderer.RenderScene();
		renderer.SwapBuffers();
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_F5)) {
			Shader::ReloadAllShaders();
		}

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_PLUS)) {
			renderer.setWaterLevel(renderer.getWaterLevel() + 0.01f);
		}

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_MINUS)) {
			renderer.setWaterLevel(renderer.getWaterLevel() - 0.01f);
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_1)) {
			renderer.setWaveAmplitude(renderer.getWaveAmplitude() + 0.001f);
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_2)) {
			renderer.setWaveAmplitude(renderer.getWaveAmplitude() - 0.001f);
		}
	}
	return 0;
}