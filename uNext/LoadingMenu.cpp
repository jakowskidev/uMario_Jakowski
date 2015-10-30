#include "LoadingMenu.h"
#include "CFG.h"
#include "Core.h"

/* ******************************************** */

LoadingMenu::LoadingMenu(void) {
	this->iTime = 0;
	this->loadingType = true;
}

LoadingMenu::~LoadingMenu(void) {

}

/* ******************************************** */

void LoadingMenu::Update() {
	if(SDL_GetTicks() >= iTime + 2500 + (loadingType ? 0 : 2750)) {
		if(loadingType) {
			if(!CCore::getMap()->getInEvent()) {
				CCore::getMap()->setSpawnPoint();
				CCore::getMap()->loadLVL();
			}
			CCFG::getMM()->setViewID(CCFG::getMM()->eGame);
			CCFG::getMusic()->changeMusic(true, true);
		} else {
			CCore::getMap()->resetGameData();
			CCFG::getMM()->setViewID(CCFG::getMM()->eMainMenu);
		}
	} else {
		CCFG::getMusic()->StopMusic();
	}
	CCore::getMap()->UpdateBlocks();
}

void LoadingMenu::Draw(SDL_Renderer* rR) {
	if(loadingType) {
		CCore::getMap()->DrawGameLayout(rR);
		CCFG::getText()->Draw(rR, "WORLD", 320, 144);
		CCFG::getText()->Draw(rR, CCore::getMap()->getLevelName(), 416, 144);

		CCore::getMap()->getPlayer()->getMarioSprite()->getTexture()->Draw(rR, 342, 210 - CCore::getMap()->getPlayer()->getHitBoxY()/2);

		CCFG::getText()->Draw(rR, "y", 384, 208);

		if(CCore::getMap()->getPlayer()->getNumOfLives() > 9) {
			CCore::getMap()->getBlock(180)->getSprite()->getTexture()->Draw(rR, 410, 210);
		}
		CCFG::getText()->Draw(rR, std::to_string(CCore::getMap()->getPlayer()->getNumOfLives()), 432, 208);
		CCFG::getText()->DrawCenterX(rR, "REMEMBER THAT YOU CAN RUN WITH " + CCFG::getKeyString(CCFG::keyIDShift), 400, 16);
	} else {
		CCore::getMap()->DrawGameLayout(rR);
		CCFG::getText()->DrawCenterX(rR, "GAME OVER", 240, 16);
	}
}

/* ******************************************** */

void LoadingMenu::updateTime() {
	this->iTime = SDL_GetTicks();
}