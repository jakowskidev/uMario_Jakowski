#include "Flag.h"
#include "Core.h"

/* ******************************************** */

Flag::Flag(int iXPos, int iYPos) {
	this->iXPos = iXPos;
	this->iYPos = iYPos;

	this->iYTextPos = CCFG::GAME_HEIGHT - 3*32;
	this->iPoints = -1;

	this->iBlockID = CCore::getMap()->getLevelType() == 4 || CCore::getMap()->getLevelType() == 3 ? 168 : 42;

	this->castleFlagExtraXPos = this->castleFlagY = 0;
}

Flag::~Flag(void) {

}

/* ******************************************** */

void Flag::Update() {
	iYPos += 4;
	iYTextPos -= 4;
}

void Flag::UpdateCastleFlag() {
	if(castleFlagY <= 56)
	castleFlagY += 2;
}

void Flag::Draw(SDL_Renderer* rR, CIMG* iIMG) {
	iIMG->Draw(rR, (int)(iXPos + CCore::getMap()->getXPos()), iYPos);

	if(iPoints > 0) {
		CCFG::getText()->Draw(rR, std::to_string(iPoints), (int)(iXPos + CCore::getMap()->getXPos() + 42), iYTextPos - 22, 8, 16);
	}
}

void Flag::DrawCastleFlag(SDL_Renderer* rR, CIMG* iIMG) {
	iIMG->Draw(rR, (int)(iXPos + CCore::getMap()->getXPos() + castleFlagExtraXPos + 7*32 - 14), CCFG::GAME_HEIGHT + 14 - 6*32 - castleFlagY);
}

/* ******************************************** */