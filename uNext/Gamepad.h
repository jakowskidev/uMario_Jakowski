#pragma once
#include "header.h"
#include <string.h>


#ifndef GAMEPAD_H
#define GAMEPAD_H

#define MAX_CONTROLLERS 4
class GameController {
	int MaxJoysticks = SDL_NumJoysticks();
	SDL_GameController *Controllers[MAX_CONTROLLERS];
public:
	
	int ControllerIndex = 0;

	GameController(void);
	~GameController(void);


	bool GetButton(int ControllerNum, std::string Button);

	int Add(int  JoystickIndex);

	void Remove(SDL_JoystickID ControllerID);
};


#endif