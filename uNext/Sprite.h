#pragma once

#ifndef SPRITE_H
#define SPRITE_H

#include "IMG.h"
#include <string>
#include <vector>

class Sprite
{
private:
	std::vector<CIMG*> tSprite;
	std::vector<unsigned int> iDelay;

	bool bRotate;

	int iCurrentFrame;
	int iStartFrame;
	int iEndFrame;

	unsigned long lTimePassed;

public:
	Sprite(void);
	Sprite(SDL_Renderer* rR, std::vector<std::string> tSprite, std::vector<unsigned int> iDelay, bool bRotate);
	~Sprite(void);

	void Update();

	/* ----- get & set ----- */
	CIMG* getTexture();
	CIMG* getTexture(int iID);
};

#endif

