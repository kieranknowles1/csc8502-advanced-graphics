/******************************************************************************
Class:InputDevice
Implements:
Author:Rich Davison
Description:Abstract base class for Windows RAW keyboard / mouse input

Input devices can be temporarily sent to sleep (so keyboard input doesn't work
when the game is minimised etc), and obviously woken up again.

Input devices may also keep track of 'holds' - i.e keys or buttons pressed for
more than one frame. This allows you to have both things that trigger once,
and are continuously active as long as a key / button is pressed.

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""      
 
*//////////////////////////////////////////////////////////////////////////////

#pragma once
#include "common.h"

class InputDevice	{
protected:
	friend class Window;
	InputDevice(void) { isAwake = true;};
	~InputDevice(void){};

protected:
	virtual void UpdateHolds() {}
	virtual void Sleep(){ isAwake = false;}
	virtual void Wake() { isAwake = true;}

	bool			isAwake;		//Is the device awake...
};