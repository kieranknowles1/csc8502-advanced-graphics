#include "Window.h"
#include "Mouse.h"
#include "Keyboard.h"

#include "SDL2/SDL.h"
#include "SDL2/SDL_syswm.h"

Window* Window::window		= nullptr;
Keyboard*Window::keyboard	= nullptr;
Mouse*Window::mouse			= nullptr;

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
	window			= this;
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

	if(!keyboard) {
		keyboard	= new Keyboard(windowHandle);
	}
	if(!mouse) {
		mouse		= new Mouse(windowHandle);
	}

	timer		= new GameTimer();

	Window::GetMouse()->SetAbsolutePositionBounds((unsigned int)size.x,(unsigned int)size.y);

	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(window->windowHandle, &pt);
	Window::GetMouse()->SetAbsolutePosition(pt.x,pt.y);

	LockMouseToWindow(lockMouse);
	ShowOSPointer(showMouse);
	isActive = true;
}


Window::~Window(void)
{
	delete keyboard;keyboard = nullptr;
	delete mouse;	mouse	 = nullptr;
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

	while(PeekMessage(&msg,windowHandle,0,0,PM_REMOVE)) {
		CheckMessages(msg); 
	}
	return !forceQuit;
}

void Window::CheckMessages(MSG &msg)	{
	switch (msg.message)	{				// Is There A Message Waiting?
		case (WM_QUIT):
		case (WM_CLOSE): {					// Have We Received A Quit Message?
			window->ShowOSPointer(true);
			window->LockMouseToWindow(false);
			forceQuit = true;
		}break;
		case (WM_INPUT): {
			UINT dwSize;
			GetRawInputData((HRAWINPUT)msg.lParam, RID_INPUT, NULL, &dwSize,sizeof(RAWINPUTHEADER));

			BYTE* lpb = new BYTE[dwSize];
	
			GetRawInputData((HRAWINPUT)msg.lParam, RID_INPUT, lpb, &dwSize,sizeof(RAWINPUTHEADER));
			RAWINPUT* raw = (RAWINPUT*)lpb;

			if (keyboard && window->isActive && raw->header.dwType == RIM_TYPEKEYBOARD) {
				Window::GetKeyboard()->Update(raw);
			}

			if (mouse && window->isActive && raw->header.dwType == RIM_TYPEMOUSE) {
				Window::GetMouse()->Update(raw);
			}
			delete lpb;
		}break;

		default: {								// If Not, Deal With Window Messages
			TranslateMessage(&msg);				// Translate The Message
			DispatchMessage(&msg);				// Dispatch The Message
		}
	}
}

void	Window::LockMouseToWindow(bool lock)	{
	lockMouse = lock;

	// FIXME: SDL2's way isn't working
#ifdef _WIN32
	if(lock) {
		RECT		windowRect;
		GetWindowRect (window->windowHandle, &windowRect);

		SetCapture(window->windowHandle);
		ClipCursor(&windowRect);

		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(window->windowHandle, &pt);
		Window::GetMouse()->SetAbsolutePosition(pt.x,pt.y);
	}
	else{
		ReleaseCapture();
		ClipCursor(NULL);
	}
#endif // _WIN32
}

void Window::ShowOSPointer(bool show) {
	showMouse = show;
	SDL_ShowCursor(show ? SDL_ENABLE : SDL_DISABLE);
}

void Window::SetTitle(const std::string& title) {
	windowTitle = title;
	SDL_SetWindowTitle(sdlWindow, title.c_str());
}
