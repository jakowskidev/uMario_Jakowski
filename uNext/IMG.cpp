#include "IMG.h"

/* ******************************************** */

CIMG::CIMG(void) { }

CIMG::CIMG(std::string fileName, SDL_Renderer* rR) {
	setIMG(fileName, rR);
}

CIMG::~CIMG(void) {
	SDL_DestroyTexture(tIMG);
}

/* ******************************************** */

void CIMG::Draw(SDL_Renderer* rR, int iXOffset, int iYOffset) {
	rRect.x = iXOffset;
	rRect.y = iYOffset;

	SDL_RenderCopy(rR, tIMG, NULL, &rRect);
}

void CIMG::Draw(SDL_Renderer* rR, int iXOffset, int iYOffset, bool bRotate) {
	rRect.x = iXOffset;
	rRect.y = iYOffset;

	if(!bRotate) {
		SDL_RenderCopy(rR, tIMG, NULL, &rRect);
	} else {
		SDL_RenderCopyEx(rR, tIMG, NULL, &rRect, 180.0, NULL, SDL_FLIP_VERTICAL);
	}
}

void CIMG::DrawVert(SDL_Renderer* rR, int iXOffset, int iYOffset) {
	rRect.x = iXOffset;
	rRect.y = iYOffset;

	SDL_RenderCopyEx(rR, tIMG, NULL, &rRect, 180.0, NULL, SDL_FLIP_HORIZONTAL);
}

void CIMG::Draw(SDL_Renderer* rR, SDL_Rect rCrop, SDL_Rect rRect) {
	SDL_RenderCopy(rR, tIMG, &rCrop, &rRect);
}

/* ******************************************** */

void CIMG::setIMG(std::string fileName, SDL_Renderer* rR) {
	fileName = "files/images/" + fileName + ".bmp";
	SDL_Surface* loadedSurface = SDL_LoadBMP(fileName.c_str());
	SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 255, 0, 255));

	tIMG = SDL_CreateTextureFromSurface(rR, loadedSurface);
	int iWidth, iHeight;

	SDL_QueryTexture(tIMG, NULL, NULL, &iWidth, &iHeight);
	
	rRect.x  = 0;
	rRect.y = 0;
	rRect.w = iWidth;
	rRect.h = iHeight;
	SDL_FreeSurface(loadedSurface);
}

SDL_Texture* CIMG::getIMG() {
	return tIMG;
}

SDL_Rect CIMG::getRect() {
	return rRect;
}