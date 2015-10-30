#include "MapLevel.h"

/* ******************************************** */

MapLevel::MapLevel(void) { }

MapLevel::MapLevel(int iBlockID) {
	this->iBlockID = iBlockID;

	this->iNumOfUse = 0;

	this->blockAnimation = false;
	this->iYPos = 0;
	this->bYDirection = true;

	this->spawnMushroom = false;
	this->spawnStar = false;

	this->powerUP = true;
}

MapLevel::~MapLevel(void) {

}

/* ******************************************** */

void MapLevel::startBlockAnimation() {
	this->blockAnimation = true;
	this->iYPos = 0;
	this->bYDirection = true;
}

int MapLevel::updateYPos() {
	if(blockAnimation) {
		if (bYDirection)
		{
			if (iYPos < 10)
			{
				if (iYPos < 5) {
					iYPos += 2;
				} else {
					 iYPos += 1;
				}
			}
			else {
				bYDirection = false;
			}
		}
		else {
			if (iYPos > 0) {
				if (iYPos > 5) {
					iYPos -= 2;
				} else {
					iYPos -= 1;
				}
			}
			else {
				blockAnimation = false;
			}
		}
	}

	return iYPos;
}

/* ******************************************** */

int MapLevel::getBlockID() {
	return iBlockID;
}

void MapLevel::setBlockID(int iBlockID) {
	this->iBlockID = iBlockID;
}

int MapLevel::getYPos() {
	return iYPos;
}

int MapLevel::getNumOfUse() {
	return iNumOfUse;
}

void MapLevel::setNumOfUse(int iNumOfUse) {
	this->iNumOfUse = iNumOfUse;
}

bool MapLevel::getSpawnMushroom() {
	return spawnMushroom;
}

void MapLevel::setSpawnMushroom(bool spawnMushroom) {
	this->spawnMushroom = spawnMushroom;
}

bool MapLevel::getPowerUP() {
	return powerUP;
}

void MapLevel::setPowerUP(bool powerUP) {
	this->powerUP = powerUP;
}

bool MapLevel::getSpawnStar() {
	return spawnStar;
}

void MapLevel::setSpawnStar(bool spawnStar) {
	this->spawnStar = spawnStar;
}