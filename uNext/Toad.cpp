#include "Toad.h"
#include "Core.h"

/* ******************************************** */

Toad::Toad(int iXPos, int iYPos, bool peach) {
	this->fXPos = (float)iXPos;
	this->fYPos = (float)iYPos;

	this->moveSpeed = 0;
	this->collisionOnlyWithPlayer = true;

	this->minionSpawned = true;
	this->iBlockID = peach ? 27 : 26;

	this->iHitBoxX = 32;
	this->iHitBoxY = 48;
}

Toad::~Toad(void) {

}

/* ******************************************** */

void Toad::Update() { }
void Toad::minionPhysics() { }

void Toad::Draw(SDL_Renderer* rR, CIMG* iIMG) {
	iIMG->Draw(rR,(int)(fXPos + CCore::getMap()->getXPos()), (int)fYPos, !moveDirection);
}

/* ******************************************** */

void Toad::collisionWithPlayer(bool TOP) { }

void Toad::setMinionState(int minionState) { }