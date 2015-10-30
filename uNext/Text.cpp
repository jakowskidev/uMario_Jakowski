#include "Text.h"
#include "CFG.h"

/* ******************************************** */

Text::Text(void) {
	rCrop.x = 0;
	rCrop.y = 0;
	rCrop.w = 8;
	rCrop.h = 8;

	rRect.x = 0;
	rRect.y = 0;
	rRect.w = 16;
	rRect.h = 16;

	this->fontSize = 16;
	this->extraLeft = 0;
	this->nextExtraLeft = 0;
}

Text::~Text(void) {
	delete FONT;
}

/* ******************************************** */

void Text::Draw(SDL_Renderer* rR, std::string sText, int X, int Y, int fontSize) {
	this->fontSize = fontSize;
	this->extraLeft = 0;
	this->nextExtraLeft = 0;
	
	for(unsigned int i = 0; i < sText.size(); i++) {
		rCrop.x = getPos(sText.at(i));

		rRect.x = X + fontSize * i - extraLeft;
		rRect.y = Y;
		rRect.w = fontSize;
		rRect.h = fontSize;
		FONT->Draw(rR, rCrop, rRect);
		extraLeft += nextExtraLeft;
		nextExtraLeft = 0;
	}
}

void Text::Draw(SDL_Renderer* rR, std::string sText, int X, int Y, int fontSize, int iR, int iG, int iB) {
	this->fontSize = fontSize;
	this->extraLeft = 0;
	this->nextExtraLeft = 0;
	
	for(unsigned int i = 0; i < sText.size(); i++) {
		SDL_SetTextureColorMod(FONT->getIMG(), iR, iG, iB);
		rCrop.x = getPos(sText.at(i));

		rRect.x = X + fontSize * i - extraLeft;
		rRect.y = Y;
		rRect.w = fontSize;
		rRect.h = fontSize;
		FONT->Draw(rR, rCrop, rRect);
		extraLeft += nextExtraLeft;
		nextExtraLeft = 0;
		SDL_SetTextureColorMod(FONT->getIMG(), 255, 255, 255);
	}
}

void Text::DrawCenterX(SDL_Renderer* rR, std::string sText, int Y, int fontSize, int iR, int iG, int iB) {
	int X = CCFG::GAME_WIDTH / 2 - getTextWidth(sText, fontSize) / 2;

	Draw(rR, sText, X, Y, fontSize, iR, iG, iB);
}

void Text::Draw(SDL_Renderer* rR, std::string sText, int X, int Y, int iWidth, int iHeight) {
	for(unsigned int i = 0; i < sText.size(); i++) {
		rCrop.x = getPos(sText.at(i));

		rRect.x = X + iWidth * i - extraLeft;
		rRect.y = Y;
		rRect.w = iWidth;
		rRect.h = iHeight;
		FONT->Draw(rR, rCrop, rRect);
	}
}


void Text::DrawWS(SDL_Renderer* rR, std::string sText, int X, int Y,int iR, int iG, int iB, int fontSize) {
	this->fontSize = fontSize;
	this->extraLeft = 0;
	this->nextExtraLeft = 0;
	
	for(unsigned int i = 0; i < sText.size(); i++) {
		SDL_SetTextureColorMod(FONT->getIMG(), 0, 0, 0);
		rCrop.x = getPos(sText.at(i));

		rRect.x = X + fontSize * i - extraLeft - 1;
		rRect.y = Y + 1;
		rRect.w = fontSize;
		rRect.h = fontSize;
		FONT->Draw(rR, rCrop, rRect);
		SDL_SetTextureColorMod(FONT->getIMG(), 255, 255, 255);
		rRect.x = X + fontSize * i - extraLeft + 1;
		rRect.y = Y - 1;
		FONT->Draw(rR, rCrop, rRect);
		extraLeft += nextExtraLeft;
		nextExtraLeft = 0;
	}	
}

/* ******************************************** */

int Text::getTextWidth(std::string sText, int fontSize) {
	int iOutput = sText.size() * fontSize;
	nextExtraLeft = 0;

	for(unsigned int i = 0; i < sText.size(); i++) {
		checkExtra(sText.at(i));
	}

	iOutput -= nextExtraLeft;

	return iOutput;
}

int Text::getPos(int iChar) {
	if(iChar >= 43 && iChar < 91) {
		checkExtra(iChar);
		return (iChar - 43) * rCrop.w + rCrop.w;
	}

	if(iChar >= 118 && iChar < 123) { // v w x y z
		return (iChar - 70) * rCrop.w + rCrop.w;
	}

	return 0;
}

/* ******************************************** */

void Text::checkExtra(int iChar) {
	switch(iChar) {
	case 44: case 46: case 58: case 59:
		nextExtraLeft += 4 * fontSize / rRect.w;
		break;
	}
}

/* ******************************************** */

void Text::setFont(SDL_Renderer* rR, std::string fileName) {
	FONT = new CIMG(fileName, rR);
}