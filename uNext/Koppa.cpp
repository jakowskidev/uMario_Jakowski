#include "Koppa.h"
#include "Core.h"

/* ******************************************** */

Koppa::Koppa(int iX, int iY, int minionState, bool moveDirection, int iBlockID) {
	this->fXPos = (float)iX;
	this->fYPos = (float)iY;
	this->minionState = minionState;

	this->moveDirection = moveDirection;
	this->flyDirection = moveDirection;

	this->moveSpeed = 1;

	this->iBlockID = iBlockID;

	switch(minionState) {
		case 0:
			this->iHitBoxX = 32;
			this->iHitBoxY = 32;
			break;
		case 1:
			this->iHitBoxX = 32;
			this->iHitBoxY = 32;
			break;
		case 2:
			this->iHitBoxX = 32;
			this->iHitBoxY = 28;
			this->moveSpeed = 0;
			this->killOtherUnits = true;
			break;
		case 3:
			this->iHitBoxX = 32;
			this->iHitBoxY = 32;
			this->moveDirection = true;
			break;
	}

	this->iDistance = 7*32;
}

Koppa::~Koppa(void) {

}

/* ******************************************** */

void Koppa::minionPhysics() {
	if(minionState == 3) {
		if(flyDirection) {
			if(iDistance > 0) {
				fYPos -= 2;
				iDistance -= 2;
			} else {
				iDistance = 7*32;
				flyDirection = !flyDirection;
			}
		} else {
			if(iDistance > 0) {
				fYPos += 2;
				iDistance -= 2;
			} else {
				iDistance = 7*32;
				flyDirection = !flyDirection;
			}
		}
	}
	else if (jumpState == 1) {
		if(minionState == 0) {
			updateYPos(-2);
			currentJumpDistance += 2;

			if (jumpDistance <= currentJumpDistance) {
				jumpState = 2;
			}
		} else {
			Minion::physicsState1();
		}
	} else {
		if (!CCore::getMap()->checkCollisionLB((int)fXPos + 2, (int)fYPos + 2, iHitBoxY, true) && !CCore::getMap()->checkCollisionRB((int)fXPos - 2, (int)fYPos + 2, iHitBoxX, iHitBoxY, true) && !onAnotherMinion) {
			if(minionState == 0) {
				updateYPos(2);

				jumpState = 2;

				if (fYPos >= CCFG::GAME_HEIGHT) {
					minionState = -1;
				}
			} else {
				Minion::physicsState2();
			}
		} else {
			jumpState = 0;
			onAnotherMinion = false;
		}
	}
}

void Koppa::Update() {
	if (minionState == 0) {
		updateXPos();
		if(jumpState == 0) {
			startJump(1);
		}
	} else if(minionState == 1) {
		updateXPos();
	} else if(minionState == 2) {
		updateXPos();
	} else if(minionState == -2) {
		Minion::minionDeathAnimation();
	} else if (minionState != 3 && SDL_GetTicks() - 500 >= (unsigned)deadTime) {
		minionState = -1;
	}
}

void Koppa::Draw(SDL_Renderer* rR, CIMG* iIMG) {
	if(minionState != -2) {
		iIMG->Draw(rR, (int)fXPos + (int)CCore::getMap()->getXPos(), (int)fYPos + (minionState <= 1 ? -14 : 2), !moveDirection);
	} else {
		iIMG->DrawVert(rR, (int)fXPos + (int)CCore::getMap()->getXPos(), (int)fYPos + 2);
	}
}

void Koppa::updateXPos() {
	// ----- LEFT
	if (moveDirection) {
		if (CCore::getMap()->checkCollisionLB((int)fXPos - moveSpeed, (int)fYPos - 2, iHitBoxY, true) || CCore::getMap()->checkCollisionLT((int)fXPos - moveSpeed, (int)fYPos + 2, true)) {
			moveDirection = !moveDirection;
		} else {
			bool LB = CCore::getMap()->checkCollisionLB((int)fXPos + iHitBoxX/2, (int)fYPos, iHitBoxY + 2, false), RB = CCore::getMap()->checkCollisionRB((int)fXPos + iHitBoxX/2, (int)fYPos, iHitBoxX, iHitBoxY + 2, false);
			if((minionState == 1 && iBlockID != 4) && ((!LB && RB))) {
				moveDirection = !moveDirection;
				fXPos += moveSpeed;
			} else {
				fXPos -= moveSpeed;
			}
		}
	}
	// ----- RIGHT
	else {
		if (CCore::getMap()->checkCollisionRB((int)fXPos + moveSpeed, (int)fYPos - 2, iHitBoxX, iHitBoxY, true) || CCore::getMap()->checkCollisionRT((int)fXPos + moveSpeed, (int)fYPos + 2, iHitBoxX, true)) {
			moveDirection = !moveDirection;
		} else {
			bool LB = CCore::getMap()->checkCollisionLB((int)fXPos - iHitBoxX/2, (int)fYPos, iHitBoxY + 2, false), RB = CCore::getMap()->checkCollisionRB((int)fXPos - iHitBoxX/2, (int)fYPos, iHitBoxX, iHitBoxY + 2, false);
			if((minionState == 1 && iBlockID != 4) && ((LB && !RB))) {
				moveDirection = !moveDirection;
				fXPos -= moveSpeed;
			} else {
				fXPos += moveSpeed;
			}
		}
	}

	if(fXPos < -iHitBoxX) {
		minionState = -1;
	}
}

/* ******************************************** */

void Koppa::collisionWithPlayer(bool TOP) {
	if(CCore::getMap()->getPlayer()->getStarEffect()) {
		setMinionState(-2);
	} else if(TOP) {
		if(minionState == 0 || minionState == 3) {
			minionState = 1;
			setMinion();
			CCore::getMap()->getPlayer()->resetJump();
			CCore::getMap()->getPlayer()->startJump(1);
			CCore::getMap()->getPlayer()->setYPos((float)CCore::getMap()->getPlayer()->getYPos() - 4);
			points(100);
			CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cSTOMP);
		} else if(minionState == 1) {
			minionState = 2;
			setMinion();
			CCore::getMap()->getPlayer()->resetJump();
			CCore::getMap()->getPlayer()->startJump(1);
			CCore::getMap()->getPlayer()->setYPos((float)CCore::getMap()->getPlayer()->getYPos() - 4);
			points(100);
			CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cSTOMP);
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
		if(minionState == 2) {
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

void Koppa::collisionEffect() {
	if(minionState != 2 && moveSpeed != 0) {
		moveDirection = !moveDirection;
	}
}

/* ******************************************** */

void Koppa::setMinionState(int minionState) {
	this->minionState = minionState;

	if (this->minionState == 3) {
		deadTime = SDL_GetTicks();
	}

	Minion::setMinionState(minionState);
}

/* ******************************************** */

void Koppa::setMinion() {
	switch(minionState) {
		case 0: case 3:
			this->iHitBoxX = 32;
			this->iHitBoxY = 32;
			break;
		case 1:
			this->iHitBoxX = 32;
			this->iHitBoxY = 32;
			break;
		case 2:
			this->iHitBoxX = 32;
			this->iHitBoxY = 28;
			this->moveSpeed = 0;
			this->killOtherUnits = true;
			break;
	}

	switch(iBlockID) {
		case 7:
			iBlockID = 4;
			break;
		case 14:
			iBlockID = 12;
			break;
		case 17:
			iBlockID = 15;
			break;
		case 4:
			iBlockID = 5;
			break;
		case 12:
			iBlockID = 13;
			break;
		case 15:
			iBlockID = 16;
			break;
	}
}