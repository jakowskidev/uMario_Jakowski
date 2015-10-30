#pragma once

#ifndef KOPPA_H
#define KOPPA_H

#include "Minion.h"

class Koppa : public Minion
{
private:
	// ----- MinionState, 0 & 3 = Alive with wings, 1 = Alive, 2 = Dead, 4 = Dead animation, -1 = Destroy

	int iDistance;
	bool flyDirection; // -- minionState 3
public:
	Koppa(int iX, int iY, int minionState, bool moveDirection, int iBlockID);
	~Koppa(void);

	void Update();
	void Draw(SDL_Renderer* rR, CIMG* iIMG);

	void minionPhysics();
	void updateXPos();
	
	void collisionWithPlayer(bool TOP);
	void collisionEffect();

	void setMinionState(int minionState);

	void setMinion();
};

#endif