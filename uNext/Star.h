#pragma once

#ifndef STAR_H
#define STAR_H

#include "Minion.h"

class Star : public Minion
{
private:
	bool inSpawnState;
	int inSpawnY;
	int iX, iY; // inSpawnState draw Block
public:
	Star(int iXPos, int iYPos, int iX, int iY);
	~Star(void);

	void Update();
	bool updateMinion();

	void minionPhysics();

	void Draw(SDL_Renderer* rR, CIMG* iIMG);

	void collisionWithPlayer(bool TOP);

	void setMinionState(int minionState);
};

#endif