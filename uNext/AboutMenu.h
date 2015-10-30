#pragma once

#ifndef ABOUTMENU_H
#define ABOUTMENU_H

#include "Menu.h"

class AboutMenu : public Menu
{
private:
	unsigned int iTime;

	int cR, cG, cB, nR, nG, nB;
	int colorStepID, iColorID;

	// ----- true = RIGHT, false = LEFT
	bool moveDirection;

	int iNumOfUnits;
public:
	AboutMenu(void);
	~AboutMenu(void);

	void Update();
	void Draw(SDL_Renderer* rR);

	void enter();

	void launch();
	void reset();

	void nextColor();
	int getColorStep(int iOld, int iNew);

	void setBackgroundColor(SDL_Renderer* rR);
	void updateTime();
};

#endif