#include "PlayerFireBall.h"
#include "Core.h"

/* ******************************************** */

PlayerFireBall::PlayerFireBall(int iXPos, int iYPos, bool moveDirection) {
	this->fXPos = (float)iXPos;
	this->fYPos = (float)iYPos;

	this->moveDirection = moveDirection;

	this->killOtherUnits = true;

	this->minionSpawned = true;

	this->moveSpeed = 14;

	this->iBlockID = 62;

	this->jumpState = 2;

	this->iHitBoxX = 16;
	this->iHitBoxY = 16;

	this->bDestroy = false;

	this->destroyFrameID = 15;
}

PlayerFireBall::~PlayerFireBall(void) {

}

/* ******************************************** */

void PlayerFireBall::Update() {
	if(bDestroy) {
		if(destroyFrameID > 10) {
			this->iBlockID = 63;
		} else if(destroyFrameID > 5) {
			this->iBlockID = 64;
		} else if(destroyFrameID > 0) {
			this->iBlockID = 65;
		} else {
			minionState = -1;
		}
		--destroyFrameID;
	} else {
		if(jumpState == 0) {
			jumpState = 1;
			currentJumpSpeed = startJumpSpeed;
			jumpDistance = 34.0f;
			currentJumpDistance = 0;
		} else if(jumpState == 1) {
			updateYPos(-1);
			currentJumpDistance += 1;

			if (jumpDistance <= currentJumpDistance) {
				jumpState = 2;
			}
		} else {
			if (!CCore::getMap()->checkCollisionLB((int)fXPos + 2, (int)fYPos + 2, iHitBoxY, true) && !CCore::getMap()->checkCollisionRB((int)fXPos - 2, (int)fYPos + 2, iHitBoxX, iHitBoxY, true) && !onAnotherMinion) {
				updateYPos(1);

				jumpState = 2;

				if (fYPos >= CCFG::GAME_HEIGHT) {
					minionState = -1;
				}
			} else {
				jumpState = 0;
				onAnotherMinion = false;
			}
		}

		updateXPos();
	}
}

void PlayerFireBall::Draw(SDL_Renderer* rR, CIMG* iIMG) {
	if(!bDestroy) {
		iIMG->Draw(rR, (int)fXPos + (int)CCore::getMap()->getXPos(), (int)fYPos, !moveDirection);
	} else {
		iIMG->Draw(rR, (int)fXPos + (int)CCore::getMap()->getXPos() - (moveDirection ? 16 : 0), (int)fYPos - 8, !moveDirection);
	}
}

void PlayerFireBall::updateXPos() {
	// ----- LEFT
	if (moveDirection) {
		if (CCore::getMap()->checkCollisionLB((int)fXPos - moveSpeed, (int)fYPos - 2, iHitBoxY, true) || CCore::getMap()->checkCollisionLT((int)fXPos - moveSpeed, (int)fYPos + 2, true)) {
			bDestroy = true;
			collisionOnlyWithPlayer = true;
			CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cBLOCKHIT);
		} else {
			fXPos -= (jumpState == 0 ? moveSpeed : moveSpeed/2.0f);
		}
	}
	// ----- RIGHT
	else {
		if (CCore::getMap()->checkCollisionRB((int)fXPos + moveSpeed, (int)fYPos - 2, iHitBoxX, iHitBoxY, true) || CCore::getMap()->checkCollisionRT((int)fXPos + moveSpeed, (int)fYPos + 2, iHitBoxX, true)) {
			bDestroy = true;
			collisionOnlyWithPlayer = true;
			CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cBLOCKHIT);
		} else {
			fXPos += (jumpState == 0 ? moveSpeed : moveSpeed/2.0f);
		}
	}

	if(fXPos < -iHitBoxX) {
		minionState = -1;
	}
}

void PlayerFireBall::minionPhysics() { 
	if(!bDestroy) {
		Minion::minionPhysics();
	}
}

/* ******************************************** */

void PlayerFireBall::collisionWithPlayer(bool TOP) { }

void PlayerFireBall::collisionWithAnotherUnit() {
	bDestroy = true;
	collisionOnlyWithPlayer = true;
}

void PlayerFireBall::collisionEffect() {

}

void PlayerFireBall::setMinionState(int minionState) {

}