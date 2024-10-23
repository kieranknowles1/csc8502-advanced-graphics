#include "../nclGL/window.h"
#include "Renderer.h"

int main() {
	Window w("Texturing!", 1280, 720,false);	 //This is all boring win32 window creation stuff!
	if(!w.HasInitialised()) {
		return -1;
	}
	
	Renderer renderer(w);	//This handles all the boring OGL 3.2 initialisation stuff, and sets up our tutorial!
	if(!renderer.HasInitialised()) {
		return -1;
	}

	float rotate = 0.0f;
	float scale = 1.0f;
	Vector2 position(0, 0);
	while(w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)){
		if(Window::GetKeyboard()->KeyDown(KEYBOARD_LEFT) ) {
			--rotate;
			scale += 0.01;
		}

		if(Window::GetKeyboard()->KeyDown(KEYBOARD_RIGHT) ) {
			++rotate;
			scale -= 0.01;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_W))
			position.y -= 0.01; // Negative since this is used as a janky camera
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_S))
			position.y += 0.01;
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_A))
			position.x -= 0.01;
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_D))
			position.x += 0.01;
		renderer.UpdateTextureMatrix(rotate, scale, position);

		if(Window::GetKeyboard()->KeyTriggered(KEYBOARD_1) ) {
			renderer.ToggleFiltering();
		}

		if(Window::GetKeyboard()->KeyTriggered(KEYBOARD_2) ) {
			renderer.ToggleRepeating();
		}

		renderer.RenderScene();
		renderer.SwapBuffers();
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_F5)) {
			Shader::ReloadAllShaders();
		}
	}

	return 0;
}