#pragma once

#ifndef COIN_H
#define COIN_H

#include "header.h"

class Coin
{
private:
	int iXPos, iYPos;
	int iLEFT;

	int iSpriteID;
	int iStepID;

	bool bTOP;

	bool bDelete;
public:
	Coin(int iXPos, int iYPos);
	~Coin(void);

	void Update();
	void Draw(SDL_Renderer* rR);

	int getXPos();
	int getYPos();
	bool getDelete();
};

#endif