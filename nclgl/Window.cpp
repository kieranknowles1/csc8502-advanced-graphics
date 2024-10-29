#include "Window.h"
#include "Mouse.h"
#include "Keyboard.h"

#include "SDL2/SDL.h"
#include "SDL2/SDL_syswm.h"

Window::Window(std::string title, int sizeX, int sizeY, bool fullScreen)	{
	SDL_version version;
	SDL_GetVersion(&version);

	SDL_version compiled;
	SDL_VERSION(&compiled);

	std::cout << "SDL version: " << (int)version.major << "." << (int)version.minor << "." << (int)version.patch << std::endl;
	std::cout << "Compiled against: " << (int)compiled.major << "." << (int)compiled.minor << "." << (int)compiled.patch << std::endl;

	sdlWindow = SDL_CreateWindow(
		title.c_str(),
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		sizeX, sizeY,
		SDL_WINDOW_OPENGL
	);

	// TODO: This is platform specific, we should use SDL to abstract windows stuff away
	SDL_SysWMinfo sysInfo;
	SDL_VERSION(&sysInfo.version);
	bool ok = SDL_GetWindowWMInfo(sdlWindow, &sysInfo);
	if (!ok) {
		auto message = SDL_GetError();
		throw std::runtime_error("Failed to get SDL window info: " + std::string(message));
	}

	renderer		= NULL;
	forceQuit		= false;
	mouseLeftWindow	= false;
	lockMouse		= false;
	showMouse		= true;
	windowTitle		= title;

	this->fullScreen = fullScreen;

	size.x = (float)sizeX; size.y = (float)sizeY;

	fullScreen ? position.x = 0.0f : position.x = 100.0f;
	fullScreen ? position.y = 0.0f : position.y = 100.0f;

	HINSTANCE hInstance = GetModuleHandle( NULL );

	windowHandle = sysInfo.info.win.window;

 	if(!windowHandle) {
		// Should be caught by SDL_CreateWindow
		throw std::runtime_error("Failed to create window handle");
	}

	keyboard	= new Keyboard(windowHandle);
	mouse		= new Mouse(windowHandle);

	timer		= new GameTimer();

	Window::GetMouse()->SetAbsolutePositionBounds((unsigned int)size.x,(unsigned int)size.y);

	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(windowHandle, &pt);
	Window::GetMouse()->SetAbsolutePosition(pt.x,pt.y);

	LockMouseToWindow(lockMouse);
	ShowOSPointer(showMouse);
	isActive = true;
}


Window::~Window(void)
{
	delete keyboard;
	delete mouse;
}

void Window::swapBuffers() {
	SDL_GL_SwapWindow(sdlWindow);
}

void	Window::SetRenderer(OGLRenderer* r)	{
	renderer = r;
	if(r) {
		renderer->Resize((int)size.x,(int)size.y);				
	}
}

bool	Window::UpdateWindow() {
	MSG		msg;

	timer->Tick();

	float diff = timer->GetTimeDeltaSeconds();

	Window::GetMouse()->UpdateDoubleClick(diff);

	Window::GetKeyboard()->UpdateHolds();
	Window::GetMouse()->UpdateHolds();

	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		handleEvent(e);
	}
	return !forceQuit;
}

void Window::handleEvent(SDL_Event& e) {
	switch (e.type) {
	case SDL_QUIT:
		forceQuit = true;
		break;
	case SDL_KEYDOWN:
	case SDL_KEYUP:
		keyboard->update(e.key);
		break;
	case SDL_MOUSEMOTION:
		mouse->update(e.motion);
		break;
	case SDL_MOUSEBUTTONDOWN:
	case SDL_MOUSEBUTTONUP:
	case SDL_MOUSEWHEEL:
		// TODO: Handle this
		break;
	default:
		break;
	}
}

void	Window::LockMouseToWindow(bool lock)	{
	lockMouse = lock;
	SDL_SetRelativeMouseMode(lock ? SDL_TRUE : SDL_FALSE);
}

void Window::ShowOSPointer(bool show) {
	showMouse = show;
	SDL_ShowCursor(show ? SDL_ENABLE : SDL_DISABLE);
}

void Window::SetTitle(const std::string& title) {
	windowTitle = title;
	SDL_SetWindowTitle(sdlWindow, title.c_str());
}
