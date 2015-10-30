#include "LevelText.h"

/* ******************************************** */

LevelText::LevelText(int iXPos, int iYPos, std::string sText) {
	this->iXPos = iXPos;
	this->iYPos = iYPos;
	this->sText = sText;
}

LevelText::~LevelText(void) {

}

/* ******************************************** */

int LevelText::getXPos() {
	return iXPos;
}

int LevelText::getYPos() {
	return iYPos;
}

void LevelText::setText(std::string sText) {
	this->sText = sText;
}

std::string LevelText::getText() {
	return sText;
}