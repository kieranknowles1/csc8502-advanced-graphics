/******************************************************************************
Class:Keyboard
Implements:InputDevice
Author:Rich Davison
Description:RAW Input keyboard class. I've made absolutely no attempt to ensure
that this class works for multiple keyboards attached to a single system.

You shouldn't really have to mess with this class too much, there's nothing too
interesting about it!

STUDENT CHALLENGE! You could have a function pointer per key, that can be
automatically called when a key is triggered / held? (Checked from within the
Update function)

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*//////////////////////////////////////////////////////////////////////////////
#pragma once
#include "InputDevice.h"

#include <SDL2/SDL_scancode.h>

struct SDL_KeyboardEvent;

class Keyboard : public InputDevice	{
public:
	friend class Window;

	//Is this key currently pressed down?
	bool KeyDown(SDL_Scancode key);
	//Has this key been held down for multiple frames?
	bool KeyHeld(SDL_Scancode key);
	//Is this the first update the key has been pressed for?
	bool KeyTriggered(SDL_Scancode key);

protected:
	Keyboard();
	~Keyboard(void){}
	//Update the holdStates array...call this each frame!
	virtual void UpdateHolds();
	//Sends the keyboard to sleep
	virtual void Sleep();

	void update(SDL_KeyboardEvent& e);

	bool keyStates[SDL_NUM_SCANCODES]; // Key down
	bool holdStates[SDL_NUM_SCANCODES]; // Key down for multiple frames
};



