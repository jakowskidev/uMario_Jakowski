#pragma once

#ifndef TOAD_H
#define TOAD_H

#include "Minion.h"

class Toad : public Minion
{
private:


public:
	Toad(int iXPos, int iYPos, bool peach);
	~Toad(void);

	void Update();
	void Draw(SDL_Renderer* rR, CIMG* iIMG);

	void minionPhysics();
	void collisionWithPlayer(bool TOP);

	void setMinionState(int minionState);
};

#endif