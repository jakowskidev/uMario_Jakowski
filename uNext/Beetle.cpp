#include "Beetle.h"
#include "Core.h"

/* ******************************************** */

Beetle::Beetle(int iXPos, int iYPos, bool moveDirection) {
	this->fXPos = (float)iXPos;
	this->fYPos = (float)iYPos;

	this->minionState = 0;
	this->iBlockID = CCore::getMap()->getLevelType() == 0 || CCore::getMap()->getLevelType() == 4 ? 53 : CCore::getMap()->getLevelType() == 1 ? 55 : 57;
	this->moveDirection = moveDirection;
	this->moveSpeed = 1;
}

Beetle::~Beetle(void) {

}

/* ******************************************** */

void Beetle::Update() {
	if(minionState == -2) {
		Minion::minionDeathAnimation();
	} else {
		updateXPos();
	}
}

void Beetle::Draw(SDL_Renderer* rR, CIMG* iIMG) {
	if(minionState != -2) {
		iIMG->Draw(rR, (int)fXPos + (int)CCore::getMap()->getXPos(), (int)fYPos, !moveDirection);
	} else {
		iIMG->DrawVert(rR, (int)fXPos + (int)CCore::getMap()->getXPos(), (int)fYPos);
	}
}

/* ******************************************** */

void Beetle::collisionWithPlayer(bool TOP) {
	if(CCore::getMap()->getPlayer()->getStarEffect()) {
		setMinionState(-2);
	} else if(TOP) {
		if(minionState == 0) {
			minionState = 1;

			++iBlockID;
			CCore::getMap()->getPlayer()->resetJump();
			CCore::getMap()->getPlayer()->startJump(1);
			CCore::getMap()->getPlayer()->setYPos((float)CCore::getMap()->getPlayer()->getYPos() - 4);
			points(100);
			CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cSTOMP);
			moveSpeed = 0;
			killOtherUnits = true;
		} else {
			if(moveSpeed > 0) {
				moveSpeed = 0;
			} else {
				if((fXPos + iHitBoxX) / 2 < (CCore::getMap()->getPlayer()->getXPos() - CCore::getMap()->getXPos() + CCore::getMap()->getPlayer()->getHitBoxX()) / 2) {
					moveDirection = true;
				} else {
					moveDirection = false;
				}

				moveSpeed = 6;
			}

			CCore::getMap()->getPlayer()->setYPos((float)CCore::getMap()->getPlayer()->getYPos() - 4);
			CCore::getMap()->getPlayer()->resetJump();
			CCore::getMap()->getPlayer()->startJump(1);
			points(100);
			CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cSTOMP);
		}
	} else {
		if(minionState == 1) {
			if(moveSpeed == 0) {
				//moveDirection = !CCore::getMap()->getPlayer()->getMoveDirection();
				moveDirection = (fXPos + iHitBoxX/2 < CCore::getMap()->getPlayer()->getXPos() - CCore::getMap()->getXPos() + CCore::getMap()->getPlayer()->getHitBoxX()/2);
				if(moveDirection) fXPos -= CCore::getMap()->getPlayer()->getMoveSpeed() + 1;
				else fXPos += CCore::getMap()->getPlayer()->getMoveSpeed() + 1;
				moveSpeed = 6;
				CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cSTOMP);
			} else {
				CCore::getMap()->playerDeath(true, false);
			}
		} else {
			CCore::getMap()->playerDeath(true, false);
		}
	}
}

void Beetle::collisionEffect() {
	if(minionState == 0) moveDirection = !moveDirection;
}

void Beetle::setMinionState(int minionState) {
	if(minionState != -2 || CCore::getMap()->getPlayer()->getStarEffect()) {
		Minion::setMinionState(minionState);
	} else {
		moveDirection = !moveDirection;
	}
}