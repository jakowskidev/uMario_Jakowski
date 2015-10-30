#pragma once

#ifndef BULLETBILLSPAWNER_H
#define BULLETBULLSPAWNER_H

#include "Minion.h"

class BulletBillSpawner : public Minion
{
private:
	int nextBulletBillFrameID;
public:
	BulletBillSpawner(int iXPos, int iYPos, int minionState);
	~BulletBillSpawner(void);

	void Update();

	void minionPhysics();
};

#endif