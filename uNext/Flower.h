#pragma once

#ifndef FLOWER_H
#define FLOWER_H

#include "Minion.h"

class Flower : public Minion
{
private:
	bool inSpawnState;
	int inSpawnY;
	int iX, iY; // inSpawnState draw Block
public:
	Flower(int iXPos, int iYPos, int iX, int iY);
	~Flower(void);

	void Update();
	bool updateMinion();

	void Draw(SDL_Renderer* rR, CIMG* iIMG);

	void collisionWithPlayer(bool TOP);

	void setMinionState(int minionState);
};

#endif