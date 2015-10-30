#include "BulletBillSpawner.h"
#include "Core.h"
#include "stdlib.h"
#include "time.h"

/* ******************************************** */

BulletBillSpawner::BulletBillSpawner(int iXPos, int iYPos, int minionState) {
	this->fXPos = (float)iXPos;
	this->fYPos = (float)iYPos;

	this->moveSpeed = 0;

	this->collisionOnlyWithPlayer = true;

	this->minionState = minionState;

	this->iBlockID = 0;
	this->minionSpawned = minionState != 0;

	srand((unsigned)time(NULL));

	this->nextBulletBillFrameID = 12;
}

BulletBillSpawner::~BulletBillSpawner(void) {

}

/* ******************************************** */

void BulletBillSpawner::Update() {
	if(nextBulletBillFrameID <= 0) {
		if(minionState == 0) {
			if(fXPos > -CCore::getMap()->getXPos() - 64 && fXPos < -CCore::getMap()->getXPos() + CCFG::GAME_WIDTH + 64 + iHitBoxX) {
				if(!(CCore::getMap()->getPlayer()->getXPos() - CCore::getMap()->getXPos() + CCore::getMap()->getPlayer()->getHitBoxX()/2 > fXPos - 96 && CCore::getMap()->getPlayer()->getXPos() - CCore::getMap()->getXPos() + CCore::getMap()->getPlayer()->getHitBoxX()/2 < fXPos + 96)) {
					CCore::getMap()->addBulletBill((int)fXPos, (int)fYPos - 14, true, minionState);
					nextBulletBillFrameID = 145 + rand()%145;
				}
			}
		} else {
			CCore::getMap()->addBulletBill((int)(-CCore::getMap()->getXPos() + CCFG::GAME_WIDTH + iHitBoxX * 2), (int)fYPos - rand()%9*32 - 16, true, minionState);
			nextBulletBillFrameID = 80 + rand()%96;
		}
	} else {
		if(!(CCore::getMap()->getPlayer()->getXPos() - CCore::getMap()->getXPos() + CCore::getMap()->getPlayer()->getHitBoxX()/2 > fXPos - 96 && CCore::getMap()->getPlayer()->getXPos() - CCore::getMap()->getXPos() + CCore::getMap()->getPlayer()->getHitBoxX()/2 < fXPos + 96)) {
			--nextBulletBillFrameID;
		}
	}
}

void BulletBillSpawner::minionPhysics() { }

/* ******************************************** */