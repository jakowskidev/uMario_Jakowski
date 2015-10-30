#include "Pipe.h"
#include "Core.h"

/* ******************************************** */

Pipe::Pipe(int iType, int iLX, int iLY, int iRX, int iRY, int newPlayerPosX, int newPlayerPosY, int newCurrentLevel, int newLevelType, bool newMoveMap, int iDelay, int iSpeed, bool newUnderWater) {
	this->iType = iType;
	
	this->iLX = iLX;
	this->iLY = iLY;
	this->iRX = iRX;
	this->iRY = iRY;

	this->newPlayerPosX = newPlayerPosX;
	this->newPlayerPosY = newPlayerPosY;

	this->newCurrentLevel = newCurrentLevel;
	this->newLevelType = newLevelType;
	this->newMoveMap = newMoveMap;
	this->newUnderWater = newUnderWater;

	this->iDelay = iDelay;

	this->iSpeed = iSpeed;
}


Pipe::~Pipe(void) {

}

/* ******************************************** */

void Pipe::checkUse() {
	if(iType == 0 || iType == 2) {
		if(CCore::getMap()->getPlayer()->getSquat() && -(int)CCore::getMap()->getXPos() + CCore::getMap()->getPlayer()->getXPos() >= iLX * 32 + 4 && -(int)CCore::getMap()->getXPos() + CCore::getMap()->getPlayer()->getXPos() + CCore::getMap()->getPlayer()->getHitBoxX() < (iRX + 1) * 32 - 4) {
			setEvent();
		}
	} else {
		if(!CCore::getMap()->getPlayer()->getSquat() && CCore::getMap()->getBlockIDX(-(int)CCore::getMap()->getXPos() + CCore::getMap()->getPlayer()->getXPos() + CCore::getMap()->getPlayer()->getHitBoxX() / 2 + 2) == iRX - 1 && CCore::getMap()->getBlockIDY(CCore::getMap()->getPlayer()->getYPos() + CCore::getMap()->getPlayer()->getHitBoxY() + 2) == iRY - 1) {
			setEvent();
		}
	}
}

/* ******************************************** */

void Pipe::setEvent() {
	CCore::getMap()->getEvent()->resetData();
	CCore::getMap()->getPlayer()->stopMove();
	CCore::getMap()->getPlayer()->resetJump();

	CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cPIPE);

	CCore::getMap()->getEvent()->eventTypeID = CCore::getMap()->getEvent()->eNormal;

	CCore::getMap()->getEvent()->newCurrentLevel = newCurrentLevel;
	CCore::getMap()->getEvent()->newLevelType = newLevelType;
	CCore::getMap()->getEvent()->newMoveMap = newMoveMap;

	CCore::getMap()->getEvent()->iSpeed = iSpeed;
	CCore::getMap()->getEvent()->iDelay = iDelay;

	CCore::getMap()->getEvent()->inEvent = false;

	CCore::getMap()->getEvent()->newUnderWater = newUnderWater;

	CCore::getMap()->getEvent()->newMapXPos = (newPlayerPosX <= 32 * 2 ? 0 : -(newPlayerPosX - 32 * 2));
	CCore::getMap()->getEvent()->newPlayerXPos = (newPlayerPosX <= 32 * 2 ? newPlayerPosX : 32 * 2);
	CCore::getMap()->getEvent()->newPlayerYPos = newPlayerPosY;

	if(iType == 0) { // VERTICAL -> NONE
		CCore::getMap()->getEvent()->newPlayerYPos -= CCore::getMap()->getPlayer()->getPowerLVL() > 0 ? 32 : 0;
		CCore::getMap()->getEvent()->vOLDDir.push_back(CCore::getMap()->getEvent()->eBOT);
		CCore::getMap()->getEvent()->vOLDLength.push_back(CCore::getMap()->getPlayer()->getHitBoxY());

		CCore::getMap()->getEvent()->vOLDDir.push_back(CCore::getMap()->getEvent()->eNOTHING);
		CCore::getMap()->getEvent()->vOLDLength.push_back(35);

		for(int i = 0; i < 3; i++) {
			CCore::getMap()->getEvent()->reDrawX.push_back(iLX);
			CCore::getMap()->getEvent()->reDrawY.push_back(iLY - i);
			CCore::getMap()->getEvent()->reDrawX.push_back(iRX);
			CCore::getMap()->getEvent()->reDrawY.push_back(iRY - i);
		}
	} else if(iType == 1) {
		CCore::getMap()->getEvent()->newPlayerXPos += 32 - CCore::getMap()->getPlayer()->getHitBoxX() / 2;

		CCore::getMap()->getEvent()->vOLDDir.push_back(CCore::getMap()->getEvent()->eRIGHT);
		CCore::getMap()->getEvent()->vOLDLength.push_back(CCore::getMap()->getPlayer()->getHitBoxX());

		CCore::getMap()->getEvent()->vOLDDir.push_back(CCore::getMap()->getEvent()->eNOTHING);
		CCore::getMap()->getEvent()->vOLDLength.push_back(35);

		CCore::getMap()->getEvent()->vNEWDir.push_back(CCore::getMap()->getEvent()->ePLAYPIPETOP);
		CCore::getMap()->getEvent()->vNEWLength.push_back(1);

		CCore::getMap()->getEvent()->vNEWDir.push_back(CCore::getMap()->getEvent()->eNOTHING);
		CCore::getMap()->getEvent()->vNEWLength.push_back(35);

		CCore::getMap()->getEvent()->vNEWDir.push_back(CCore::getMap()->getEvent()->eTOP);
		CCore::getMap()->getEvent()->vNEWLength.push_back(CCore::getMap()->getPlayer()->getHitBoxY());

		for(int i = 0; i < 3; i++) {
			CCore::getMap()->getEvent()->reDrawX.push_back(iLX + i);
			CCore::getMap()->getEvent()->reDrawY.push_back(iLY);
			CCore::getMap()->getEvent()->reDrawX.push_back(iRX + i);
			CCore::getMap()->getEvent()->reDrawY.push_back(iRY);

			CCore::getMap()->getEvent()->reDrawX.push_back(CCore::getMap()->getBlockIDX(CCore::getMap()->getEvent()->newPlayerXPos - CCore::getMap()->getEvent()->newMapXPos));
			CCore::getMap()->getEvent()->reDrawY.push_back(CCore::getMap()->getBlockIDY(newPlayerPosY) - 1 - i);
			CCore::getMap()->getEvent()->reDrawX.push_back(CCore::getMap()->getBlockIDX(CCore::getMap()->getEvent()->newPlayerXPos - CCore::getMap()->getEvent()->newMapXPos) + 1);
			CCore::getMap()->getEvent()->reDrawY.push_back(CCore::getMap()->getBlockIDY(newPlayerPosY) - 1 - i);
		}
	} else { // -- VERT -> VERT
		CCore::getMap()->getEvent()->newPlayerXPos -= CCore::getMap()->getPlayer()->getPowerLVL() > 0 ? 32 : 0 - CCore::getMap()->getPlayer()->getHitBoxX()/2;
		CCore::getMap()->getEvent()->vOLDDir.push_back(CCore::getMap()->getEvent()->eBOT);
		CCore::getMap()->getEvent()->vOLDLength.push_back(CCore::getMap()->getPlayer()->getHitBoxY());

		CCore::getMap()->getEvent()->vOLDDir.push_back(CCore::getMap()->getEvent()->eNOTHING);
		CCore::getMap()->getEvent()->vOLDLength.push_back(55);

		for(int i = 0; i < 3; i++) {
			CCore::getMap()->getEvent()->reDrawX.push_back(iLX);
			CCore::getMap()->getEvent()->reDrawY.push_back(iLY - i);
			CCore::getMap()->getEvent()->reDrawX.push_back(iRX);
			CCore::getMap()->getEvent()->reDrawY.push_back(iRY - i);

			CCore::getMap()->getEvent()->reDrawX.push_back(CCore::getMap()->getBlockIDX(CCore::getMap()->getEvent()->newPlayerXPos - CCore::getMap()->getEvent()->newMapXPos));
			CCore::getMap()->getEvent()->reDrawY.push_back(CCore::getMap()->getBlockIDY(newPlayerPosY) - 1 - i);
			CCore::getMap()->getEvent()->reDrawX.push_back(CCore::getMap()->getBlockIDX(CCore::getMap()->getEvent()->newPlayerXPos - CCore::getMap()->getEvent()->newMapXPos) + 1);
			CCore::getMap()->getEvent()->reDrawY.push_back(CCore::getMap()->getBlockIDY(newPlayerPosY) - 1 - i);
		}

		CCore::getMap()->getEvent()->vNEWDir.push_back(CCore::getMap()->getEvent()->ePLAYPIPETOP);
		CCore::getMap()->getEvent()->vNEWLength.push_back(1);

		CCore::getMap()->getEvent()->vNEWDir.push_back(CCore::getMap()->getEvent()->eNOTHING);
		CCore::getMap()->getEvent()->vNEWLength.push_back(35);

		CCore::getMap()->getEvent()->vNEWDir.push_back(CCore::getMap()->getEvent()->eTOP);
		CCore::getMap()->getEvent()->vNEWLength.push_back(CCore::getMap()->getPlayer()->getHitBoxY());
	}

	CCore::getMap()->setInEvent(true);
}