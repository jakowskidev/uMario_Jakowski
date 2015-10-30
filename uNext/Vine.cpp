#include "Vine.h"
#include "Core.h"

/* ******************************************** */

Vine::Vine(int iXPos, int iYPos, int minionState, int iBlockID) {
	this->fXPos = (float)iXPos*32 + 4;
	this->fYPos = (float)(CCFG::GAME_HEIGHT - 16 - iYPos*32);

	this->iX = iXPos;
	this->iY = iYPos;

	this->minionState = minionState;
	this->iBlockID = iBlockID;

	this->moveSpeed = 0;

	this->collisionOnlyWithPlayer = true;

	this->jumpDistance = 0;
	this->minionSpawned = true;

	this->iHitBoxX = 24;
	this->iHitBoxY = -32;
}

Vine::~Vine(void) {

}

/* ******************************************** */

void Vine::Update() {
	if(minionState == 0) {
		if(CCFG::GAME_HEIGHT + 16 - iY * 32 >= jumpDistance) {
			jumpDistance += 2;
			iHitBoxY += 2;
			fYPos -= 2;
		}
	} else {
		if(jumpDistance < 256) {
			jumpDistance += 2;
			iHitBoxY += 2;
			fYPos -= 2;
		}
	}
}

void Vine::Draw(SDL_Renderer* rR, CIMG* iIMG) {
	if(jumpDistance < 32) {
		CCore::getMap()->getMinionBlock(iBlockID - 1)->Draw(rR, (int)(iX*32 + CCore::getMap()->getXPos()), (int)(CCFG::GAME_HEIGHT - 16 - iY*32 - jumpDistance));
	} else {
		CCore::getMap()->getMinionBlock(iBlockID - 1)->Draw(rR, (int)(iX*32 + CCore::getMap()->getXPos()), (int)(CCFG::GAME_HEIGHT - 16 - iY*32 - jumpDistance));
		for(int i = 0; i < jumpDistance/32 - 1; i++) {
			iIMG->Draw(rR, (int)(iX*32 + CCore::getMap()->getXPos()), (int)(CCFG::GAME_HEIGHT + 16 - iY*32 + i*32 - jumpDistance), false);
		}
	}
	CCore::getMap()->getBlock(CCore::getMap()->getMapBlock((int)iX, (int)iY)->getBlockID())->getSprite()->getTexture()->Draw(rR, (int)(iX*32 + CCore::getMap()->getXPos()), (int)(CCFG::GAME_HEIGHT - iY*32 - 16 - CCore::getMap()->getMapBlock((int)iX, (int)iY)->getYPos()));
}

void Vine::minionPhysics() { }

/* ******************************************** */

void Vine::collisionWithPlayer(bool TOP) {
	if(minionState == 0) {
		CCore::getMap()->setInEvent(true);

		CCore::getMap()->getEvent()->resetData();
		CCore::getMap()->getPlayer()->resetJump();
		CCore::getMap()->getPlayer()->stopMove();

		CCore::getMap()->getEvent()->eventTypeID = CCore::getMap()->getEvent()->eNormal;

		CCore::getMap()->getEvent()->iSpeed = 2;

		CCore::getMap()->getEvent()->newLevelType = CCore::getMap()->getLevelType();
		CCore::getMap()->getEvent()->newCurrentLevel = CCore::getMap()->getCurrentLevelID();
		CCore::getMap()->getEvent()->newMoveMap = true;
		CCore::getMap()->getEvent()->iDelay = 0;
		CCore::getMap()->getEvent()->inEvent = false;

		CCore::getMap()->getEvent()->newMoveMap = true;
		CCore::getMap()->getEvent()->newUnderWater = false;

		if(fXPos + iHitBoxX/2 > CCore::getMap()->getPlayer()->getXPos() - CCore::getMap()->getXPos()) {
			CCore::getMap()->getEvent()->vOLDDir.push_back(CCore::getMap()->getEvent()->eRIGHT);
			CCore::getMap()->getEvent()->vOLDLength.push_back(CCore::getMap()->getPlayer()->getHitBoxX() - 4);
		} else {
			CCore::getMap()->getEvent()->vOLDDir.push_back(CCore::getMap()->getEvent()->eLEFT);
			CCore::getMap()->getEvent()->vOLDLength.push_back(CCore::getMap()->getPlayer()->getHitBoxX() - 4);
		}

		for(int i = 0; i < CCore::getMap()->getPlayer()->getYPos() + CCore::getMap()->getPlayer()->getHitBoxY(); i += 32) {
			CCore::getMap()->getEvent()->vOLDDir.push_back(CCore::getMap()->getEvent()->eVINE1);
			CCore::getMap()->getEvent()->vOLDLength.push_back(16);
			CCore::getMap()->getEvent()->vOLDDir.push_back(CCore::getMap()->getEvent()->eVINE2);
			CCore::getMap()->getEvent()->vOLDLength.push_back(16);
		}

		switch(CCore::getMap()->getCurrentLevelID()) {
			case 4: {
				CCore::getMap()->getEvent()->newMapXPos = -270*32;
				CCore::getMap()->getEvent()->newPlayerXPos = 128;
				CCore::getMap()->getEvent()->newPlayerYPos = CCFG::GAME_HEIGHT - CCore::getMap()->getPlayer()->getHitBoxY();
				CCore::getMap()->getEvent()->vOLDDir.push_back(CCore::getMap()->getEvent()->eVINESPAWN);
				CCore::getMap()->getEvent()->vOLDLength.push_back(274);

				break;
			}
			case 8: {
				CCore::getMap()->getEvent()->newMapXPos = -270*32;
				CCore::getMap()->getEvent()->newPlayerXPos = 128;
				CCore::getMap()->getEvent()->newPlayerYPos = CCFG::GAME_HEIGHT - CCore::getMap()->getPlayer()->getHitBoxY();
				CCore::getMap()->getEvent()->vOLDDir.push_back(CCore::getMap()->getEvent()->eVINESPAWN);
				CCore::getMap()->getEvent()->vOLDLength.push_back(274);

				break;
			}
			case 13: {
				CCore::getMap()->getEvent()->newMapXPos = -310*32;
				CCore::getMap()->getEvent()->newPlayerXPos = 128;
				CCore::getMap()->getEvent()->newPlayerYPos = CCFG::GAME_HEIGHT - CCore::getMap()->getPlayer()->getHitBoxY();
				CCore::getMap()->getEvent()->newLevelType = 0;

				CCore::getMap()->getEvent()->vOLDDir.push_back(CCore::getMap()->getEvent()->eVINESPAWN);
				CCore::getMap()->getEvent()->vOLDLength.push_back(314);
				break;
			}
			case 17: {
				CCore::getMap()->getEvent()->newMapXPos = -325*32;
				CCore::getMap()->getEvent()->newPlayerXPos = 128;
				CCore::getMap()->getEvent()->newPlayerYPos = CCFG::GAME_HEIGHT - CCore::getMap()->getPlayer()->getHitBoxY();
				CCore::getMap()->getEvent()->newLevelType = 0;

				CCore::getMap()->getEvent()->vOLDDir.push_back(CCore::getMap()->getEvent()->eVINESPAWN);
				CCore::getMap()->getEvent()->vOLDLength.push_back(329);
				break;
			}
			case 21: {
				CCore::getMap()->getEvent()->newMapXPos = -390*32;
				CCore::getMap()->getEvent()->newPlayerXPos = 128;
				CCore::getMap()->getEvent()->newPlayerYPos = CCFG::GAME_HEIGHT - CCore::getMap()->getPlayer()->getHitBoxY();
				CCore::getMap()->getEvent()->newLevelType = 4;

				CCore::getMap()->getEvent()->vOLDDir.push_back(CCore::getMap()->getEvent()->eVINESPAWN);
				CCore::getMap()->getEvent()->vOLDLength.push_back(394);
				break;
			}
		}

		CCore::getMap()->getEvent()->vOLDDir.push_back(CCore::getMap()->getEvent()->eNOTHING);
		CCore::getMap()->getEvent()->vOLDLength.push_back(60);

		for(int i = 0; i < 64; i += 32) {
			CCore::getMap()->getEvent()->vNEWDir.push_back(CCore::getMap()->getEvent()->eVINE1);
			CCore::getMap()->getEvent()->vNEWLength.push_back(16);
			CCore::getMap()->getEvent()->vNEWDir.push_back(CCore::getMap()->getEvent()->eVINE2);
			CCore::getMap()->getEvent()->vNEWLength.push_back(16);
		}
		
	} else {

	}
}