#pragma once

#ifndef BEETLE_H
#define BEETLE_H

#include "Minion.h"

class Beetle : public Minion
{
private:


public:
	Beetle(int iXPos, int iYPos, bool moveDirection);
	~Beetle(void);

	void Update();
	void Draw(SDL_Renderer* rR, CIMG* iIMG);

	void collisionWithPlayer(bool TOP);

	void collisionEffect();

	void setMinionState(int minionState);
};

#endif