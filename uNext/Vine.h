#pragma once

#ifndef VINE_H
#define VINE_H

#include "Minion.h"

class Vine : public Minion
{
private:
	// -- minionState = 0 VINE START, minionState = 1 VINE END

	int iX, iY;
public:
	Vine(int iXPos, int iYPos, int minionState, int iBlockID);
	~Vine(void);

	void Update();
	void Draw(SDL_Renderer* rR, CIMG* iIMG);

	void minionPhysics();

	void collisionWithPlayer(bool TOP);
};

#endif