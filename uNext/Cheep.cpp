#include "Cheep.h"
#include "Core.h"
#include "stdlib.h"
#include "time.h"

/* ******************************************** */

Cheep::Cheep(int iXPos, int iYPos, int minionType, int moveSpeed, bool moveDirection) {
	srand((unsigned)time(NULL));

	this->fXPos = (float)iXPos;
	this->fYPos = (float)iYPos + rand() % 16;

	this->moveY = false;
	this->moveDirection = false;

	this->iHitBoxX = 30;
	this->iHitBoxY = 28;

	if(minionType == 1) {
		this->minionState = 1;
		this->moveSpeed = 2;
		this->iBlockID = 31;
	} else if(minionType == 0) {
		this->minionState = 0;
		this->moveSpeed = 1;
		this->iBlockID = 30;
	} else {
		this->minionState = minionType;
		this->moveSpeed = moveSpeed;
		this->iBlockID = 31;

		this->moveDirection = moveDirection;

		this->iHitBoxY = 20;

		this->jumpDistance = 11*32.0f + rand()%80;
		moveYDIR = true;
	}
	
	this->collisionOnlyWithPlayer = true;
}

Cheep::~Cheep(void) {

}

/* ******************************************** */

void Cheep::Update() {
	if(minionState == 0 || minionState == 1) {
		if(fXPos + iHitBoxX < 0) {
			minionState = -1;
		} else {
			fXPos -= moveSpeed / 2.0f;
		}

		if(moveY) {
			if(moveYDIR) {
				if(jumpDistance > 0) {
					if(fYPos > CCFG::GAME_HEIGHT - 12*32 + 8) {
						fYPos -= 0.25f;
					}
					--jumpDistance;
				} else {
					jumpDistance = 80 * 4.0f + (rand()%32)*4;
					moveYDIR = !moveYDIR;
				}
			} else {
				if(jumpDistance > 0) {
					fYPos += 0.25f;
					--jumpDistance;
				} else {
					jumpDistance = 80 * 4.0f + (rand()%32)*4;
					moveYDIR = !moveYDIR;
				}
			}
		} else {
			moveY = rand()%218 == 0;

			if(moveY) {
				jumpDistance = (float)(rand()%96);
				moveYDIR = rand()%2 == 0;
			}
		}
	} else { // -- CHEEP SPAWNED
		if(fXPos + iHitBoxX < 0) {
			minionState = -1;
		} else {
			fXPos += moveDirection ? moveSpeed / 4.0f : -moveSpeed / 4.0f;
		}

		if(moveYDIR) {
			if(jumpDistance > 10*32) {
				fYPos -= 8;
				jumpDistance -= 8;
			} else if(jumpDistance > 9*32) {
				fYPos -= 7.5f;
				jumpDistance -= 7.5f;
			} else if(jumpDistance > 8*32) {
				fYPos -= 7;
				jumpDistance -= 7;
			} else if(jumpDistance > 6*32) {
				fYPos -= 6.5f;
				jumpDistance -= 6.5f;
			} else if(jumpDistance > 5*32) {
				fYPos -= 5.5f;
				jumpDistance -= 5.5f;
			} else if(jumpDistance > 4*32) {
				fYPos -= 4.5f;
				jumpDistance -= 4.5f;
			} else if(jumpDistance > 2*32) {
				fYPos -= 3.5f;
				jumpDistance -= 3.5f;
			} else if(jumpDistance > 0) {
				fYPos -= 2.5f;
				jumpDistance -= 2.5f;
			} else {
				moveYDIR = !moveYDIR;
				this->jumpDistance = 13*32;
			}
		} else {
			if(jumpDistance > 11*32) {
				fYPos += 2.5f;
				jumpDistance -= 2.5f;
			} else if(jumpDistance > 9*32) {
				fYPos += 3.5f;
				jumpDistance -= 3.5f;
			} else if(jumpDistance > 8*32) {
				fYPos += 4.5f;
				jumpDistance -= 4.5f;
			} else if(jumpDistance > 7*32) {
				fYPos += 5.5f;
				jumpDistance -= 5.5f;
			} else if(jumpDistance > 6*32) {
				fYPos += 6.5f;
				jumpDistance -= 6.5f;
			} else if(jumpDistance > 4*32) {
				fYPos += 7;
				jumpDistance -= 7;
			} else if(jumpDistance > 3*32) {
				fYPos += 7.5f;
				jumpDistance -= 7.5f;
			} else {
				fYPos += 8;
				jumpDistance -= 8;
			}

			if(fYPos > CCFG::GAME_HEIGHT) {
				minionState = -1;
			}
		}
	}
}

void Cheep::Draw(SDL_Renderer* rR, CIMG* iIMG) {
	if(minionState == -2) {
		iIMG->DrawVert(rR, (int)fXPos + (int)CCore::getMap()->getXPos(), (int)fYPos + 2);
	} else {
		iIMG->Draw(rR,(int)(fXPos + CCore::getMap()->getXPos()), (int)fYPos, moveDirection);
	}
}

/* ******************************************** */

void Cheep::minionPhysics() { }

void Cheep::collisionWithPlayer(bool TOP) {
	if(minionState > 1 && TOP) {
		setMinionState(-2);
		this->minionState = -2;
		moveYDIR = false;
		this->jumpDistance = CCFG::GAME_HEIGHT - fYPos;
	} else {
		CCore::getMap()->playerDeath(true, false);
	}
}