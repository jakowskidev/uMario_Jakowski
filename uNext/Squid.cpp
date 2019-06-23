#include "Squid.h"
#include "Core.h"
#include "stdlib.h"
#include "time.h"

/* ******************************************** */

Squid::Squid(int iXPos, int iYPos) {
	this->fXPos = (float)iXPos;
	this->fYPos = (float)iYPos;

	this->iHitBoxX = 32;
	this->iBlockID = 29;

	this->minionState = 0;

	this->moveDirection = false;
	this->moveSpeed = 2;

	this->moveXDistance = 96;
	this->moveYDistance = 32;

	this->collisionOnlyWithPlayer = false;

	srand((unsigned)time(NULL));
}

Squid::~Squid(void) {

}

/* ******************************************** */

void Squid::Update() {
	if(CCore::getMap()->getUnderWater()) {
		if(minionState != -2){
		if(moveXDistance <= 0) {
			if(moveYDistance > 0) {
				fYPos += 1;
				moveYDistance -= 1;
				if(moveYDistance == 0) {
					changeBlockID();
				}
			} else {
				if(fYPos + 52 > CCore::getMap()->getPlayer()->getYPos()) {
					moveDirection = CCore::getMap()->getPlayer()->getXPos() - CCore::getMap()->getXPos() + CCore::getMap()->getPlayer()->getHitBoxX()/2 > fXPos;
					moveXDistance = 96 + rand()%32;
					changeBlockID();
				} else {
					fYPos += 1;
				}
			}
		} else {
			if(moveXDistance == 64) changeBlockID();
			fXPos += moveDirection ? 2 : -2;

			if(fYPos > CCFG::GAME_HEIGHT - 12*32 - 4) {
				fYPos -= 2;
			}
			moveXDistance -= 2;
			if(moveXDistance <= 0) {
				changeBlockID();
				moveYDistance = 32;
			}


			}
		}
		else {
			Minion::minionDeathAnimation();
		}
	}
}

void Squid::Draw(SDL_Renderer* rR, CIMG* iIMG) {
	if(minionState != -2)
	{
		iIMG->Draw(rR,(int)(fXPos + CCore::getMap()->getXPos()), (int)fYPos);

	}
	else
	{
		iIMG->DrawVert(rR, (int)fXPos + (int)CCore::getMap()->getXPos(), (int)fYPos + 2);
	}
}

void Squid::minionPhysics() { }

/* ******************************************** */

void Squid::collisionWithPlayer(bool TOP) {

	if(CCore::getMap()->getPlayer()->getStarEffect()) {

		setMinionState(-2);
	}

	CCore::getMap()->playerDeath(true, false);
}

void Squid::changeBlockID() {
	switch(iBlockID) {
		case 28:
			this->iBlockID = 29;
			this->iHitBoxY = 28;
			break;
		default:
			this->iBlockID = 28;
			this->iHitBoxY = 28;
			break;
	}
}

void Squid::setMinionState(int minionState) {
	this->minionState = minionState;

	if (this->minionState == 1) {
		deadTime = SDL_GetTicks();
	}

	Minion::setMinionState(minionState);
}
