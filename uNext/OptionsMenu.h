#pragma once

#ifndef OPTIONSMENU_H
#define OPTIONSMENU_H

#include "Menu.h"

class OptionsMenu : public Menu
{
private:
	SDL_Rect rRect;
	SDL_Rect rSetKeyRect;
	SDL_Rect rVolumeBG;
	SDL_Rect rVolume;

	bool inSetKey, resetSetKey;

	bool escapeToMainMenu;
public:
	OptionsMenu(void);
	~OptionsMenu(void);

	void Update();
	void Draw(SDL_Renderer* rR);

	void enter();
	void escape();
	void setKey(int keyID);

	void updateActiveButton(int iDir);

	void updateVolumeRect();

	void setEscapeToMainMenu(bool escapeToMainMenu);
};

#endif