#include "Keyboard.h"

#include <SDL2/SDL.h>
#include <iostream>

Keyboard::Keyboard(HWND &hwnd)	{
	//Initialise the arrays to false!
	memset(keyStates, 0, sizeof(keyStates));
	memset(holdStates, 0, sizeof(holdStates));

	//Tedious windows RAW input stuff
	rid.usUsagePage		= HID_USAGE_PAGE_GENERIC;		//The keyboard isn't anything fancy
    rid.usUsage			= HID_USAGE_GENERIC_KEYBOARD;	//but it's definitely a keyboard!
    rid.dwFlags			= RIDEV_INPUTSINK;				//Yes, we want to always receive RAW input...
    rid.hwndTarget		= hwnd;							//Windows OS window handle
    RegisterRawInputDevices(&rid, 1, sizeof(rid));		//We just want one keyboard, please!
}

/*
Updates variables controlling whether a keyboard key has been
held for multiple frames.
*/
void Keyboard::UpdateHolds()	{
	memcpy(holdStates,keyStates, sizeof(keyStates));
}

/*
Sends the keyboard to sleep, so it doesn't process any
keypresses until it receives a Wake()
*/
void Keyboard::Sleep()	{
	isAwake = false;	//Night night!
	//Prevents incorrectly thinking keys have been held / pressed when waking back up
	memset(keyStates, 0, sizeof(keyStates));
	memset(holdStates, 0, sizeof(holdStates));
}

void Keyboard::update(SDL_KeyboardEvent& e)
{
	if (!isAwake) {
		return;
	}

	keyStates[e.keysym.scancode] = e.state == SDL_PRESSED;
}

/*
Returns if the key is down. Doesn't need bounds checking - 
a KeyboardKeys enum is always in range
*/
bool Keyboard::KeyDown(SDL_Scancode key)	{
	return keyStates[key];
}

/*
Returns if the key is down, and has been held down for multiple updates. 
Doesn't need bounds checking - a KeyboardKeys enum is always in range
*/
bool Keyboard::KeyHeld(SDL_Scancode key)	{
	return KeyDown(key) && holdStates[key];
}

/*
Returns true only if the key is down, but WASN't down last update.
Doesn't need bounds checking - a KeyboardKeys enum is always in range
*/
bool Keyboard::KeyTriggered(SDL_Scancode key)	 {
	return (KeyDown(key) && !KeyHeld(key));
}
