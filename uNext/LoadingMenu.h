#pragma once

#ifndef LOADINGMENU_H
#define LOADINGMENU_H

#include "Menu.h"

class LoadingMenu : public Menu
{
private:
	unsigned int iTime;
public:
	LoadingMenu(void);
	~LoadingMenu(void);

	void Update();
	void Draw(SDL_Renderer* rR);

	void updateTime();

	bool loadingType; // -- true = LOADING, false = GAME OVER
};

#endif