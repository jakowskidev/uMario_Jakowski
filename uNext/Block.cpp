#include "Block.h"

/* ******************************************** */

Block::Block(void) { }

Block::Block(int iBlockID, Sprite* sSprite, bool bCollision, bool bDeath, bool bUse, bool bVisible) {
	this->iBlockID = iBlockID;
	this->sSprite = sSprite;
	this->bCollision = bCollision;
	this->bDeath = bDeath;
	this->bUse = bUse;
	this->bVisible = bVisible;
}

Block::~Block(void) {
	delete sSprite;
}

/* ******************************************** */

void Block::Draw(SDL_Renderer* rR, int iOffsetX, int iOffsetY) {
	sSprite->getTexture()->Draw(rR, iOffsetX, iOffsetY);
}

/* ******************************************** */

int Block::getBlockID() {
	return iBlockID;
}

void Block::setBlockID(int iBlockID) {
	this->iBlockID = iBlockID;
}

Sprite* Block::getSprite() {
	return sSprite;
}

bool Block::getCollision() {
	return bCollision;
}

bool Block::getDeath() {
	return bDeath;
}

bool Block::getUse() {
	return bUse;
}

bool Block::getVisible() {
	return bVisible;
}