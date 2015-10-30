#pragma once

#ifndef PLAYERFIREBALL_H
#define PLAYERFIREBALL_H

#include "Minion.h"

class PlayerFireBall : public Minion
{
private:
	bool bDestroy;
	int destroyFrameID;
public:
	PlayerFireBall(int X, int Y, bool moveDirection);
	~PlayerFireBall(void);

	void Update();
	void Draw(SDL_Renderer* rR, CIMG* iIMG);

	void updateXPos();

	void minionPhysics();

	void collisionWithPlayer(bool TOP);

	void collisionWithAnotherUnit();

	void collisionEffect();

	void setMinionState(int minionState);
};

#endif