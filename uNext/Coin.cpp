/************************
 * @author £ukasz Jakowski
 * @since  20.04.2014 14:32
 * 
 ************************/

#include "Coin.h"
#include "Core.h"

/* ******************************************** */

Coin::Coin(int iXPos, int iYPos) {
	this->iXPos = iXPos;
	this->iYPos = iYPos;

	this->iSpriteID = 0;
	this->iStepID = 0;

	this->iLEFT = 80;
	this->bTOP = true;
	this->bDelete = false;
}


Coin::~Coin(void) {

}

/* ******************************************** */

void Coin::Update() {
	if(iLEFT > 0) {
		iLEFT -= 5;
		iYPos = iYPos + (bTOP ? - 5 : 5);

		++iStepID;
		if(iStepID > 2) {
			iStepID = 0;
			++iSpriteID;
			if(iSpriteID > 3) {
				iSpriteID = 0;
			}
		}
	} else if(bTOP) {
		bTOP = false;
		iLEFT = 80;
	} else {
		bDelete = true;
	}
}

void Coin::Draw(SDL_Renderer* rR) {
	CCore::getMap()->getBlock(50)->getSprite()->getTexture(iSpriteID)->Draw(rR, iXPos + (int)CCore::getMap()->getXPos(), iYPos);
}

/* ******************************************** */

int Coin::getXPos() {
	return iXPos;
}

int Coin::getYPos() {
	return iYPos;
}

bool Coin::getDelete() {
	return bDelete;
}