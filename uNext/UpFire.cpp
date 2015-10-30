#include "UpFire.h"
#include "Core.h"
#include "stdlib.h"
#include "time.h"

/* ******************************************** */

UpFire::UpFire(int iXPos, int iYJump) {
	this->fXPos = (float)iXPos;
	this->fYPos = (float)CCFG::GAME_HEIGHT + 16;

	this->moveDirection = true;

	this->nextJumpFrameID = 0;
	this->collisionOnlyWithPlayer = true;

	this->iYJump = iYJump + 16;
	this->jumpDistance = (float)iYJump + 16;

	this->iBlockID = 32;

	srand((unsigned)time(NULL));
}

UpFire::~UpFire(void) {

}

/* ******************************************** */

void UpFire::Update() {
	if(nextJumpFrameID <= 0) {
		if(moveDirection) {
			if(jumpDistance < 32) {
				fYPos -= 2;
				jumpDistance -= 2;
			} else if(jumpDistance < 2*32) {
				fYPos -= 4;
				jumpDistance -= 4;
			} else if(jumpDistance < 4*32) {
				fYPos -= 5;
				jumpDistance -= 5;
			} else {
				fYPos -= 6;
				jumpDistance -= 6;
			}

			if(jumpDistance <= 0) {
				moveDirection = !moveDirection;
				jumpDistance = (float)iYJump;
			}
		} else {
			if(jumpDistance > iYJump - 32) {
				fYPos += 2;
				jumpDistance -= 2;
			} else if(jumpDistance > iYJump - 2*32) {
				fYPos += 4;
				jumpDistance -= 4;
			} else if(jumpDistance > iYJump - 4*32) {
				fYPos += 5;
				jumpDistance -= 5;
			}  else {
				fYPos += 6;
				jumpDistance -= 6;
			}

			if(jumpDistance <= 0) {
				moveDirection = !moveDirection;
				jumpDistance = (float)iYJump;
				nextJumpFrameID = 25 + rand()%355;
			}
		}
	} else {
		--nextJumpFrameID;
	}
}

void UpFire::Draw(SDL_Renderer* rR, CIMG* iIMG) {
	if(moveDirection) {
		iIMG->Draw(rR, (int)fXPos + (int)CCore::getMap()->getXPos(), (int)fYPos, false);
	} else {
		iIMG->DrawVert(rR, (int)fXPos + (int)CCore::getMap()->getXPos(), (int)fYPos);
	}
}

void UpFire::minionPhysics() { }

/* ******************************************** */

void UpFire::collisionWithPlayer(bool TOP) {
	CCore::getMap()->playerDeath(true, false);
}