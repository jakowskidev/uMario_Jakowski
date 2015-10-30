/************************
 * @author £ukasz Jakowski
 * @since  20.04.2014 14:32
 * 
 ************************/

#include "Points.h"
#include "CFG.h"
#include "Core.h"

/* ******************************************** */

Points::Points(int iXPos, int iYPos, std::string sText) {
	this->iXPos = iXPos;
	this->iYPos = iYPos;
	this->sText = sText;
	this->iYLEFT = 96;

	this->iW = 8;
	this->iH = 16;

	this->bDelete = false;
}

Points::Points(int iXPos, int iYPos, std::string sText, int iW, int iH) {
	this->iXPos = iXPos;
	this->iYPos = iYPos;
	this->sText = sText;
	this->iYLEFT = 96;

	this->iW = iW;
	this->iH = iH;

	this->bDelete = false;
}

Points::~Points(void) {
	
}

/* ******************************************** */

void Points::Update() {
	if(iYLEFT > 0) {
		iYPos -= 2;
		iYLEFT -= 2;
	} else {
		bDelete = true;
	}
}

void Points::Draw(SDL_Renderer* rR) {
	CCFG::getText()->Draw(rR, sText, iXPos + (int)CCore::getMap()->getXPos(), iYPos, iW, iH);
}

/* ******************************************** */

bool Points::getDelete() {
	return bDelete;
}