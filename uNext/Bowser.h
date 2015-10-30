#pragma once

#ifndef BOWSER_H
#define BOWSER_H

#include "Minion.h"

class Bowser : public Minion
{
private:
	// -- minionState, 0 = Normal, 1 = Shot, -1 = Dead

	int nextJumpFrameID;

	bool spawnHammer;
	int nextHammerFrameID, numOfHammers;

	int iMinBlockID, iMaxBlockID;
	bool moveDir;

	int nextFireFrameID;

	int iYFireStart;
	int iFireID;
public:
	Bowser(float fXPos, float fYPos, bool spawnHammer = false);
	~Bowser(void);

	void Update();
	void Draw(SDL_Renderer* rR, CIMG* iIMG);

	void minionPhysics();

	void collisionWithPlayer(bool TOP);

	void createFire();
};

#endif