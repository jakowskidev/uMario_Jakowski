#include "Platform.h"

#include "CFG.h"
#include "Core.h"

/* ******************************************** */

Platform::Platform(int iSize, int iType, int iXStart, int iXEnd, int iYStart, int iYEnd, float fXPos, float fYPos, bool direction, int seesawPlatformID) {
	this->iSize = iSize;
	this->iType = iType;
	this->iXStart = iXStart;
	this->iXEnd = iXEnd;
	this->iYStart = iYStart;
	this->iYEnd = iYEnd;
	this->fXPos = fXPos;
	this->fYPos = fYPos;

	this->direction = direction;
	this->seesawPlatformID = seesawPlatformID;

	if(iType == 6) {
		this->iXStart = (int)fYPos;
	}

	this->ON = false;
}

Platform::~Platform(void) {

}

/* ******************************************** */

void Platform::Update() {
	switch(iType) {
		case 0:
			if(fYPos > iYEnd) {
				fYPos = (float)iYStart;
			} else {
				fYPos += 2;
			}
			break;
		case 1:
			if(fYPos < iYStart) {
				fYPos = (float)iYEnd;
			} else {
				fYPos -= 2;
			}
			break;
		case 2: // -- TOP & BOT
			if(direction) {
				if(fYPos < iYStart) {
					direction = false;
				} else if(fYPos < iYStart + 32) {
					fYPos -= 1;
				} else {
					fYPos -= 2;
				}
			} else {
				if(fYPos > iYEnd) {
					direction = true;
				} else if(fYPos > iYEnd - 32) {
					fYPos += 1;
				} else {
					fYPos += 2;
				}
			}
			break;
		case 3: // -- LEFT & RIGHT
			if(direction) {
				if(fXPos > iXEnd) {
					direction = false;
				}
				else if(fXPos < iXStart + 16 || fXPos + 16 > iXEnd) {
					fXPos += 0.5f;
				} else {
					fXPos += 1;
				}
			} else {
				if(fXPos < iXStart) {
					direction = true;
				} else if(fXPos - 16 < iXStart || fXPos + 16 > iXEnd) {
					fXPos -= 0.5f;
				} else {
					fXPos -= 1;
				}
			}
			break;
		case 4:
			if(fXPos > iXEnd) {
				iSize = 0;
			} else if(ON) {
				fXPos += 3;
			}
			break;
		case 7:
			if(fYPos < CCFG::GAME_HEIGHT + 32) {
				fYPos += 4;
			}
			break;
	}
}

void Platform::Draw(SDL_Renderer* rR) {
	if(iType == 6 || iType == 7) { // -- iXStart = YPos 2
		for(int i = 1; i < (iXStart - iYEnd)/16 + 2; i++) {
			CCore::getMap()->getBlock(CCore::getMap()->getCurrentLevelID() == 22 ? 165 : 135)->getSprite()->getTexture()->Draw(rR, (int)(iXEnd + (int)CCore::getMap()->getXPos() + (iSize*8)/2 + 16 - (iSize/2%2 == 0 ? 8 : 0)), (int)(iXStart - 16*i));
		}
	}

	for(int i = 0; i < iSize; i++) {
		CCore::getMap()->getBlock(iType != 4 ? 74 : 126)->Draw(rR, (int)fXPos + i*16 + (int)CCore::getMap()->getXPos(), (int)fYPos);
	}
}

/* ******************************************** */

float Platform::getMoveX() {
	switch(iType) {
		case 3:
			if(direction) {
				if(fXPos < iXStart + 16 || fXPos + 16 > iXEnd) {
					return 0.5f;
				} else {
					return 1;
				}
			} else {
				if(fXPos - 16 < iXStart || fXPos + 16 > iXEnd) {
					return - 0.5f;
				} else {
					return -1;
				}
			}
			break;
		case 4:
			return 3;
	}

	return 0;
}

int Platform::getMoveY() {
	switch(iType) {
		case 0:
			return 2;
		case 1:
			return -2;
		case 2:
			if(direction) {
				if(fYPos < iYStart + 32) {
					return -1;
				} else {
					return -2;
				}
			} else {
				if(fYPos > iYEnd - 32) {
					return 1;
				} else {
					return 2;
				}
			}
		case 5:
			return 2;
		case 6:
			return 2;
		case 7:
			return 4;
	}

	return 0;
}

void Platform::moveY() {
	switch(iType) {
		case 5:
			fYPos += 2;
			break;
		case 6:
			fYPos += 2;
			iXStart = (int)fYPos;
			CCore::getMap()->getPlatform(seesawPlatformID)->moveYReverse();
			if(fYPos < iYEnd) {
				iType = 7;
				CCore::getMap()->getPlatform(seesawPlatformID)->setTypeID(7);
			}
			break;

	}
}

void Platform::moveYReverse() {
	switch(iType) {
		case 6:
			fYPos -= 2;
			iXStart = (int)fYPos;
			if(fYPos < iYEnd) {
				iType = 7;
				CCore::getMap()->getPlatform(seesawPlatformID)->setTypeID(7);
			}
			break;

	}
}

/* ******************************************** */

int Platform::getXPos() {
	return (int)fXPos;
}

int Platform::getYPos() {
	return (int)fYPos;
}

int Platform::getSize() {
	return iSize;
}

void Platform::setTypeID(int iType) {
	this->iType = iType;
}

int Platform::getTypeID() {
	return iType;
}

void Platform::turnON() {
	this->ON = true;
}