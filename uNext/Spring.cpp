#include "Spring.h"
#include "Core.h"

/* ******************************************** */

Spring::Spring(int iXPos, int iYPos) {
	this->fXPos = (float)iXPos;
	this->fYPos = (float)iYPos + 2;

	this->iHitBoxX = 32;
	this->iHitBoxY = 72;

	this->iBlockID = CCore::getMap()->getLevelType() == 0 || CCore::getMap()->getLevelType() == 4 ? 37 : 40;

	this->minionState = 0;
	this->nextFrameID = 4;
	this->inAnimation = false;
}

Spring::~Spring(void) {

}

/* ******************************************** */

void Spring::Update() {
	if(inAnimation) {
		if(CCFG::keySpace) {
			extraJump = true;
			CCore::getMap()->getPlayer()->resetJump();
			CCore::getMap()->getPlayer()->setNextFallFrameID(16);
		} else {
			CCore::getMap()->getPlayer()->stopMove();
		}

		CCore::getMap()->getPlayer()->setMarioSpriteID(5);
		if(nextFrameID <= 0) {
			switch(minionState) {
				case 0:
					iBlockID = iBlockID == 37 ? 38 : 41;
					minionState = 1;
					CCore::getMap()->getPlayer()->setYPos(CCore::getMap()->getPlayer()->getYPos() + 16.0f);
					break;
				case 1:
					iBlockID = iBlockID == 38 ? 39 : 42;
					minionState = 2;
					CCore::getMap()->getPlayer()->setYPos(CCore::getMap()->getPlayer()->getYPos() + 16.0f);
					break;
				case 2:
					iBlockID = iBlockID == 39 ? 38 : 41;
					minionState = 3;
					CCore::getMap()->getPlayer()->setYPos(CCore::getMap()->getPlayer()->getYPos() - 16.0f);
					break;
				case 3:
					iBlockID = iBlockID == 38 ? 37 : 40;
					minionState = 0;
					CCore::getMap()->getPlayer()->setYPos(CCore::getMap()->getPlayer()->getYPos() - 16.0f);
					CCore::getMap()->getPlayer()->resetJump();
					CCore::getMap()->getPlayer()->startJump(4 + (extraJump ? 5 : 0));
					CCore::getMap()->getPlayer()->setSpringJump(true);
					CCore::getMap()->getPlayer()->startMove();
					if(extraJump) CCore::getMap()->getPlayer()->setCurrentJumpSpeed(10.5f);
					inAnimation = false;
					break;
			}
			nextFrameID = 4;
		} else {
			--nextFrameID;
		}
	}
}

void Spring::Draw(SDL_Renderer* rR, CIMG* iIMG) {
	iIMG->Draw(rR, (int)fXPos + (int)CCore::getMap()->getXPos(), (int)fYPos, false);
}

void Spring::minionPhysics() { }

/* ******************************************** */

void Spring::collisionWithPlayer(bool TOP) {
	if(!inAnimation) {
		if(TOP && CCore::getMap()->getPlayer()->getJumpState() == 2) {
			CCore::getMap()->getPlayer()->stopMove();
			CCore::getMap()->getPlayer()->resetJump();
			CCore::getMap()->getPlayer()->setNextFallFrameID(16);
			inAnimation = true;
			extraJump = false;
			CCFG::keySpace = false;
		} else {
			if(CCore::getMap()->getPlayer()->getMoveDirection()) {
				CCore::getMap()->getPlayer()->setXPos((float)CCore::getMap()->getPlayer()->getXPos() - CCore::getMap()->getPlayer()->getMoveSpeed());
			} else {
				CCore::getMap()->getPlayer()->setXPos((float)CCore::getMap()->getPlayer()->getXPos() + CCore::getMap()->getPlayer()->getMoveSpeed());
			}
		}
	}
}

void Spring::setMinionState(int minionState) { }