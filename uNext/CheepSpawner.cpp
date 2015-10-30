#include "CheepSpawner.h"
#include "Core.h"
#include "stdlib.h"
#include "time.h"

/* ******************************************** */

CheepSpawner::CheepSpawner(int iXPos, int iXEnd) {
	this->fXPos = (float)iXPos;
	this->fYPos = 0;
	this->iXEnd = iXEnd;

	this->collisionOnlyWithPlayer = true;
	this->iHitBoxX = this->iHitBoxY = 1;

	this->minionSpawned = true;

	nextCheep();

	this->iBlockID = 0;

	srand((unsigned)time(NULL));
}

CheepSpawner::~CheepSpawner(void) {

}

/* ******************************************** */

void CheepSpawner::Update() {
	if(iSpawnTime < SDL_GetTicks()) {
		spawnCheep();
		nextCheep();
	}
}

void CheepSpawner::minionPhysics() { }

/* ******************************************** */

void CheepSpawner::spawnCheep() {
	if(CCore::getMap()->getPlayer()->getXPos() - CCore::getMap()->getXPos() > fXPos && CCore::getMap()->getPlayer()->getXPos() - CCore::getMap()->getXPos() < iXEnd) {
		CCore::getMap()->addCheep((int)(-CCore::getMap()->getXPos() + 64 + rand()%(CCFG::GAME_WIDTH - 128)), CCFG::GAME_HEIGHT - 4, 2, 2 + rand() % 32, !(rand()%4 == 0));
	} else {
		nextCheep();
	}
}

void CheepSpawner::nextCheep() {
	iSpawnTime = SDL_GetTicks() + 675 + rand()%1025;
}