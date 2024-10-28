/*
Class:Window
Author:Rich Davison
Description:Creates and handles the Window, including the initialisation of the mouse and keyboard.
*/
#pragma once

#include "common.h"
#include <string>

#include <stdio.h>
#include <fcntl.h>

#include "OGLRenderer.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "GameTimer.h"



#define VC_EXTRALEAN
#define WINDOWCLASS "WindowClass"

// TODO: Use SDL2 for input handling
#ifndef MSG
// WinAPI types are horrible, WinAPI is horrible, Windows is horrible.
// Embrace the penguin, join Linus, and be free of this madness.
#define MSG int
#define LRESULT int
#define WPARAM int
#define LPARAM int
#define HDC int
#define HGLRC int
#define CALLBACK
#define UINT int
#endif

class OGLRenderer;

class Window	{
public:
	Window(std::string title = "OpenGL Framework", int sizeX = 800, int sizeY = 600, bool fullScreen = false);
	~Window(void);

	bool	UpdateWindow();

	void	SetRenderer(OGLRenderer* r);

	HWND	GetHandle();

	bool	HasInitialised();

	void	LockMouseToWindow(bool lock);
	void	ShowOSPointer(bool show);

	const std::string& GetTitle()   const { return windowTitle; }
	void				SetTitle(const std::string& title) {
		windowTitle = title;
		// We're a normal company, we compile C++
		// Don't follow any of its rules, standards, or guidelines
		// or any of its libraries
		//SetWindowText(windowHandle, windowTitle.c_str());
	};

	Vector2	GetScreenSize() {return size;};

	static Keyboard*	GetKeyboard()	{return keyboard;}
	static Mouse*		GetMouse()		{return mouse;}

	GameTimer*   GetTimer()		{return timer;}

protected:
	void	CheckMessages(MSG &msg);
	static LRESULT CALLBACK WindowProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);

	HWND			windowHandle;

	static Window*		window;
	static Keyboard*	keyboard;
	static Mouse*		mouse;

	GameTimer*	timer;

	OGLRenderer*		renderer;

	bool				forceQuit;
	bool				init;
	bool				fullScreen;
	bool				lockMouse;
	bool				showMouse;
	bool				mouseLeftWindow;
	bool				isActive;

	Vector2				position;
	Vector2				size;

	std::string			windowTitle;
};
