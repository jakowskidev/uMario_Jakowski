#pragma once

#ifndef PLANT_H
#define PLANT_H

#include "Minion.h"

class Plant : public Minion
{
private:
	bool bWait;
	unsigned int iTime;

	bool lockPlant;

	int iLength;

	int X, Y;

public:
	Plant(int fXPos, int fYPos, int iBlockID);
	~Plant(void);

	void Update();
	void Draw(SDL_Renderer* rR, CIMG* iIMG);

	void minionPhysics();

	void collisionWithPlayer(bool TOP);

	void collisionEffect();

	void lockMinion();

	void setMinionState(int minionState);
};

#endif