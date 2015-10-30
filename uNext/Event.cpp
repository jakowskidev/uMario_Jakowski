#include "Event.h"
#include "Core.h"

/* ******************************************** */

Event::Event(void) {
	this->iDelay = 0;
	this->newUnderWater = false;
	this->endGame = false;
	this->iTime = 0;
	this->bState = true;
	this->stepID = 0;
}

Event::~Event(void) {

}

/* ******************************************** */

void Event::Draw(SDL_Renderer* rR) {
	for(unsigned int i = 0; i < reDrawX.size(); i++) {
		if(reDrawX[i] < CCore::getMap()->getMapWidth())
			CCore::getMap()->getBlock(CCore::getMap()->getMapBlock(reDrawX[i], reDrawY[i])->getBlockID())->Draw(rR, 32 * reDrawX[i] + (int)CCore::getMap()->getXPos(), CCFG::GAME_HEIGHT - 32 * reDrawY[i] - 16);
	}
}

/* ******************************************** */

void Event::Animation() {
	switch(eventTypeID) {
		case eNormal: {
			Normal();
			break;
		}
		case eEnd: {
			Normal();
			end();
			break;
		}
		case eBossEnd: {
			Normal();
			break;
		}
		default:
			Normal();
			break;
	}
}

void Event::Normal() {
	if(bState) {
		if(vOLDDir.size() > stepID) {
			if(vOLDLength[stepID] > 0) {
				switch(vOLDDir[stepID]) {
					case eTOP: // TOP
						CCore::getMap()->getPlayer()->setYPos((float)CCore::getMap()->getPlayer()->getYPos() - iSpeed);
						vOLDLength[stepID] -= iSpeed;
						break;
					case eBOT:
						CCore::getMap()->getPlayer()->setYPos((float)CCore::getMap()->getPlayer()->getYPos() + iSpeed);
						vOLDLength[stepID] -= iSpeed;
						break;
					case eRIGHT:
						CCore::getMap()->getPlayer()->setXPos((float)CCore::getMap()->getPlayer()->getXPos() + iSpeed);
						vOLDLength[stepID] -= iSpeed;
						CCore::getMap()->getPlayer()->moveAnimation();
						CCore::getMap()->getPlayer()->setMoveDirection(true);
						break;
					case eRIGHTEND:
						CCore::getMap()->setXPos((float)CCore::getMap()->getXPos() - iSpeed);
						vOLDLength[stepID] -= iSpeed;
						CCore::getMap()->getPlayer()->moveAnimation();
						CCore::getMap()->getPlayer()->setMoveDirection(true);
						break;
					case eLEFT:
						CCore::getMap()->getPlayer()->setXPos((float)CCore::getMap()->getPlayer()->getXPos() - iSpeed);
						vOLDLength[stepID] -= iSpeed;
						CCore::getMap()->getPlayer()->moveAnimation();
						CCore::getMap()->getPlayer()->setMoveDirection(false);
						break;
					case eBOTRIGHTEND: // BOT & RIGHT
						CCore::getMap()->getPlayer()->setYPos((float)CCore::getMap()->getPlayer()->getYPos() + iSpeed);
						CCore::getMap()->setXPos((float)CCore::getMap()->getXPos() - iSpeed);
						vOLDLength[stepID] -= iSpeed;
						CCore::getMap()->getPlayer()->moveAnimation();
						break;
					case eENDBOT1:
						CCore::getMap()->getPlayer()->setYPos((float)CCore::getMap()->getPlayer()->getYPos() + iSpeed);
						vOLDLength[stepID] -= iSpeed;
						CCore::getMap()->getPlayer()->setMarioSpriteID(10);
						break;
					case eENDBOT2:
						vOLDLength[stepID] -= iSpeed;
						CCore::getMap()->getPlayer()->setMoveDirection(false);
						break;
					case eENDPOINTS:
						if(CCore::getMap()->getMapTime() > 0) {
							CCore::getMap()->setMapTime(CCore::getMap()->getMapTime() - 1);
							CCore::getMap()->getPlayer()->setScore(CCore::getMap()->getPlayer()->getScore() + 50);
							if(CCFG::getMusic()->musicStopped) {
								CCFG::getMusic()->PlayMusic(CCFG::getMusic()->mSCORERING);
							}
						} else {
							vOLDLength[stepID] = 0;
							CCFG::getMusic()->StopMusic();
						}
						CCore::getMap()->getFlag()->UpdateCastleFlag();
						break;
					case eDEATHNOTHING:
						vOLDLength[stepID] -= iSpeed;
						CCore::getMap()->getPlayer()->setMarioSpriteID(0);
						break;
					case eDEATHTOP: // DEATH TOP
						CCore::getMap()->getPlayer()->setYPos((float)CCore::getMap()->getPlayer()->getYPos() - iSpeed);
						vOLDLength[stepID] -= iSpeed;
						CCore::getMap()->getPlayer()->setMarioSpriteID(0);
						break;
					case eDEATHBOT: // DEATH BOT
						CCore::getMap()->getPlayer()->setYPos((float)CCore::getMap()->getPlayer()->getYPos() + iSpeed);
						vOLDLength[stepID] -= iSpeed;
						CCore::getMap()->getPlayer()->setMarioSpriteID(0);
						break;
					case eNOTHING: // NOTHING YAY
						vOLDLength[stepID] -= 1;
						break;
					case ePLAYPIPERIGHT:
						CCore::getMap()->getPlayer()->setXPos((float)CCore::getMap()->getPlayer()->getXPos() + iSpeed);
						vOLDLength[stepID] -= 1;
						CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cPIPE);
						break;
					case eLOADINGMENU:
						vOLDLength[stepID] -= 1;

						if(vOLDLength[stepID] < 2) {
							CCore::getMap()->setInEvent(false);
							inEvent = false;
							CCore::getMap()->getPlayer()->stopMove();
							
							CCFG::getMM()->getLoadingMenu()->loadingType = true;
							CCFG::getMM()->getLoadingMenu()->updateTime();
							CCFG::getMM()->setViewID(CCFG::getMM()->eGameLoading);
						}
						break;
					case eGAMEOVER:
						vOLDLength[stepID] -= 1;

						if(vOLDLength[stepID] < 2) {
							CCore::getMap()->setInEvent(false);
							inEvent = false;
							CCore::getMap()->getPlayer()->stopMove();
							
							CCFG::getMM()->getLoadingMenu()->loadingType = false;
							CCFG::getMM()->getLoadingMenu()->updateTime();
							CCFG::getMM()->setViewID(CCFG::getMM()->eGameLoading);

							CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cGAMEOVER);
						}
						break;
					case eBOSSEND1:
						for(int i = CCore::getMap()->getMapWidth() - 1; i > 0; i--) {
							if(CCore::getMap()->getMapBlock(i, 6)->getBlockID() == 82) {
								CCore::getMap()->getMapBlock(i, 6)->setBlockID(0);
								break;
							}
						}
						//CCore::getMap()->getMapBlock(CCore::getMap()->getBlockIDX((int)(CCore::getMap()->getPlayer()->getXPos() + CCore::getMap()->getPlayer()->getHitBoxX()/2 - CCore::getMap()->getXPos()) + vOLDLength[stepID] - 1), 6)->setBlockID(0);
						CCore::getMap()->clearPlatforms();
						CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cBRIDGEBREAK);
						vOLDLength[stepID] = 0;
						CCore::getMap()->getPlayer()->setMoveDirection(false);
						break;
					case eBOSSEND2:
						//CCore::getMap()->getMapBlock(CCore::getMap()->getBlockIDX((int)(CCore::getMap()->getPlayer()->getXPos() + CCore::getMap()->getPlayer()->getHitBoxX()/2 - CCore::getMap()->getXPos())) - 1, 5)->setBlockID(0);
						//CCore::getMap()->getMapBlock(CCore::getMap()->getBlockIDX((int)(CCore::getMap()->getPlayer()->getXPos() + CCore::getMap()->getPlayer()->getHitBoxX()/2 - CCore::getMap()->getXPos())) - 1, 4)->setBlockID(0);
						for(int i = CCore::getMap()->getMapWidth() - 1; i > 0; i--) {
							if(CCore::getMap()->getMapBlock(i, 5)->getBlockID() == 79) {
								CCore::getMap()->getMapBlock(i, 5)->setBlockID(0);
								break;
							}
						}
						for(int i = CCore::getMap()->getMapWidth() - 1; i > 0; i--) {
							if(CCore::getMap()->getMapBlock(i, 4)->getBlockID() == 76) {
								CCore::getMap()->getMapBlock(i, 4)->setBlockID(0);
								break;
							}
						}
						CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cBRIDGEBREAK);
						vOLDLength[stepID] = 0;
						break;
					case eBOSSEND3:
						for(int i = CCore::getMap()->getMapWidth() - 1; i > 0; i--) {
							if(CCore::getMap()->getMapBlock(i, 4)->getBlockID() == 76) {
								CCore::getMap()->getMapBlock(i, 4)->setBlockID(0);
								break;
							}
						}
						//CCore::getMap()->getMapBlock(CCore::getMap()->getBlockIDX((int)(CCore::getMap()->getPlayer()->getXPos() + CCore::getMap()->getPlayer()->getHitBoxX()/2 - CCore::getMap()->getXPos())) - vOLDLength[stepID], 4)->setBlockID(0);
						CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cBRIDGEBREAK);
						CCore::getMap()->getPlayer()->setMoveDirection(true);
						vOLDLength[stepID] = 0;
						break;
					case eBOSSEND4:
						CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cBOWSERFALL);
						vOLDLength[stepID] = 0;
						break;
					case eBOTRIGHTBOSS: // BOT & RIGHT
						CCore::getMap()->getPlayer()->moveAnimation();
						CCore::getMap()->getPlayer()->playerPhysics();
						CCore::getMap()->setXPos((float)CCore::getMap()->getXPos() - iSpeed);
						vOLDLength[stepID] -= iSpeed;
						break;
					case eBOSSTEXT1:
						CCore::getMap()->addText(vOLDLength[stepID], CCFG::GAME_HEIGHT - 16 - 9*32, "THANK YOU MARIOz");
						vOLDLength[stepID] = 0;
						break;
					case eBOSSTEXT2:
						CCore::getMap()->addText(vOLDLength[stepID] + 16, CCFG::GAME_HEIGHT - 16 - 7*32, "BUT OUR PRINCESS IS IN");
						CCore::getMap()->addText(vOLDLength[stepID] + 16, CCFG::GAME_HEIGHT - 16 - 6*32, "ANOTHER CASTLEz");
						vOLDLength[stepID] = 0;
						break;
					case eENDGAMEBOSSTEXT1:
						CCore::getMap()->addText(vOLDLength[stepID], CCFG::GAME_HEIGHT - 16 - 9*32, "THANK YOU MARIOz");
						vOLDLength[stepID] = 0;
						break;
					case eENDGAMEBOSSTEXT2:
						CCore::getMap()->addText(vOLDLength[stepID] + 16, CCFG::GAME_HEIGHT - 16 - 7*32, "YOUR QUEST IS OVER.");
						vOLDLength[stepID] = 0;
						break;
					case eMARIOSPRITE1:
						CCore::getMap()->getPlayer()->setMarioSpriteID(1);
						vOLDLength[stepID] = 0;
						break;
					case eVINE1:
						CCore::getMap()->getPlayer()->setYPos((float)CCore::getMap()->getPlayer()->getYPos() - iSpeed);
						vOLDLength[stepID] -= iSpeed;
						CCore::getMap()->getPlayer()->setMarioSpriteID(10);
						break;
					case eVINE2:
						CCore::getMap()->getPlayer()->setYPos((float)CCore::getMap()->getPlayer()->getYPos() - iSpeed);
						vOLDLength[stepID] -= iSpeed;
						CCore::getMap()->getPlayer()->setMarioSpriteID(11);
						break;
					case eVINESPAWN:
						CCore::getMap()->addVine(vOLDLength[stepID], 0, 1, newLevelType == 0 || newLevelType == 4 ? 34 : 36);
						vOLDLength[stepID] = 0;
						break;
				}
			} else {
				++stepID;
				iTime = SDL_GetTicks();
			}
		} else {
			if(!endGame) {
				if(SDL_GetTicks() >= iTime + iDelay) {
					bState = false;
					stepID = 0;
					newLevel();
					CCore::getMap()->getPlayer()->stopMove();
					if(inEvent) {
						CCFG::getMM()->getLoadingMenu()->updateTime();
						CCFG::getMM()->getLoadingMenu()->loadingType = true;
						CCFG::getMM()->setViewID(CCFG::getMM()->eGameLoading);
						CCore::getMap()->startLevelAnimation();
					}

					CCFG::keySpace = false;
				}
			} else {
				CCore::getMap()->resetGameData();
				CCFG::getMM()->setViewID(CCFG::getMM()->eMainMenu);
				CCore::getMap()->setInEvent(false);
				CCore::getMap()->getPlayer()->stopMove();
				inEvent = false;
				CCFG::keySpace = false;
				endGame = false;
				stepID = 0;
			}
		}
	} else {
		if(vNEWDir.size() > stepID) {
			if(vNEWLength[stepID] > 0) {
				switch(vNEWDir[stepID]) {
					case eTOP: // TOP
						CCore::getMap()->getPlayer()->setYPos((float)CCore::getMap()->getPlayer()->getYPos() - iSpeed);
						vNEWLength[stepID] -= iSpeed;
						break;
					case eBOT:
						CCore::getMap()->getPlayer()->setYPos((float)CCore::getMap()->getPlayer()->getYPos() + iSpeed);
						vNEWLength[stepID] -= iSpeed;
						break;
					case eRIGHT:
						CCore::getMap()->getPlayer()->setXPos((float)CCore::getMap()->getPlayer()->getXPos() + iSpeed);
						vNEWLength[stepID] -= iSpeed;
						CCore::getMap()->getPlayer()->moveAnimation();
						break;
					case eLEFT:
						CCore::getMap()->getPlayer()->setXPos((float)CCore::getMap()->getPlayer()->getXPos() - iSpeed);
						vNEWLength[stepID] -= iSpeed;
						CCore::getMap()->getPlayer()->moveAnimation();
						break;
					case ePLAYPIPETOP:
						vNEWLength[stepID] -= 1;
						CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cPIPE);
						break;
					case eNOTHING: // NOTHING YAY
						vNEWLength[stepID] -= 1;
						break;
					case eVINE1:
						CCore::getMap()->getPlayer()->setYPos((float)CCore::getMap()->getPlayer()->getYPos() - iSpeed);
						vNEWLength[stepID] -= iSpeed;
						CCore::getMap()->getPlayer()->setMarioSpriteID(10);
						break;
					case eVINE2:
						CCore::getMap()->getPlayer()->setYPos((float)CCore::getMap()->getPlayer()->getYPos() - iSpeed);
						vNEWLength[stepID] -= iSpeed;
						CCore::getMap()->getPlayer()->setMarioSpriteID(11);
						break;
				}
			} else {
				++stepID;
			}
		} else {
			CCore::getMap()->setInEvent(false);
			CCore::getMap()->getPlayer()->stopMove();
			CCFG::getMusic()->changeMusic(true, true);
			inEvent = false;
			CCFG::keySpace = false;
			CCore::resetKeys();
		}
	}
}

void Event::end() {
	if(CCore::getMap()->getFlag() != NULL && CCore::getMap()->getFlag()->iYPos < CCFG::GAME_HEIGHT - 16 - 3*32 - 4) {
		CCore::getMap()->getFlag()->Update();
	}
}

void Event::newLevel() {
	CCore::getMap()->setXPos((float)newMapXPos);
	CCore::getMap()->getPlayer()->setXPos((float)newPlayerXPos);
	CCore::getMap()->getPlayer()->setYPos((float)newPlayerYPos);
	CCore::getMap()->setMoveMap(newMoveMap);
	if(CCore::getMap()->getCurrentLevelID() != newCurrentLevel) {
		CCFG::getMM()->getLoadingMenu()->updateTime();
		CCFG::getMM()->getLoadingMenu()->loadingType = true;
		CCFG::getMM()->setViewID(CCFG::getMM()->eGameLoading);
		CCore::getMap()->getPlayer()->setCoins(0);
	}
	CCore::getMap()->setCurrentLevelID(newCurrentLevel);
	CCore::getMap()->setLevelType(newLevelType);
	if(newUnderWater) {
		CCore::getMap()->getPlayer()->resetRun();
	}
	CCore::getMap()->setUnderWater(newUnderWater);

	CCore::getMap()->lockMinions();
}

/* ******************************************** */

void Event::resetData() {
	vNEWDir.clear();
	vNEWLength.clear();
	vOLDDir.clear();
	vOLDLength.clear();
	resetRedraw();

	this->eventTypeID = eNormal;

	this->bState = true;
	this->stepID = 0;
	this->inEvent = false;
	this->endGame = false;
	this->newUnderWater = false;
}

void Event::resetRedraw() {
	reDrawX.clear();
	reDrawY.clear();
}