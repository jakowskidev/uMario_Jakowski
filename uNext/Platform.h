#pragma once

#ifndef PLATFORM_H
#define PLATFORM_H

#include "header.h"

class Platform
{
private:
	// -- 0 = BOT, 1 = TOP, 2 = TOP & BOT, 3 = RIGHT & LEFT, 4 = BONUS, 5 = FALLING, 6 = SEESAW, 7 = FALLING SEESAW
	int iType;

	int iXStart, iXEnd, iYStart, iYEnd;

	float fXPos, fYPos;

	// -- true = RIGHT | TOP, false = LEFT | BOT
	bool direction;

	int iSize;

	bool ON; // -- iType = 4, BONUS, OFF & ON

	int seesawPlatformID;
public:
	Platform(int iSize, int iType, int iXStart, int iXEnd, int iYStart, int iYEnd, float fXPos, float fYPos, bool direction, int seesawPlatformID = 0);
	~Platform(void);

	void Update();
	void Draw(SDL_Renderer* rR);

	void moveY();
	void moveYReverse();

	int getXPos();
	int getYPos();
	int getSize();

	float getMoveX();
	int getMoveY();

	int getTypeID();
	void setTypeID(int iType);
	void turnON();
};

#endif