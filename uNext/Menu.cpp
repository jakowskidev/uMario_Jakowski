#include "Menu.h"
#include "CFG.h"

/* ******************************************** */

Menu::Menu(void) {
	this->activeMenuOption = 0;
}


Menu::~Menu(void) {

}

/* ******************************************** */

void Menu::Update() {

}

void Menu::Draw(SDL_Renderer* rR) {
	for(unsigned int i = 0; i < lMO.size(); i++) {
		CCFG::getText()->Draw(rR, lMO[i]->getText(), lMO[i]->getXPos(), lMO[i]->getYPos());
	}

	CCFG::getMM()->getActiveOption()->Draw(rR, lMO[activeMenuOption]->getXPos() - 32, lMO[activeMenuOption]->getYPos());
}

/* ******************************************** */

void Menu::updateActiveButton(int iDir) {
	switch(iDir) {
		case 0:
			if (activeMenuOption - 1 < 0) {
				activeMenuOption = numOfMenuOptions - 1;
			} else {
				--activeMenuOption;
			}
			break;
		case 2:
			if (activeMenuOption + 1 >= numOfMenuOptions) {
				activeMenuOption = 0;
			} else {
				++activeMenuOption;
			}
			break;
		default:
			break;
	}
}