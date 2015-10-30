#pragma once

#ifndef HAMMER_H
#define HAMMER_H

#include "Minion.h"

class Hammer : public Minion
{
private:

public:
	Hammer(int iXPos, int iYPos, bool moveDirection);
	~Hammer(void);

	void Update();
	void Draw(SDL_Renderer* rR, CIMG* iIMG);

	void minionPhysics();

	void collisionWithPlayer(bool TOP);
};

#endif