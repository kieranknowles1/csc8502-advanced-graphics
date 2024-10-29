#include "Keyboard.h"

#include <iostream>
#include <string.h>

#include <SDL2/SDL.h>

Keyboard::Keyboard()	{
	//Initialise the arrays to false!
	memset(keyStates, 0, sizeof(keyStates));
	memset(holdStates, 0, sizeof(holdStates));
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
