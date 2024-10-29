/*
Class:Window
Author:Rich Davison
Description:Creates and handles the Window, including the initialisation of the mouse and keyboard.
*/
#pragma once

#include "common.h"
#include <string>

#include <windows.h>
#include <io.h>
#include <stdio.h>
#include <fcntl.h>

#include "OGLRenderer.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "GameTimer.h"

#define VC_EXTRALEAN
#define WINDOWCLASS "WindowClass"

class OGLRenderer;

// SDL likes to use its own main function, we don't want it
struct SDL_Window;

class Window	{
public:
	Window(std::string title = "OpenGL Framework", int sizeX = 800, int sizeY = 600, bool fullScreen = false);
	~Window(void);

	bool	UpdateWindow();	

	void	SetRenderer(OGLRenderer* r);

	SDL_Window* getSdlWindow() {
		return sdlWindow;
	}

	void swapBuffers();

	void	LockMouseToWindow(bool lock);
	void	ShowOSPointer(bool show);

	const std::string& GetTitle()   const { return windowTitle; }
	void				SetTitle(const std::string& title);

	Vector2	GetScreenSize() {return size;};

	static Keyboard*	GetKeyboard()	{return keyboard;}
	static Mouse*		GetMouse()		{return mouse;}

	GameTimer*   GetTimer()		{return timer;}

protected:
	void	CheckMessages(MSG &msg);

	HWND			windowHandle;

	static Window*		window;
	static Keyboard*	keyboard;
	static Mouse*		mouse;

	SDL_Window* sdlWindow;

	GameTimer*	timer;

	OGLRenderer*		renderer;

	bool				forceQuit;
	bool				fullScreen;
	bool				lockMouse;
	bool				showMouse;
	bool				mouseLeftWindow;
	bool				isActive;

	Vector2				position;
	Vector2				size;

	std::string			windowTitle;
};