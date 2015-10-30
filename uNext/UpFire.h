#pragma once

#ifndef UPFIRE_H
#define UPFIRE_H

#include "Minion.h"

class UpFire : public Minion
{
private:
	// moveDirection true = TOP, false = BOT
	int iYJump;
	int nextJumpFrameID;
public:
	UpFire(int iXPos, int iYJump);
	~UpFire(void);

	void Update();
	void Draw(SDL_Renderer* rR, CIMG* iIMG);

	void minionPhysics();

	void collisionWithPlayer(bool TOP);
};

#endif