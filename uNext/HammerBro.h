#pragma once

#ifndef HAMMERBRO_H
#define HAMMERBRO_H

#include "Minion.h"

class HammerBro : public Minion
{
private:
	bool moveDIR;
	float moveDistance;

	int nextJumpFrameID;
	bool newY; // true = UP, false = DOWN

	int hammerID;
	int nextHammerFrameID;
public:
	HammerBro(int iXPos, int iYPos);
	~HammerBro(void);

	void Update();
	void Draw(SDL_Renderer* rR, CIMG* iIMG);

	void minionPhysics();

	void collisionWithPlayer(bool TOP);

	void collisionEffect();
};

#endif