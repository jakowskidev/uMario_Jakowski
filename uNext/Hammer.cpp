#include "Hammer.h"
#include "Core.h"

/* ******************************************** */

Hammer::Hammer(int iXPos, int iYPos, bool moveDirection) {
	this->fXPos = (float)iXPos + 4;
	this->fYPos = (float)iYPos + 4;

	this->iBlockID = CCore::getMap()->getLevelType() == 0 || CCore::getMap()->getLevelType() == 4 ? 47 : 48;

	this->iHitBoxX = 24;
	this->iHitBoxY = 24;

	this->moveDirection = moveDirection;
	this->jumpState = 1;
	this->jumpDistance = 48;
	this->currentJumpDistance = 0;
	this->moveSpeed = 0;
	this->minionState = 0;

	this->collisionOnlyWithPlayer = true;

	this->minionSpawned = true;
}

Hammer::~Hammer(void) {

}

/* ******************************************** */

void Hammer::Update() {
	if(minionState != -2) {
		++moveSpeed;
		if(moveSpeed > 35) {
			fXPos += moveDirection ? 3 : -3;
		} else if(moveSpeed > 15) {
			fXPos += moveDirection ? 2.5f : -2.5f;
		} else {
			fXPos += moveDirection ? 2 : -2;
		}
	} else {
		Minion::minionDeathAnimation();
	}
}

void Hammer::Draw(SDL_Renderer* rR, CIMG* iIMG) {
	if(minionState != -2) {
		iIMG->Draw(rR, (int)fXPos + (int)CCore::getMap()->getXPos() - 4, (int)fYPos - 4, false);
	} else {
		iIMG->DrawVert(rR, (int)fXPos + (int)CCore::getMap()->getXPos() - 4, (int)fYPos - 4);
	}
}

void Hammer::minionPhysics() {
	if (jumpState == 1) {
		Minion::physicsState1();
	} else {
		currentFallingSpeed *= 1.06f;

		if (currentFallingSpeed > startJumpSpeed) {
			currentFallingSpeed = startJumpSpeed;
		}

		fYPos += currentFallingSpeed;

		if (fYPos >= CCFG::GAME_HEIGHT) {
			minionState = -1;
		}
	}
}

/* ******************************************** */

void Hammer::collisionWithPlayer(bool TOP) {
	if(CCore::getMap()->getPlayer()->getStarEffect()) {
		setMinionState(-2);
	} else {
		CCore::getMap()->playerDeath(true, false);
	}
}