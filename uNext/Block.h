#pragma once

#ifndef BLOCK_H
#define BLOCK_H

#include "IMG.h"
#include "Sprite.h"

/* ******************************************** */

class Block
{
private :
	Sprite* sSprite;

	int iBlockID;

	bool bCollision;
	bool bDeath;
	bool bUse;
	bool bVisible;

public:
	Block(void);
	Block(int iBlockID, Sprite* sSprite, bool bCollision, bool bDeath, bool bUse, bool bVisible);
	~Block(void);

	void Draw(SDL_Renderer* rR, int iOffsetX, int iOffsetY);

	/* ----- get & set ----- */
	int getBlockID();
	void setBlockID(int iID);

	Sprite* getSprite();
	bool getCollision();
	bool getDeath();
	bool getUse();
	bool getVisible();
};

#endif
