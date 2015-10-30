#include "Map.h"
#include "CFG.h"
#include "math.h"
#include "stdlib.h"
#include "time.h"

/* ******************************************** */

Map::Map(void) {

}

Map::Map(SDL_Renderer* rR) {
	oPlayer = new Player(rR, 84, 368);

	this->currentLevelID = 0;

	this->iMapWidth = 0;
	this->iMapHeight = 0;
	this->iLevelType = 0;

	this->drawLines = false;
	this->fXPos = 0;
	this->fYPos = 0;

	this->inEvent = false;

	this->iSpawnPointID = 0;

	this->bMoveMap = true;

	this->iFrameID = 0;

	this->bTP = false;

	CCFG::getText()->setFont(rR, "font");

	oEvent = new Event();
	oFlag = NULL;

	srand((unsigned)time(NULL));

	loadGameData(rR);
	loadLVL();
}

Map::~Map(void) {
	for(std::vector<Block*>::iterator i = vBlock.begin(); i != vBlock.end(); i++) {
		delete (*i);
	}

	for(std::vector<Block*>::iterator i = vMinion.begin(); i != vMinion.end(); i++) {
		delete (*i);
	}

	delete oEvent;
	delete oFlag;
}

/* ******************************************** */

void Map::Update() {
	UpdateBlocks();

	if(!oPlayer->getInLevelAnimation()) {
		UpdateMinionBlokcs();

		UpdateMinions();
		
		if(!inEvent) {
			UpdatePlayer();

			++iFrameID;
			if(iFrameID > 32) {
				iFrameID = 0;
				if(iMapTime > 0) {
					--iMapTime;
					if(iMapTime == 90) {
						CCFG::getMusic()->StopMusic();
						CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cLOWTIME);
					} else if(iMapTime == 86) {
						CCFG::getMusic()->changeMusic(true, true);
					}

					if(iMapTime <= 0) {
						playerDeath(true, true);
					}
				}
			}
		} else {
			oEvent->Animation();
		}

		for(unsigned int i = 0; i < vPlatform.size(); i++) {
			vPlatform[i]->Update();
		}
	} else {
		oPlayer->powerUPAnimation();
	}

	for(unsigned int i = 0; i < lBlockDebris.size(); i++) {
		if(lBlockDebris[i]->getDebrisState() != -1) {
			lBlockDebris[i]->Update();
		} else {
			delete lBlockDebris[i];
			lBlockDebris.erase(lBlockDebris.begin() + i);
		}
	}
	
	for(unsigned int i = 0; i < lPoints.size(); i++) {
		if(!lPoints[i]->getDelete()) {
			lPoints[i]->Update();
		} else {
			delete lPoints[i];
			lPoints.erase(lPoints.begin() + i);
		}
	}

	for(unsigned int i = 0; i < lCoin.size(); i++) {
		if(!lCoin[i]->getDelete()) {
			lCoin[i]->Update();
		} else {
			lPoints.push_back(new Points(lCoin[i]->getXPos(), lCoin[i]->getYPos(), "200"));
			delete lCoin[i];
			lCoin.erase(lCoin.begin() + i);
		}
	}
}

void Map::UpdatePlayer() {
	oPlayer->Update();
	checkSpawnPoint();
}

void Map::UpdateMinions() {
	for(int i = 0; i < iMinionListSize; i++) {
		for(int j = 0, jSize = lMinion[i].size(); j < jSize; j++) {
			if(lMinion[i][j]->updateMinion()) {
				lMinion[i][j]->Update();
			}
		}
	}
	
	// ----- UPDATE MINION LIST ID
	for(int i = 0; i < iMinionListSize; i++) {
		for(int j = 0, jSize = lMinion[i].size(); j < jSize; j++) {
			if(lMinion[i][j]->minionSpawned) {
				if(lMinion[i][j]->minionState == -1) {
					delete lMinion[i][j];
					lMinion[i].erase(lMinion[i].begin() + j);
					jSize = lMinion[i].size();
					continue;
				}
				
				if(floor(lMinion[i][j]->fXPos / 160) != i) {
					lMinion[(int)floor((int)lMinion[i][j]->fXPos / 160)].push_back(lMinion[i][j]);
					lMinion[i].erase(lMinion[i].begin() + j);
					jSize = lMinion[i].size();
				}
			}
		}
	}

	for(unsigned int i = 0; i < lBubble.size(); i++) {
		lBubble[i]->Update();

		if(lBubble[i]->getDestroy()) {
			delete lBubble[i];
			lBubble.erase(lBubble.begin() + i);
		}
	}
}

void Map::UpdateMinionsCollisions() {
	// ----- COLLISIONS
	for(int i = 0; i < iMinionListSize; i++) {
		for(unsigned int j = 0; j < lMinion[i].size(); j++) {
			if(!lMinion[i][j]->collisionOnlyWithPlayer /*&& lMinion[i][j]->minionSpawned*/ && lMinion[i][j]->deadTime < 0) {
				// ----- WITH MINIONS IN SAME LIST
				for(unsigned int k = j + 1; k < lMinion[i].size(); k++) {
					if(!lMinion[i][k]->collisionOnlyWithPlayer /*&& lMinion[i][k]->minionSpawned*/ && lMinion[i][k]->deadTime < 0) {
						if(lMinion[i][j]->getXPos() < lMinion[i][k]->getXPos()) {
							if(lMinion[i][j]->getXPos() + lMinion[i][j]->iHitBoxX >= lMinion[i][k]->getXPos() && lMinion[i][j]->getXPos() + lMinion[i][j]->iHitBoxX <= lMinion[i][k]->getXPos() + lMinion[i][k]->iHitBoxX && ((lMinion[i][j]->getYPos() <= lMinion[i][k]->getYPos() + lMinion[i][k]->iHitBoxY && lMinion[i][j]->getYPos() + lMinion[i][j]->iHitBoxY >= lMinion[i][k]->getYPos() + lMinion[i][k]->iHitBoxY) || (lMinion[i][k]->getYPos() <= lMinion[i][j]->getYPos() + lMinion[i][j]->iHitBoxY && lMinion[i][k]->getYPos() + lMinion[i][k]->iHitBoxY >= lMinion[i][j]->getYPos() + lMinion[i][j]->iHitBoxY))) {
								if(lMinion[i][j]->killOtherUnits && lMinion[i][j]->moveSpeed > 0 && lMinion[i][k]->minionSpawned) {
									lMinion[i][k]->setMinionState(-2);
									lMinion[i][j]->collisionWithAnotherUnit();
								}

								if(lMinion[i][k]->killOtherUnits && lMinion[i][k]->moveSpeed > 0 && lMinion[i][j]->minionSpawned) {
									lMinion[i][j]->setMinionState(-2);
									lMinion[i][k]->collisionWithAnotherUnit();
								}
							
								if(lMinion[i][j]->getYPos() - 4 <= lMinion[i][k]->getYPos() + lMinion[i][k]->iHitBoxY && lMinion[i][j]->getYPos() + 4 >= lMinion[i][k]->getYPos() + lMinion[i][k]->iHitBoxY) {
									lMinion[i][k]->onAnotherMinion = true;
								} else if(lMinion[i][k]->getYPos() - 4 <= lMinion[i][j]->getYPos() + lMinion[i][j]->iHitBoxY && lMinion[i][k]->getYPos() + 4 >= lMinion[i][j]->getYPos() + lMinion[i][j]->iHitBoxY) {
									lMinion[i][j]->onAnotherMinion = true;
								} else {
									lMinion[i][j]->collisionEffect();
									lMinion[i][k]->collisionEffect();
								}
							}
						} else {
							if(lMinion[i][k]->getXPos() + lMinion[i][k]->iHitBoxX >= lMinion[i][j]->getXPos() && lMinion[i][k]->getXPos() + lMinion[i][k]->iHitBoxX <= lMinion[i][j]->getXPos() + lMinion[i][j]->iHitBoxX && ((lMinion[i][j]->getYPos() <= lMinion[i][k]->getYPos() + lMinion[i][k]->iHitBoxY && lMinion[i][j]->getYPos() + lMinion[i][j]->iHitBoxY >= lMinion[i][k]->getYPos() + lMinion[i][k]->iHitBoxY) || (lMinion[i][k]->getYPos() <= lMinion[i][j]->getYPos() + lMinion[i][j]->iHitBoxY && lMinion[i][k]->getYPos() + lMinion[i][k]->iHitBoxY >= lMinion[i][j]->getYPos() + lMinion[i][j]->iHitBoxY))) {
								if(lMinion[i][j]->killOtherUnits && lMinion[i][j]->moveSpeed > 0 && lMinion[i][k]->minionSpawned) {
									lMinion[i][k]->setMinionState(-2);
									lMinion[i][j]->collisionWithAnotherUnit();
								}

								if(lMinion[i][k]->killOtherUnits && lMinion[i][k]->moveSpeed > 0 && lMinion[i][j]->minionSpawned) {
									lMinion[i][j]->setMinionState(-2);
									lMinion[i][k]->collisionWithAnotherUnit();
								}

								if(lMinion[i][j]->getYPos() - 4 <= lMinion[i][k]->getYPos() + lMinion[i][k]->iHitBoxY && lMinion[i][j]->getYPos() + 4 >= lMinion[i][k]->getYPos() + lMinion[i][k]->iHitBoxY) {
									lMinion[i][k]->onAnotherMinion = true;
								} else if(lMinion[i][k]->getYPos() - 4 <= lMinion[i][j]->getYPos() + lMinion[i][j]->iHitBoxY && lMinion[i][k]->getYPos() + 4 >= lMinion[i][j]->getYPos() + lMinion[i][j]->iHitBoxY) {
									lMinion[i][j]->onAnotherMinion = true;
								} else {
									lMinion[i][j]->collisionEffect();
									lMinion[i][k]->collisionEffect();
								}
							}
						}
					}
				}

				// ----- WITH MINIONS IN OTHER LIST
				if(i + 1 < iMinionListSize) {
					for(unsigned int k = 0; k < lMinion[i + 1].size(); k++) {
						if(!lMinion[i + 1][k]->collisionOnlyWithPlayer /*&& lMinion[i + 1][k]->minionSpawned*/ && lMinion[i + 1][k]->deadTime < 0) {
							if(lMinion[i][j]->getXPos() < lMinion[i + 1][k]->getXPos()) {
								if(lMinion[i][j]->getXPos() + lMinion[i][j]->iHitBoxX >= lMinion[i + 1][k]->getXPos() && lMinion[i][j]->getXPos() + lMinion[i][j]->iHitBoxX <= lMinion[i + 1][k]->getXPos() + lMinion[i + 1][k]->iHitBoxX && ((lMinion[i][j]->getYPos() <= lMinion[i + 1][k]->getYPos() + lMinion[i + 1][k]->iHitBoxY && lMinion[i][j]->getYPos() + lMinion[i][j]->iHitBoxY >= lMinion[i + 1][k]->getYPos() + lMinion[i + 1][k]->iHitBoxY) || (lMinion[i + 1][k]->getYPos() <= lMinion[i][j]->getYPos() + lMinion[i][j]->iHitBoxY && lMinion[i + 1][k]->getYPos() + lMinion[i + 1][k]->iHitBoxY >= lMinion[i][j]->getYPos() + lMinion[i][j]->iHitBoxY))) {
									if(lMinion[i][j]->killOtherUnits && lMinion[i][j]->moveSpeed > 0 && lMinion[i + 1][k]->minionSpawned) {
										lMinion[i + 1][k]->setMinionState(-2);
										lMinion[i][j]->collisionWithAnotherUnit();
									}

									if(lMinion[i + 1][k]->killOtherUnits && lMinion[i + 1][k]->moveSpeed > 0 && lMinion[i][j]->minionSpawned) {
										lMinion[i][j]->setMinionState(-2);
										lMinion[i + 1][k]->collisionWithAnotherUnit();
									}
									
									if(lMinion[i][j]->getYPos() - 4 <= lMinion[i + 1][k]->getYPos() + lMinion[i + 1][k]->iHitBoxY && lMinion[i][j]->getYPos() + 4 >= lMinion[i + 1][k]->getYPos() + lMinion[i + 1][k]->iHitBoxY) {
										lMinion[i + 1][k]->onAnotherMinion = true;
									} else if(lMinion[i + 1][k]->getYPos() - 4 <= lMinion[i][j]->getYPos() + lMinion[i][j]->iHitBoxY && lMinion[i + 1][k]->getYPos() + 4 >= lMinion[i][j]->getYPos() + lMinion[i][j]->iHitBoxY) {
										lMinion[i][j]->onAnotherMinion = true;
									} else {
										lMinion[i][j]->collisionEffect();
										lMinion[i + 1][k]->collisionEffect();
									}
								}
							} else {
								if(lMinion[i + 1][k]->getXPos() + lMinion[i + 1][k]->iHitBoxX >= lMinion[i][j]->getXPos() && lMinion[i + 1][k]->getXPos() + lMinion[i + 1][k]->iHitBoxX < lMinion[i][j]->getXPos() + lMinion[i][j]->iHitBoxX && ((lMinion[i][j]->getYPos() <= lMinion[i + 1][k]->getYPos() + lMinion[i + 1][k]->iHitBoxY && lMinion[i][j]->getYPos() + lMinion[i][j]->iHitBoxY >= lMinion[i + 1][k]->getYPos() + lMinion[i + 1][k]->iHitBoxY) || (lMinion[i + 1][k]->getYPos() <= lMinion[i][j]->getYPos() + lMinion[i][j]->iHitBoxY && lMinion[i + 1][k]->getYPos() + lMinion[i + 1][k]->iHitBoxY >= lMinion[i][j]->getYPos() + lMinion[i][j]->iHitBoxY))) {
									if(lMinion[i][j]->killOtherUnits && lMinion[i][j]->moveSpeed > 0 && lMinion[i + 1][k]->minionSpawned) {
										lMinion[i + 1][k]->setMinionState(-2);
										lMinion[i][j]->collisionWithAnotherUnit();
									}

									if(lMinion[i + 1][k]->killOtherUnits && lMinion[i + 1][k]->moveSpeed > 0 && lMinion[i][j]->minionSpawned) {
										lMinion[i][j]->setMinionState(-2);
										lMinion[i + 1][k]->collisionWithAnotherUnit();
									}
									/*
									if(lMinion[i][j]->getYPos() + lMinion[i][j]->iHitBoxY < lMinion[i + 1][k]->getYPos() + lMinion[i + 1][k]->iHitBoxY) {
										lMinion[i][j]->onAnotherMinion = true;
										continue;
									} else {
										lMinion[i + 1][k]->onAnotherMinion = true;
										continue;
									}*/

									if(lMinion[i][j]->getYPos() - 4 <= lMinion[i + 1][k]->getYPos() + lMinion[i + 1][k]->iHitBoxY && lMinion[i][j]->getYPos() + 4 >= lMinion[i + 1][k]->getYPos() + lMinion[i + 1][k]->iHitBoxY) {
										lMinion[i + 1][k]->onAnotherMinion = true;
									} else if(lMinion[i + 1][k]->getYPos() - 4 <= lMinion[i][j]->getYPos() + lMinion[i][j]->iHitBoxY && lMinion[i + 1][k]->getYPos() + 4 >= lMinion[i][j]->getYPos() + lMinion[i][j]->iHitBoxY) {
										lMinion[i][j]->onAnotherMinion = true;
									} else {
										lMinion[i][j]->collisionEffect();
										lMinion[i + 1][k]->collisionEffect();
									}
								}
							}
						}
					}
				}
			}
		}
	}

	if(!inEvent && !oPlayer->getInLevelAnimation()) {
		// ----- COLLISION WITH PLAYER
		for(int i = getListID(-(int)fXPos + oPlayer->getXPos()) - (getListID(-(int)fXPos + oPlayer->getXPos()) > 0 ? 1 : 0), iSize = i + 2; i < iSize; i++) {
			for(unsigned int j = 0, jSize = lMinion[i].size(); j < jSize; j++) {
				if(lMinion[i][j]->deadTime < 0) {
					if((oPlayer->getXPos() - fXPos >= lMinion[i][j]->getXPos() && oPlayer->getXPos() - fXPos <= lMinion[i][j]->getXPos() + lMinion[i][j]->iHitBoxX) || (oPlayer->getXPos() - fXPos + oPlayer->getHitBoxX() >= lMinion[i][j]->getXPos() && oPlayer->getXPos() - fXPos + oPlayer->getHitBoxX() <= lMinion[i][j]->getXPos() + lMinion[i][j]->iHitBoxX)) {
						if(lMinion[i][j]->getYPos() - 2 <= oPlayer->getYPos() + oPlayer->getHitBoxY() && lMinion[i][j]->getYPos() + 16 >= oPlayer->getYPos() + oPlayer->getHitBoxY()) {
							lMinion[i][j]->collisionWithPlayer(true);
						} else if((lMinion[i][j]->getYPos() <= oPlayer->getYPos() + oPlayer->getHitBoxY() && lMinion[i][j]->getYPos() + lMinion[i][j]->iHitBoxY >= oPlayer->getYPos() + oPlayer->getHitBoxY()) || (lMinion[i][j]->getYPos() <= oPlayer->getYPos() && lMinion[i][j]->getYPos() + lMinion[i][j]->iHitBoxY >= oPlayer->getYPos())) {
							lMinion[i][j]->collisionWithPlayer(false);
						}
					}
				}
			}
		}
	}
}

void Map::UpdateBlocks() {
	vBlock[2]->getSprite()->Update();
	vBlock[8]->getSprite()->Update();
	vBlock[29]->getSprite()->Update();
	vBlock[55]->getSprite()->Update();
	vBlock[57]->getSprite()->Update();
	vBlock[70]->getSprite()->Update();
	vBlock[71]->getSprite()->Update();
	vBlock[72]->getSprite()->Update();
	vBlock[73]->getSprite()->Update();
	vBlock[82]->getSprite()->Update();
}

void Map::UpdateMinionBlokcs() {
	vMinion[0]->getSprite()->Update();
	vMinion[4]->getSprite()->Update();
	vMinion[6]->getSprite()->Update();
	vMinion[7]->getSprite()->Update();
	vMinion[8]->getSprite()->Update();
	vMinion[10]->getSprite()->Update();
	vMinion[12]->getSprite()->Update();
	vMinion[14]->getSprite()->Update();
	vMinion[15]->getSprite()->Update();
	vMinion[17]->getSprite()->Update();
	vMinion[18]->getSprite()->Update();
	vMinion[19]->getSprite()->Update();
	vMinion[20]->getSprite()->Update();
	vMinion[21]->getSprite()->Update();
	vMinion[22]->getSprite()->Update();
	vMinion[23]->getSprite()->Update();
	vMinion[24]->getSprite()->Update();
	vMinion[30]->getSprite()->Update();
	vMinion[31]->getSprite()->Update();
	vMinion[43]->getSprite()->Update();
	vMinion[44]->getSprite()->Update();
	vMinion[45]->getSprite()->Update();
	vMinion[46]->getSprite()->Update();
	vMinion[47]->getSprite()->Update();
	vMinion[48]->getSprite()->Update();
	vMinion[51]->getSprite()->Update();
	vMinion[52]->getSprite()->Update();
	vMinion[53]->getSprite()->Update();
	vMinion[55]->getSprite()->Update();
	vMinion[57]->getSprite()->Update();
	vMinion[62]->getSprite()->Update();
}

/* ******************************************** */

void Map::Draw(SDL_Renderer* rR) {
	DrawMap(rR);

	for(unsigned int i = 0; i < vPlatform.size(); i++) {
		vPlatform[i]->Draw(rR);
	}

	DrawMinions(rR);

	for(unsigned int i = 0; i < lPoints.size(); i++) {
		lPoints[i]->Draw(rR);
	}

	for(unsigned int i = 0; i < lCoin.size(); i++) {
		lCoin[i]->Draw(rR);
	}

	for(unsigned int i = 0; i < lBlockDebris.size(); i++) {
		lBlockDebris[i]->Draw(rR);
	}

	for(unsigned int i = 0; i < vLevelText.size(); i++) {
		CCFG::getText()->Draw(rR, vLevelText[i]->getText(), vLevelText[i]->getXPos() + (int)fXPos, vLevelText[i]->getYPos());
	}
	/*
	if(drawLines) {
		aa->Draw(rR, (int)fXPos, -16);
		DrawLines(rR);
	}*/

	for(unsigned int i = 0; i < lBubble.size(); i++) {
		lBubble[i]->Draw(rR, vBlock[lBubble[i]->getBlockID()]->getSprite()->getTexture());
	}

	oPlayer->Draw(rR);

	if(inEvent) {
		oEvent->Draw(rR);
	}

	DrawGameLayout(rR);
}

void Map::DrawMap(SDL_Renderer* rR) {
	if(oFlag != NULL) {
		oFlag->DrawCastleFlag(rR, vBlock[51]->getSprite()->getTexture());
	}

	for(int i = getStartBlock(), iEnd = getEndBlock(); i < iEnd && i < iMapWidth; i++) {
		for(int j = iMapHeight - 1; j >= 0; j--) {
			if(lMap[i][j]->getBlockID() != 0) {
				vBlock[lMap[i][j]->getBlockID()]->Draw(rR, 32 * i + (int)fXPos, CCFG::GAME_HEIGHT - 32 * j - 16 - lMap[i][j]->updateYPos());
			}
		}
	}

	if(oFlag != NULL) {
		oFlag->Draw(rR, vBlock[oFlag->iBlockID]->getSprite()->getTexture());
	}
}

void Map::DrawMinions(SDL_Renderer* rR) {
	for(int i = 0; i < iMinionListSize; i++) {
		for(int j = 0, jSize = lMinion[i].size(); j < jSize; j++) {
			lMinion[i][j]->Draw(rR, vMinion[lMinion[i][j]->getBloockID()]->getSprite()->getTexture());
			//CCFG::getText()->DrawWS(rR, std::to_string(i), lMinion[i][j]->getXPos() + (int)fXPos, lMinion[i][j]->getYPos(), 0, 0, 0, 8);
		}
	}
}

void Map::DrawGameLayout(SDL_Renderer* rR) {
	CCFG::getText()->Draw(rR, "MARIO", 54, 16);

	if(oPlayer->getScore() < 100) {
		CCFG::getText()->Draw(rR, "00000" + std::to_string(oPlayer->getScore()), 54, 32);
	} else if(oPlayer->getScore() < 1000) {
		CCFG::getText()->Draw(rR, "000" + std::to_string(oPlayer->getScore()), 54, 32);
	} else if(oPlayer->getScore() < 10000) {
		CCFG::getText()->Draw(rR, "00" + std::to_string(oPlayer->getScore()), 54, 32);
	} else if(oPlayer->getScore() < 100000) {
		CCFG::getText()->Draw(rR, "0" + std::to_string(oPlayer->getScore()), 54, 32);
	} else {
		CCFG::getText()->Draw(rR, std::to_string(oPlayer->getScore()), 54, 32);
	}

	CCFG::getText()->Draw(rR, "WORLD", 462, 16);
	CCFG::getText()->Draw(rR, getLevelName(), 480, 32);

	if(iLevelType != 1) {
		vBlock[2]->Draw(rR, 268, 32);
	} else {
		vBlock[57]->Draw(rR, 268, 32);
	}
	CCFG::getText()->Draw(rR, "y", 286, 32);
	CCFG::getText()->Draw(rR, (oPlayer->getCoins() < 10 ? "0" : "") + std::to_string(oPlayer->getCoins()), 302, 32);

	CCFG::getText()->Draw(rR, "TIME", 672, 16);
	if(CCFG::getMM()->getViewID() == CCFG::getMM()->eGame) {
		if(iMapTime > 100) {
			CCFG::getText()->Draw(rR, std::to_string(iMapTime), 680, 32);
		} else if(iMapTime > 10) {
			CCFG::getText()->Draw(rR, "0" + std::to_string(iMapTime), 680, 32);
		} else {
			CCFG::getText()->Draw(rR, "00" + std::to_string(iMapTime), 680, 32);
		}
	}
}

void Map::DrawLines(SDL_Renderer* rR) {
	SDL_SetRenderDrawBlendMode(rR, SDL_BLENDMODE_BLEND); // APLHA ON !
	SDL_SetRenderDrawColor(rR, 255, 255, 255, 128);

	for(int i = 0; i < CCFG::GAME_WIDTH / 32 + 1; i++) {
		SDL_RenderDrawLine(rR, 32 * i - (-(int)fXPos) % 32, 0, 32 * i - (-(int)fXPos) % 32, CCFG::GAME_HEIGHT);
	}

	for(int i = 0; i < CCFG::GAME_HEIGHT / 32 + 1; i++) {
		SDL_RenderDrawLine(rR, 0, 32 * i - 16 + (int)fYPos, CCFG::GAME_WIDTH, 32 * i - 16 + (int)fYPos);
	}

	for(int i = 0; i < CCFG::GAME_WIDTH / 32 + 1; i++) {
		for(int j = 0; j < CCFG::GAME_HEIGHT / 32; j++) {
			CCFG::getText()->Draw(rR, std::to_string(i + (-((int)fXPos + (-(int)fXPos) % 32)) / 32), 32 * i + 16 - (-(int)fXPos) % 32 - CCFG::getText()->getTextWidth(std::to_string(i + (-((int)fXPos + (-(int)fXPos) % 32)) / 32), 8) / 2, CCFG::GAME_HEIGHT - 9 - 32 * j, 8);
			CCFG::getText()->Draw(rR, std::to_string(j), 32 * i + 16 - (-(int)fXPos) % 32 - CCFG::getText()->getTextWidth(std::to_string(j), 8) / 2 + 1, CCFG::GAME_HEIGHT - 32 * j, 8);
		}
	}

	SDL_SetRenderDrawBlendMode(rR, SDL_BLENDMODE_NONE); // APLHA OFF !
}

/* ******************************************** */

void Map::moveMap(int nX, int nY) {
	if (fXPos + nX > 0) {
		oPlayer->updateXPos((int)(nX - fXPos));
		fXPos = 0;
	}
	else {
		this->fXPos += nX;
	}
}

int Map::getStartBlock() {
	return (int)(-fXPos - (-(int)fXPos) % 32) / 32;
}

int Map::getEndBlock() {
	return (int)(-fXPos - (-(int)fXPos) % 32 + CCFG::GAME_WIDTH) / 32 + 2;
}

/* ******************************************** */

/* ******************************************** */
/* ---------------- COLLISION ---------------- */

Vector2* Map::getBlockID(int nX, int nY) {
	return new Vector2((int)(nX < 0 ? 0 : nX) / 32, (int)(nY > CCFG::GAME_HEIGHT - 16 ? 0 : (CCFG::GAME_HEIGHT - 16 - nY + 32) / 32));
}

int Map::getBlockIDX(int nX) {
	return (int)(nX < 0 ? 0 : nX) / 32;
}

int Map::getBlockIDY(int nY) {
	return (int)(nY > CCFG::GAME_HEIGHT - 16 ? 0 : (CCFG::GAME_HEIGHT - 16 - nY + 32) / 32);
}

bool Map::checkCollisionLB(int nX, int nY, int nHitBoxY, bool checkVisible) {
	return checkCollision(getBlockID(nX, nY + nHitBoxY), checkVisible);
}

bool Map::checkCollisionLT(int nX, int nY, bool checkVisible) {
	return checkCollision(getBlockID(nX, nY), checkVisible);
}

bool Map::checkCollisionLC(int nX, int nY, int nHitBoxY, bool checkVisible) {
	return checkCollision(getBlockID(nX, nY + nHitBoxY), checkVisible);
}

bool Map::checkCollisionRC(int nX, int nY, int nHitBoxX, int nHitBoxY, bool checkVisible) {
	return checkCollision(getBlockID(nX + nHitBoxX, nY + nHitBoxY), checkVisible);
}

bool Map::checkCollisionRB(int nX, int nY, int nHitBoxX, int nHitBoxY, bool checkVisible) {
	return checkCollision(getBlockID(nX + nHitBoxX, nY + nHitBoxY), checkVisible);
}

bool Map::checkCollisionRT(int nX, int nY, int nHitBoxX, bool checkVisible) {
	return checkCollision(getBlockID(nX + nHitBoxX, nY), checkVisible);
}

int Map::checkCollisionWithPlatform(int nX, int nY, int iHitBoxX, int iHitBoxY) {
	for(unsigned int i = 0; i < vPlatform.size(); i++) {
		if(-fXPos + nX + iHitBoxX >= vPlatform[i]->getXPos() && - fXPos + nX <= vPlatform[i]->getXPos() + vPlatform[i]->getSize() * 16) {
			if(nY + iHitBoxY >= vPlatform[i]->getYPos() && nY + iHitBoxY <= vPlatform[i]->getYPos() + 16) {
				return i;
			}
		}
	}

	return -1;
}

bool Map::checkCollision(Vector2* nV, bool checkVisible) {
	bool output = vBlock[lMap[nV->getX()][nV->getY()]->getBlockID()]->getCollision() && (checkVisible ? vBlock[lMap[nV->getX()][nV->getY()]->getBlockID()]->getVisible() : true);
	delete nV;
	return output;
}

void Map::checkCollisionOnTopOfTheBlock(int nX, int nY) {
	switch(lMap[nX][nY + 1]->getBlockID()) {
		case 29: case 71: case 72: case 73:// COIN
			lMap[nX][nY + 1]->setBlockID(0);
			lCoin.push_back(new Coin(nX * 32 + 7, CCFG::GAME_HEIGHT - nY * 32 - 48));
			CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cCOIN);
			oPlayer->setCoins(oPlayer->getCoins() + 1);
			return;
			break;
	}

	for(int i = (nX - nX%5)/5, iEnd = i + 3; i < iEnd && i < iMinionListSize; i++) {
		for(unsigned int j = 0; j < lMinion[i].size(); j++) {
			if(!lMinion[i][j]->collisionOnlyWithPlayer && lMinion[i][j]->getMinionState() >= 0 && ((lMinion[i][j]->getXPos() >= nX*32 && lMinion[i][j]->getXPos() <= nX*32 + 32) || (lMinion[i][j]->getXPos() + lMinion[i][j]->iHitBoxX >= nX*32 && lMinion[i][j]->getXPos() + lMinion[i][j]->iHitBoxX <= nX*32 + 32))) {
				if(lMinion[i][j]->getYPos() + lMinion[i][j]->iHitBoxY >= CCFG::GAME_HEIGHT - 24 - nY*32 && lMinion[i][j]->getYPos() + lMinion[i][j]->iHitBoxY <= CCFG::GAME_HEIGHT - nY*32 + 16) {
					lMinion[i][j]->moveDirection = !lMinion[i][j]->moveDirection;
					lMinion[i][j]->setMinionState(-2);
				}
			}
		}
	}
}

/* ---------------- COLLISION ---------------- */
/* ******************************************** */
/* ----------------- MINIONS ----------------- */

int Map::getListID(int nX) {
	return (int)(nX / 160);
}

void Map::addPoints(int X, int Y, std::string sText, int iW, int iH) {
	lPoints.push_back(new Points(X, Y, sText, iW, iH));
}

void Map::addGoombas(int iX, int iY, bool moveDirection) {
	lMinion[getListID(iX)].push_back(new Goombas(iX, iY, iLevelType == 0 || iLevelType == 4 ? 0 : iLevelType == 1 ? 8 : 10, moveDirection));
}

void Map::addKoppa(int iX, int iY, int minionState, bool moveDirection) {
	int tempBlock;

	switch(minionState) {
		case 0: case 3:
			tempBlock = iLevelType == 0 || iLevelType == 4 ? 7 : iLevelType == 1 ? 14 : 17;
			break;
		case 1:
			tempBlock = iLevelType == 0 || iLevelType == 4 ? 4 : iLevelType == 1 ? 12 : 15;
			break;
		case 2:
			tempBlock = iLevelType == 0 || iLevelType == 4 ? 5 : iLevelType == 1 ? 13 : 16;
			break;
	}

	lMinion[getListID(iX)].push_back(new Koppa(iX, iY, minionState, moveDirection, tempBlock));
}

void Map::addBeetle(int X, int Y, bool moveDirection) {
	lMinion[getListID(X)].push_back(new Beetle(X, Y, moveDirection));
}

void Map::addPlant(int iX, int iY) {
	lMinion[getListID(iX)].push_back(new Plant(iX, iY, iLevelType == 0 || iLevelType == 4 ? 18 : 19));
}

void Map::addToad(int X, int Y, bool peach) {
	lMinion[getListID(X)].push_back(new Toad(X, Y, peach));
}

void Map::addSquid(int X, int Y) {
	lMinion[getListID(X)].push_back(new Squid(X, Y));
}

void Map::addHammer(int X, int Y, bool moveDirection) {
	lMinion[getListID(X)].push_back(new Hammer(X, Y, moveDirection));
}

void Map::addHammerBro(int X, int Y) {
	lMinion[getListID(X)].push_back(new HammerBro(X, Y));
}

void Map::addFireBall(int X, int Y, int iWidth, int iSliceID, bool DIR) {
	for(int i = 0; i < iWidth; i++) {
		lMinion[getListID((int)X)].push_back(new FireBall(X + 8, Y + 8, 14*i, iSliceID, DIR));
	}
}

void Map::addSpikey(int X, int Y) {
	lMinion[getListID(X)].push_back(new Spikey(X, Y));
}

void Map::addPlayerFireBall(int X, int Y, bool moveDirection) {
	lMinion[getListID(X)].push_back(new PlayerFireBall(X, Y, moveDirection));
}

void Map::addBowser(int X, int Y, bool spawnHammer) {
	lMinion[getListID(X)].push_back(new Bowser((float)X, (float)Y, spawnHammer));
}

void Map::addUpFire(int X, int iYEnd) {
	lMinion[getListID(X)].push_back(new UpFire(X, iYEnd));
}

void Map::addFire(float fX, float fY, int toYPos) {
	lMinion[getListID((int)fX)].push_back(new Fire(fX, fY, toYPos));
}

void Map::addCheep(int X, int Y, int minionType, int moveSpeed, bool moveDirection) {
	lMinion[getListID(X)].push_back(new Cheep(X, Y, minionType, moveSpeed, moveDirection));
}

void Map::addCheepSpawner(int X, int XEnd) {
	lMinion[getListID(X)].push_back(new CheepSpawner(X, XEnd));
}

void Map::addBubble(int X, int Y) {
	lBubble.push_back(new Bubble(X, Y));
}

void Map::addLakito(int X, int Y, int iMaxXPos) {
	lMinion[getListID(X)].push_back(new Lakito(X, Y, iMaxXPos));
}

void Map::addVine(int X, int Y, int minionState, int iBlockID) {
	lMinion[getListID(X)].push_back(new Vine(X, Y, minionState, iBlockID));
	if(minionState == 0) {
		CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cVINE);
	}
}

void Map::addSpring(int X, int Y) {
	lMinion[getListID(X)].push_back(new Spring(X, Y));
	//lMap[X/32][(CCFG::GAME_HEIGHT - 16 - Y)/32 - 1]->setBlockID(83);
}

void Map::addBulletBillSpawner(int X, int Y, int minionState) {
	lMinion[getListID(X*32)].push_back(new BulletBillSpawner(X*32, CCFG::GAME_HEIGHT - Y*32, minionState));
}

void Map::addBulletBill(int X, int Y, bool moveDirection, int minionState) {
	lMinion[getListID(X)].push_back(new BulletBill(X, Y, moveDirection, minionState));
}

void Map::lockMinions() {
	for(unsigned int i = 0; i < lMinion.size(); i++) {
		for(unsigned int j = 0; j < lMinion[i].size(); j++) {
			lMinion[i][j]->lockMinion();
		}
	}
}
void Map::addText(int X, int Y, std::string sText) {
	vLevelText.push_back(new LevelText(X, Y, sText));
}

int Map::getNumOfMinions() {
	int iOutput = 0;

	for(int i = 0, size = lMinion.size(); i < size; i++) {
		iOutput += lMinion[i].size();
	}

	return iOutput;
}

/* ----------------- MINIONS ----------------- */
/* ******************************************** */

/* ---------- LOAD GAME DATA ---------- */

void Map::loadGameData(SDL_Renderer* rR) {
	std::vector<std::string> tSprite;
	std::vector<unsigned int> iDelay;
	
	// ----- 0 Transparent -----
	tSprite.push_back("transp");
	iDelay.push_back(0);
	vBlock.push_back(new Block(0, new Sprite(rR, tSprite, iDelay, false), false, true, false, false));
	tSprite.clear();
	iDelay.clear();
	// ----- 1 -----
	tSprite.push_back("gnd_red_1");
	iDelay.push_back(0);
	vBlock.push_back(new Block(1, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 2 -----
	tSprite.push_back("coin_0");
	iDelay.push_back(300);
	tSprite.push_back("coin_2");
	iDelay.push_back(30);
	tSprite.push_back("coin_1");
	iDelay.push_back(130);
	tSprite.push_back("coin_2");
	iDelay.push_back(140);
	vBlock.push_back(new Block(2, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 3 -----
	tSprite.push_back("bush_center_0");
	iDelay.push_back(0);
	vBlock.push_back(new Block(3, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 4 -----
	tSprite.push_back("bush_center_1");
	iDelay.push_back(0);
	vBlock.push_back(new Block(4, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 5 -----
	tSprite.push_back("bush_left");
	iDelay.push_back(0);
	vBlock.push_back(new Block(5, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 6 -----
	tSprite.push_back("bush_right");
	iDelay.push_back(0);
	vBlock.push_back(new Block(6, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 7 -----
	tSprite.push_back("bush_top");
	iDelay.push_back(0);
	vBlock.push_back(new Block(7, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 8 -----
	tSprite.push_back("blockq_0");
	iDelay.push_back(300);
	tSprite.push_back("blockq_2");
	iDelay.push_back(30);
	tSprite.push_back("blockq_1");
	iDelay.push_back(130);
	tSprite.push_back("blockq_2");
	iDelay.push_back(140);
	vBlock.push_back(new Block(8, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 9 -----
	tSprite.push_back("blockq_used");
	iDelay.push_back(0);
	vBlock.push_back(new Block(9, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 10 -----
	tSprite.push_back("grass_left");
	iDelay.push_back(0);
	vBlock.push_back(new Block(10, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 11 -----
	tSprite.push_back("grass_center");
	iDelay.push_back(0);
	vBlock.push_back(new Block(11, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 12 -----
	tSprite.push_back("grass_right");
	iDelay.push_back(0);
	vBlock.push_back(new Block(12, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 13 -----
	tSprite.push_back("brickred");
	iDelay.push_back(0);
	vBlock.push_back(new Block(13, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 14 -----
	tSprite.push_back("cloud_left_bot");
	iDelay.push_back(0);
	vBlock.push_back(new Block(14, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 15 -----
	tSprite.push_back("cloud_left_top");
	iDelay.push_back(0);
	vBlock.push_back(new Block(15, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 16 -----
	tSprite.push_back("cloud_center_bot");
	iDelay.push_back(0);
	vBlock.push_back(new Block(16, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 17 -----
	tSprite.push_back("cloud_center_top");
	iDelay.push_back(0);
	vBlock.push_back(new Block(17, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 18 -----
	tSprite.push_back("cloud_right_bot");
	iDelay.push_back(0);
	vBlock.push_back(new Block(18, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 19 -----
	tSprite.push_back("cloud_right_top");
	iDelay.push_back(0);
	vBlock.push_back(new Block(19, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 20 -----
	tSprite.push_back("pipe_left_bot");
	iDelay.push_back(0);
	vBlock.push_back(new Block(20, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 21 -----
	tSprite.push_back("pipe_left_top");
	iDelay.push_back(0);
	vBlock.push_back(new Block(21, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 22 -----
	tSprite.push_back("pipe_right_bot");
	iDelay.push_back(0);
	vBlock.push_back(new Block(22, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 23 -----
	tSprite.push_back("pipe_right_top");
	iDelay.push_back(0);
	vBlock.push_back(new Block(23, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 24 BlockQ2 -----
	tSprite.push_back("transp");
	iDelay.push_back(0);
	vBlock.push_back(new Block(24, new Sprite(rR, tSprite, iDelay, false), true, false, true, false));
	tSprite.clear();
	iDelay.clear();
	// ----- 25 -----
	tSprite.push_back("gnd_red2");
	iDelay.push_back(0);
	vBlock.push_back(new Block(25, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 26 -----
	tSprite.push_back("gnd1");
	iDelay.push_back(0);
	vBlock.push_back(new Block(26, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 27 -----
	tSprite.push_back("gnd1_2");
	iDelay.push_back(0);
	vBlock.push_back(new Block(27, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 28 -----
	tSprite.push_back("brick1");
	iDelay.push_back(0);
	vBlock.push_back(new Block(28, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 29 -----
	tSprite.push_back("coin_use0");
	iDelay.push_back(300);
	tSprite.push_back("coin_use2");
	iDelay.push_back(30);
	tSprite.push_back("coin_use1");
	iDelay.push_back(130);
	tSprite.push_back("coin_use2");
	iDelay.push_back(140);
	vBlock.push_back(new Block(29, new Sprite(rR, tSprite, iDelay, false), false, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 30 -----
	tSprite.push_back("pipe1_left_bot");
	iDelay.push_back(0);
	vBlock.push_back(new Block(30, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 31 -----
	tSprite.push_back("pipe1_left_top");
	iDelay.push_back(0);
	vBlock.push_back(new Block(31, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 32 -----
	tSprite.push_back("pipe1_right_bot");
	iDelay.push_back(0);
	vBlock.push_back(new Block(32, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 33 -----
	tSprite.push_back("pipe1_right_top");
	iDelay.push_back(0);
	vBlock.push_back(new Block(33, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 34 -----
	tSprite.push_back("pipe1_hor_bot_right");
	iDelay.push_back(0);
	vBlock.push_back(new Block(34, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 35 -----
	tSprite.push_back("pipe1_hor_top_center");
	iDelay.push_back(0);
	vBlock.push_back(new Block(35, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 36 -----
	tSprite.push_back("pipe1_hor_top_left");
	iDelay.push_back(0);
	vBlock.push_back(new Block(36, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 37 -----
	tSprite.push_back("pipe1_hor_bot_center");
	iDelay.push_back(0);
	vBlock.push_back(new Block(37, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 38 -----
	tSprite.push_back("pipe1_hor_bot_left");
	iDelay.push_back(0);
	vBlock.push_back(new Block(38, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 39 -----
	tSprite.push_back("pipe1_hor_top_right");
	iDelay.push_back(0);
	vBlock.push_back(new Block(39, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 40 -----
	tSprite.push_back("end0_l");
	iDelay.push_back(0);
	vBlock.push_back(new Block(40, new Sprite(rR, tSprite, iDelay, false), false, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 41 -----
	tSprite.push_back("end0_dot");
	iDelay.push_back(0);
	vBlock.push_back(new Block(41, new Sprite(rR, tSprite, iDelay, false), false, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 42 -----
	tSprite.push_back("end0_flag");
	iDelay.push_back(0);
	vBlock.push_back(new Block(42, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 43 -----
	tSprite.push_back("castle0_brick");
	iDelay.push_back(0);
	vBlock.push_back(new Block(43, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 44 -----
	tSprite.push_back("castle0_top0");
	iDelay.push_back(0);
	vBlock.push_back(new Block(44, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 45 -----
	tSprite.push_back("castle0_top1");
	iDelay.push_back(0);
	vBlock.push_back(new Block(45, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 46 -----
	tSprite.push_back("castle0_center_center_top");
	iDelay.push_back(0);
	vBlock.push_back(new Block(46, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 47 -----
	tSprite.push_back("castle0_center_center");
	iDelay.push_back(0);
	vBlock.push_back(new Block(47, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 48 -----
	tSprite.push_back("castle0_center_left");
	iDelay.push_back(0);
	vBlock.push_back(new Block(48, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 49 -----
	tSprite.push_back("castle0_center_right");
	iDelay.push_back(0);
	vBlock.push_back(new Block(49, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 50 -----
	tSprite.push_back("coin_an0");
	iDelay.push_back(0);
	tSprite.push_back("coin_an1");
	iDelay.push_back(0);
	tSprite.push_back("coin_an2");
	iDelay.push_back(0);
	tSprite.push_back("coin_an3");
	iDelay.push_back(0);
	vBlock.push_back(new Block(50, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 51 -----
	tSprite.push_back("castle_flag");
	iDelay.push_back(0);
	vBlock.push_back(new Block(51, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 52 -----
	tSprite.push_back("firework0");
	iDelay.push_back(0);
	vBlock.push_back(new Block(52, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 53 -----
	tSprite.push_back("firework1");
	iDelay.push_back(0);
	vBlock.push_back(new Block(53, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 54 -----
	tSprite.push_back("firework2");
	iDelay.push_back(0);
	vBlock.push_back(new Block(54, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 55 -----
	tSprite.push_back("blockq1_0");
	iDelay.push_back(300);
	tSprite.push_back("blockq1_2");
	iDelay.push_back(30);
	tSprite.push_back("blockq1_1");
	iDelay.push_back(130);
	tSprite.push_back("blockq1_2");
	iDelay.push_back(140);
	vBlock.push_back(new Block(55, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 56 -----
	tSprite.push_back("blockq1_used");
	iDelay.push_back(0);
	vBlock.push_back(new Block(56, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 57 -----
	tSprite.push_back("coin1_0");
	iDelay.push_back(300);
	tSprite.push_back("coin1_2");
	iDelay.push_back(30);
	tSprite.push_back("coin1_1");
	iDelay.push_back(130);
	tSprite.push_back("coin1_2");
	iDelay.push_back(140);
	vBlock.push_back(new Block(57, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 58 -----
	tSprite.push_back("pipe_hor_bot_right");
	iDelay.push_back(0);
	vBlock.push_back(new Block(58, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 59 -----
	tSprite.push_back("pipe_hor_top_center");
	iDelay.push_back(0);
	vBlock.push_back(new Block(59, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 60 -----
	tSprite.push_back("pipe_hor_top_left");
	iDelay.push_back(0);
	vBlock.push_back(new Block(60, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 61 -----
	tSprite.push_back("pipe_hor_bot_center");
	iDelay.push_back(0);
	vBlock.push_back(new Block(61, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 62 -----
	tSprite.push_back("pipe_hor_bot_left");
	iDelay.push_back(0);
	vBlock.push_back(new Block(62, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 63 -----
	tSprite.push_back("pipe_hor_top_right");
	iDelay.push_back(0);
	vBlock.push_back(new Block(63, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 64 -----
	tSprite.push_back("block_debris0");
	iDelay.push_back(0);
	vBlock.push_back(new Block(64, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 65 -----
	tSprite.push_back("block_debris1");
	iDelay.push_back(0);
	vBlock.push_back(new Block(65, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 66 -----
	tSprite.push_back("block_debris2");
	iDelay.push_back(0);
	vBlock.push_back(new Block(66, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 67 -----
	tSprite.push_back("t_left");
	iDelay.push_back(0);
	vBlock.push_back(new Block(67, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 68 -----
	tSprite.push_back("t_center");
	iDelay.push_back(0);
	vBlock.push_back(new Block(68, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 69 -----
	tSprite.push_back("t_right");
	iDelay.push_back(0);
	vBlock.push_back(new Block(69, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 70 -----
	tSprite.push_back("t_bot");
	iDelay.push_back(0);
	vBlock.push_back(new Block(70, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 71 -----
	tSprite.push_back("coin_use00");
	iDelay.push_back(300);
	tSprite.push_back("coin_use02");
	iDelay.push_back(30);
	tSprite.push_back("coin_use01");
	iDelay.push_back(130);
	tSprite.push_back("coin_use02");
	iDelay.push_back(140);
	vBlock.push_back(new Block(71, new Sprite(rR, tSprite, iDelay, false), false, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 72 -----
	tSprite.push_back("coin_use20");
	iDelay.push_back(300);
	tSprite.push_back("coin_use22");
	iDelay.push_back(30);
	tSprite.push_back("coin_use21");
	iDelay.push_back(130);
	tSprite.push_back("coin_use22");
	iDelay.push_back(140);
	vBlock.push_back(new Block(72, new Sprite(rR, tSprite, iDelay, false), false, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 73 -----
	tSprite.push_back("coin_use30");
	iDelay.push_back(300);
	tSprite.push_back("coin_use32");
	iDelay.push_back(30);
	tSprite.push_back("coin_use31");
	iDelay.push_back(130);
	tSprite.push_back("coin_use32");
	iDelay.push_back(140);
	vBlock.push_back(new Block(73, new Sprite(rR, tSprite, iDelay, false), false, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 74 -----
	tSprite.push_back("platform");
	iDelay.push_back(0);
	vBlock.push_back(new Block(74, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 75 -----
	tSprite.push_back("gnd_4");
	iDelay.push_back(0);
	vBlock.push_back(new Block(75, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 76 -----
	tSprite.push_back("bridge_0");
	iDelay.push_back(0);
	vBlock.push_back(new Block(76, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 77 -----
	tSprite.push_back("lava_0");
	iDelay.push_back(0);
	vBlock.push_back(new Block(77, new Sprite(rR, tSprite, iDelay, false), false, true, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 78 -----
	tSprite.push_back("lava_1");
	iDelay.push_back(0);
	vBlock.push_back(new Block(78, new Sprite(rR, tSprite, iDelay, false), false, true, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 79 -----
	tSprite.push_back("bridge_1");
	iDelay.push_back(0);
	vBlock.push_back(new Block(78, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 80 -----
	tSprite.push_back("blockq2_used");
	iDelay.push_back(0);
	vBlock.push_back(new Block(80, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 81 -----
	tSprite.push_back("brick2");
	iDelay.push_back(0);
	vBlock.push_back(new Block(81, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 82 -----
	tSprite.push_back("axe_0");
	iDelay.push_back(225);
	tSprite.push_back("axe_1");
	iDelay.push_back(225);
	tSprite.push_back("axe_2");
	iDelay.push_back(200);
	vBlock.push_back(new Block(82, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 83 ----- END BRIDGE
	tSprite.push_back("transp");
	iDelay.push_back(0);
	vBlock.push_back(new Block(83, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 84 -----
	tSprite.push_back("tree_1");
	iDelay.push_back(0);
	vBlock.push_back(new Block(84, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 85 -----
	tSprite.push_back("tree_2");
	iDelay.push_back(0);
	vBlock.push_back(new Block(85, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 86 -----
	tSprite.push_back("tree_3");
	iDelay.push_back(0);
	vBlock.push_back(new Block(86, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 87 -----
	tSprite.push_back("tree1_1");
	iDelay.push_back(0);
	vBlock.push_back(new Block(87, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 88 -----
	tSprite.push_back("tree1_2");
	iDelay.push_back(0);
	vBlock.push_back(new Block(88, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 89 -----
	tSprite.push_back("tree1_3");
	iDelay.push_back(0);
	vBlock.push_back(new Block(89, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 90 -----
	tSprite.push_back("fence");
	iDelay.push_back(0);
	vBlock.push_back(new Block(90, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 91 -----
	tSprite.push_back("tree_0");
	iDelay.push_back(0);
	vBlock.push_back(new Block(91, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 92 -----
	tSprite.push_back("uw_0");
	iDelay.push_back(0);
	vBlock.push_back(new Block(92, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 93 -----
	tSprite.push_back("uw_1");
	iDelay.push_back(0);
	vBlock.push_back(new Block(93, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 94 -----
	tSprite.push_back("water_0");
	iDelay.push_back(0);
	vBlock.push_back(new Block(94, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 95 -----
	tSprite.push_back("water_1");
	iDelay.push_back(0);
	vBlock.push_back(new Block(95, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 96 -----
	tSprite.push_back("bubble");
	iDelay.push_back(0);
	vBlock.push_back(new Block(96, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 97 -----
	tSprite.push_back("pipe2_left_bot");
	iDelay.push_back(0);
	vBlock.push_back(new Block(97, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 98 -----
	tSprite.push_back("pipe2_left_top");
	iDelay.push_back(0);
	vBlock.push_back(new Block(98, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 99 -----
	tSprite.push_back("pipe2_right_bot");
	iDelay.push_back(0);
	vBlock.push_back(new Block(99, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 100 -----
	tSprite.push_back("pipe2_right_top");
	iDelay.push_back(0);
	vBlock.push_back(new Block(100, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 101 -----
	tSprite.push_back("pipe2_hor_bot_right");
	iDelay.push_back(0);
	vBlock.push_back(new Block(101, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 102 -----
	tSprite.push_back("pipe2_hor_top_center");
	iDelay.push_back(0);
	vBlock.push_back(new Block(102, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 103 -----
	tSprite.push_back("pipe2_hor_top_left");
	iDelay.push_back(0);
	vBlock.push_back(new Block(103, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 104 -----
	tSprite.push_back("pipe2_hor_bot_center");
	iDelay.push_back(0);
	vBlock.push_back(new Block(104, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 105 -----
	tSprite.push_back("pipe2_hor_bot_left");
	iDelay.push_back(0);
	vBlock.push_back(new Block(105, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 106 -----
	tSprite.push_back("pipe2_hor_top_right");
	iDelay.push_back(0);
	vBlock.push_back(new Block(106, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 107 -----
	tSprite.push_back("bridge2");
	iDelay.push_back(0);
	vBlock.push_back(new Block(107, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 108 -----
	tSprite.push_back("bridge3");
	iDelay.push_back(0);
	vBlock.push_back(new Block(108, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 109 -----
	tSprite.push_back("platform1");
	iDelay.push_back(0);
	vBlock.push_back(new Block(109, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 110 -----
	tSprite.push_back("water_2");
	iDelay.push_back(0);
	vBlock.push_back(new Block(110, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 111 -----
	tSprite.push_back("water_3");
	iDelay.push_back(0);
	vBlock.push_back(new Block(111, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 112 -----
	tSprite.push_back("pipe3_left_bot");
	iDelay.push_back(0);
	vBlock.push_back(new Block(112, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 113 -----
	tSprite.push_back("pipe3_left_top");
	iDelay.push_back(0);
	vBlock.push_back(new Block(113, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 114 -----
	tSprite.push_back("pipe3_right_bot");
	iDelay.push_back(0);
	vBlock.push_back(new Block(114, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 115 -----
	tSprite.push_back("pipe3_right_top");
	iDelay.push_back(0);
	vBlock.push_back(new Block(115, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 116 -----
	tSprite.push_back("pipe3_hor_bot_right");
	iDelay.push_back(0);
	vBlock.push_back(new Block(116, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 117 -----
	tSprite.push_back("pipe3_hor_top_center");
	iDelay.push_back(0);
	vBlock.push_back(new Block(117, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 118 -----
	tSprite.push_back("pipe3_hor_top_left");
	iDelay.push_back(0);
	vBlock.push_back(new Block(118, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 119 -----
	tSprite.push_back("pipe3_hor_bot_center");
	iDelay.push_back(0);
	vBlock.push_back(new Block(119, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 120 -----
	tSprite.push_back("pipe3_hor_bot_left");
	iDelay.push_back(0);
	vBlock.push_back(new Block(120, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 121 -----
	tSprite.push_back("pipe3_hor_top_right");
	iDelay.push_back(0);
	vBlock.push_back(new Block(121, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 122 -----
	tSprite.push_back("bridge4");
	iDelay.push_back(0);
	vBlock.push_back(new Block(122, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 123 -----
	tSprite.push_back("end1_l");
	iDelay.push_back(0);
	vBlock.push_back(new Block(123, new Sprite(rR, tSprite, iDelay, false), false, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 124 -----
	tSprite.push_back("end1_dot");
	iDelay.push_back(0);
	vBlock.push_back(new Block(124, new Sprite(rR, tSprite, iDelay, false), false, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 125 -----
	tSprite.push_back("bonus");
	iDelay.push_back(0);
	vBlock.push_back(new Block(125, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 126 -----
	tSprite.push_back("platformbonus");
	iDelay.push_back(0);
	vBlock.push_back(new Block(126, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 127 ----- // -- BONUS END
	tSprite.push_back("transp");
	iDelay.push_back(0);
	vBlock.push_back(new Block(127, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 128 ----- // -- SPAWN VINE
	tSprite.push_back("brickred");
	iDelay.push_back(0);
	vBlock.push_back(new Block(128, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 129 ----- // -- SPAWN VINE
	tSprite.push_back("brick1");
	iDelay.push_back(0);
	vBlock.push_back(new Block(129, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 130 -----
	tSprite.push_back("vine");
	iDelay.push_back(0);
	vBlock.push_back(new Block(130, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 131 -----
	tSprite.push_back("vine1");
	iDelay.push_back(0);
	vBlock.push_back(new Block(131, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 132 -----
	tSprite.push_back("seesaw_0");
	iDelay.push_back(0);
	vBlock.push_back(new Block(132, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 133 -----
	tSprite.push_back("seesaw_1");
	iDelay.push_back(0);
	vBlock.push_back(new Block(133, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 134 -----
	tSprite.push_back("seesaw_2");
	iDelay.push_back(0);
	vBlock.push_back(new Block(134, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 135 -----
	tSprite.push_back("seesaw_3");
	iDelay.push_back(0);
	vBlock.push_back(new Block(135, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 136 -----
	tSprite.push_back("pipe4_left_bot");
	iDelay.push_back(0);
	vBlock.push_back(new Block(136, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 137 -----
	tSprite.push_back("pipe4_left_top");
	iDelay.push_back(0);
	vBlock.push_back(new Block(137, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 138 -----
	tSprite.push_back("pipe4_right_bot");
	iDelay.push_back(0);
	vBlock.push_back(new Block(138, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 139 -----
	tSprite.push_back("pipe4_right_top");
	iDelay.push_back(0);
	vBlock.push_back(new Block(139, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 140 -----
	tSprite.push_back("t_left1");
	iDelay.push_back(0);
	vBlock.push_back(new Block(140, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 141 -----
	tSprite.push_back("t_center1");
	iDelay.push_back(0);
	vBlock.push_back(new Block(141, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 142 -----
	tSprite.push_back("t_right1");
	iDelay.push_back(0);
	vBlock.push_back(new Block(142, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 143 -----
	tSprite.push_back("t_bot0");
	iDelay.push_back(0);
	vBlock.push_back(new Block(143, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 144 -----
	tSprite.push_back("t_bot1");
	iDelay.push_back(0);
	vBlock.push_back(new Block(144, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 145 -----
	tSprite.push_back("b_top");
	iDelay.push_back(0);
	vBlock.push_back(new Block(145, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 146 -----
	tSprite.push_back("b_top1");
	iDelay.push_back(0);
	vBlock.push_back(new Block(146, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 147 -----
	tSprite.push_back("b_bot");
	iDelay.push_back(0);
	vBlock.push_back(new Block(147, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 148 -----
	tSprite.push_back("cloud_left_bot1");
	iDelay.push_back(0);
	vBlock.push_back(new Block(148, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 149 -----
	tSprite.push_back("cloud_center_bot1");
	iDelay.push_back(0);
	vBlock.push_back(new Block(149, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 150 -----
	tSprite.push_back("cloud_center_top1");
	iDelay.push_back(0);
	vBlock.push_back(new Block(150, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 151 -----
	tSprite.push_back("t_left2");
	iDelay.push_back(0);
	vBlock.push_back(new Block(151, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 152 -----
	tSprite.push_back("t_center2");
	iDelay.push_back(0);
	vBlock.push_back(new Block(152, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 153 -----
	tSprite.push_back("t_right2");
	iDelay.push_back(0);
	vBlock.push_back(new Block(153, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 154 -----
	tSprite.push_back("t_bot2");
	iDelay.push_back(0);
	vBlock.push_back(new Block(154, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 155 -----
	tSprite.push_back("castle1_brick");
	iDelay.push_back(0);
	vBlock.push_back(new Block(155, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 156 -----
	tSprite.push_back("castle1_top0");
	iDelay.push_back(0);
	vBlock.push_back(new Block(156, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 157 -----
	tSprite.push_back("castle1_top1");
	iDelay.push_back(0);
	vBlock.push_back(new Block(157, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 158 -----
	tSprite.push_back("castle1_center_center_top");
	iDelay.push_back(0);
	vBlock.push_back(new Block(158, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 159 -----
	tSprite.push_back("castle1_center_center");
	iDelay.push_back(0);
	vBlock.push_back(new Block(159, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 160 -----
	tSprite.push_back("castle1_center_left");
	iDelay.push_back(0);
	vBlock.push_back(new Block(160, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 161 -----
	tSprite.push_back("castle1_center_right");
	iDelay.push_back(0);
	vBlock.push_back(new Block(161, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 162 -----
	tSprite.push_back("seesaw1_0");
	iDelay.push_back(0);
	vBlock.push_back(new Block(162, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 163 -----
	tSprite.push_back("seesaw1_1");
	iDelay.push_back(0);
	vBlock.push_back(new Block(163, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 164 -----
	tSprite.push_back("seesaw1_2");
	iDelay.push_back(0);
	vBlock.push_back(new Block(164, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 165 -----
	tSprite.push_back("seesaw1_3");
	iDelay.push_back(0);
	vBlock.push_back(new Block(165, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 166 -----
	tSprite.push_back("gnd2");
	iDelay.push_back(0);
	vBlock.push_back(new Block(166, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 167 -----
	tSprite.push_back("gnd2_2");
	iDelay.push_back(0);
	vBlock.push_back(new Block(167, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 168 -----
	tSprite.push_back("end1_flag");
	iDelay.push_back(0);
	vBlock.push_back(new Block(168, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 169 ----- TP
	tSprite.push_back("transp");
	iDelay.push_back(0);
	vBlock.push_back(new Block(169, new Sprite(rR, tSprite, iDelay, false), false, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 170 ----- TP2
	tSprite.push_back("transp");
	iDelay.push_back(0);
	vBlock.push_back(new Block(170, new Sprite(rR, tSprite, iDelay, false), false, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 171 ----- TP3 - bTP
	tSprite.push_back("transp");
	iDelay.push_back(0);
	vBlock.push_back(new Block(171, new Sprite(rR, tSprite, iDelay, false), false, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 172 -----
	tSprite.push_back("pipe5_left_bot");
	iDelay.push_back(0);
	vBlock.push_back(new Block(172, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 173 -----
	tSprite.push_back("pipe5_left_top");
	iDelay.push_back(0);
	vBlock.push_back(new Block(173, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 174 -----
	tSprite.push_back("pipe5_right_bot");
	iDelay.push_back(0);
	vBlock.push_back(new Block(174, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 175 -----
	tSprite.push_back("pipe5_right_top");
	iDelay.push_back(0);
	vBlock.push_back(new Block(175, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 176 -----
	tSprite.push_back("pipe6_left_bot");
	iDelay.push_back(0);
	vBlock.push_back(new Block(176, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 177 -----
	tSprite.push_back("pipe6_left_top");
	iDelay.push_back(0);
	vBlock.push_back(new Block(177, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 178 -----
	tSprite.push_back("pipe6_right_bot");
	iDelay.push_back(0);
	vBlock.push_back(new Block(178, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 179 -----
	tSprite.push_back("pipe6_right_top");
	iDelay.push_back(0);
	vBlock.push_back(new Block(179, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 180 -----
	tSprite.push_back("crown");
	iDelay.push_back(0);
	vBlock.push_back(new Block(180, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 181 -----
	tSprite.push_back("gnd_5");
	iDelay.push_back(0);
	vBlock.push_back(new Block(166, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 182 ----- ENDUSE
	tSprite.push_back("transp");
	iDelay.push_back(0);
	vBlock.push_back(new Block(182, new Sprite(rR, tSprite, iDelay, false), false, false, true, true));
	tSprite.clear();
	iDelay.clear();


	// --------------- MINION ---------------

	// ----- 0 -----
	tSprite.push_back("goombas_0");
	iDelay.push_back(200);
	tSprite.push_back("goombas_1");
	iDelay.push_back(200);
	vMinion.push_back(new Block(0, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 1 -----
	tSprite.push_back("goombas_ded");
	iDelay.push_back(0);
	vMinion.push_back(new Block(1, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 2 -----
	tSprite.push_back("mushroom");
	iDelay.push_back(0);
	vMinion.push_back(new Block(2, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 3 -----
	tSprite.push_back("mushroom_1up");
	iDelay.push_back(0);
	vMinion.push_back(new Block(3, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 4 -----
	tSprite.push_back("koopa_0");
	iDelay.push_back(200);
	tSprite.push_back("koopa_1");
	iDelay.push_back(200);
	vMinion.push_back(new Block(4, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 5 -----
	tSprite.push_back("koopa_ded");
	iDelay.push_back(0);
	vMinion.push_back(new Block(5, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 6 -----
	tSprite.push_back("flower0");
	iDelay.push_back(50);
	tSprite.push_back("flower1");
	iDelay.push_back(50);
	tSprite.push_back("flower2");
	iDelay.push_back(50);
	tSprite.push_back("flower3");
	iDelay.push_back(50);
	vMinion.push_back(new Block(6, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 7 -----
	tSprite.push_back("koopa_2");
	iDelay.push_back(200);
	tSprite.push_back("koopa_3");
	iDelay.push_back(200);
	vMinion.push_back(new Block(7, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 8 -----
	tSprite.push_back("goombas1_0");
	iDelay.push_back(200);
	tSprite.push_back("goombas1_1");
	iDelay.push_back(200);
	vMinion.push_back(new Block(8, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 9 -----
	tSprite.push_back("goombas1_ded");
	iDelay.push_back(0);
	vMinion.push_back(new Block(9, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 10 -----
	tSprite.push_back("goombas2_0");
	iDelay.push_back(200);
	tSprite.push_back("goombas2_1");
	iDelay.push_back(200);
	vMinion.push_back(new Block(10, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 11 -----
	tSprite.push_back("goombas2_ded");
	iDelay.push_back(0);
	vMinion.push_back(new Block(11, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 12 -----
	tSprite.push_back("koopa1_0");
	iDelay.push_back(200);
	tSprite.push_back("koopa1_1");
	iDelay.push_back(200);
	vMinion.push_back(new Block(12, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 13 -----
	tSprite.push_back("koopa1_ded");
	iDelay.push_back(0);
	vMinion.push_back(new Block(13, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 14 -----
	tSprite.push_back("koopa1_2");
	iDelay.push_back(200);
	tSprite.push_back("koopa1_3");
	iDelay.push_back(200);
	vMinion.push_back(new Block(14, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 15 -----
	tSprite.push_back("koopa2_0");
	iDelay.push_back(200);
	tSprite.push_back("koopa2_1");
	iDelay.push_back(200);
	vMinion.push_back(new Block(15, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 16 -----
	tSprite.push_back("koopa2_ded");
	iDelay.push_back(0);
	vMinion.push_back(new Block(16, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 17 -----
	tSprite.push_back("koopa2_2");
	iDelay.push_back(200);
	tSprite.push_back("koopa2_3");
	iDelay.push_back(200);
	vMinion.push_back(new Block(17, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 18 -----
	tSprite.push_back("plant_0");
	iDelay.push_back(125);
	tSprite.push_back("plant_1");
	iDelay.push_back(125);
	vMinion.push_back(new Block(18, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 19 -----
	tSprite.push_back("plant1_0");
	iDelay.push_back(125);
	tSprite.push_back("plant1_1");
	iDelay.push_back(125);
	vMinion.push_back(new Block(19, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 20 -----
	tSprite.push_back("bowser0");
	iDelay.push_back(285);
	tSprite.push_back("bowser1");
	iDelay.push_back(285);
	vMinion.push_back(new Block(20, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 21 -----
	tSprite.push_back("bowser2");
	iDelay.push_back(285);
	tSprite.push_back("bowser3");
	iDelay.push_back(285);
	vMinion.push_back(new Block(21, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 22 -----
	tSprite.push_back("fire_0");
	iDelay.push_back(35);
	tSprite.push_back("fire_1");
	iDelay.push_back(35);
	vMinion.push_back(new Block(22, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 23 -----
	tSprite.push_back("fireball_0");
	iDelay.push_back(75);
	tSprite.push_back("fireball_1");
	iDelay.push_back(75);
	tSprite.push_back("fireball_2");
	iDelay.push_back(75);
	tSprite.push_back("fireball_3");
	iDelay.push_back(75);
	vMinion.push_back(new Block(23, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 24 -----
	tSprite.push_back("star_0");
	iDelay.push_back(75);
	tSprite.push_back("star_1");
	iDelay.push_back(75);
	tSprite.push_back("star_2");
	iDelay.push_back(75);
	tSprite.push_back("star_3");
	iDelay.push_back(75);
	vMinion.push_back(new Block(24, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 25 -----
	tSprite.push_back("mushroom1_1up");
	iDelay.push_back(0);
	vMinion.push_back(new Block(25, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 26 -----
	tSprite.push_back("toad");
	iDelay.push_back(0);
	vMinion.push_back(new Block(26, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 27 -----
	tSprite.push_back("peach");
	iDelay.push_back(0);
	vMinion.push_back(new Block(27, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 28 -----
	tSprite.push_back("squid0");
	iDelay.push_back(0);
	vMinion.push_back(new Block(28, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 29 -----
	tSprite.push_back("squid1");
	iDelay.push_back(0);
	vMinion.push_back(new Block(29, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 30 -----
	tSprite.push_back("cheep0");
	iDelay.push_back(120);
	tSprite.push_back("cheep1");
	iDelay.push_back(120);
	vMinion.push_back(new Block(30, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 31 -----
	tSprite.push_back("cheep2");
	iDelay.push_back(110);
	tSprite.push_back("cheep3");
	iDelay.push_back(110);
	vMinion.push_back(new Block(31, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 32 -----
	tSprite.push_back("upfire");
	iDelay.push_back(0);
	vMinion.push_back(new Block(32, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 33 -----
	tSprite.push_back("vine_top");
	iDelay.push_back(0);
	vMinion.push_back(new Block(33, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 34 -----
	tSprite.push_back("vine");
	iDelay.push_back(0);
	vMinion.push_back(new Block(34, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 35 -----
	tSprite.push_back("vine1_top");
	iDelay.push_back(0);
	vMinion.push_back(new Block(35, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 36 -----
	tSprite.push_back("vine1");
	iDelay.push_back(0);
	vMinion.push_back(new Block(36, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 37 -----
	tSprite.push_back("spring_0");
	iDelay.push_back(0);
	vMinion.push_back(new Block(37, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 38 -----
	tSprite.push_back("spring_1");
	iDelay.push_back(0);
	vMinion.push_back(new Block(38, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 39 -----
	tSprite.push_back("spring_2");
	iDelay.push_back(0);
	vMinion.push_back(new Block(39, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 40 -----
	tSprite.push_back("spring1_0");
	iDelay.push_back(0);
	vMinion.push_back(new Block(40, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 41 -----
	tSprite.push_back("spring1_1");
	iDelay.push_back(0);
	vMinion.push_back(new Block(41, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 42 -----
	tSprite.push_back("spring1_2");
	iDelay.push_back(0);
	vMinion.push_back(new Block(42, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 43 -----
	tSprite.push_back("hammerbro_0");
	iDelay.push_back(175);
	tSprite.push_back("hammerbro_1");
	iDelay.push_back(175);
	vMinion.push_back(new Block(43, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 44 -----
	tSprite.push_back("hammerbro_2");
	iDelay.push_back(155);
	tSprite.push_back("hammerbro_3");
	iDelay.push_back(155);
	vMinion.push_back(new Block(44, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 45 -----
	tSprite.push_back("hammerbro1_0");
	iDelay.push_back(175);
	tSprite.push_back("hammerbro1_1");
	iDelay.push_back(175);
	vMinion.push_back(new Block(45, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 46 -----
	tSprite.push_back("hammerbro1_2");
	iDelay.push_back(155);
	tSprite.push_back("hammerbro1_3");
	iDelay.push_back(155);
	vMinion.push_back(new Block(46, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 47 -----
	tSprite.push_back("hammer_0");
	iDelay.push_back(95);
	tSprite.push_back("hammer_1");
	iDelay.push_back(95);
	tSprite.push_back("hammer_2");
	iDelay.push_back(95);
	tSprite.push_back("hammer_3");
	iDelay.push_back(95);
	vMinion.push_back(new Block(47, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 48 -----
	tSprite.push_back("hammer1_0");
	iDelay.push_back(95);
	tSprite.push_back("hammer1_1");
	iDelay.push_back(95);
	tSprite.push_back("hammer1_2");
	iDelay.push_back(95);
	tSprite.push_back("hammer1_3");
	iDelay.push_back(95);
	vMinion.push_back(new Block(48, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 49 -----
	tSprite.push_back("lakito_0");
	iDelay.push_back(0);
	vMinion.push_back(new Block(49, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 50 -----
	tSprite.push_back("lakito_1");
	iDelay.push_back(0);
	vMinion.push_back(new Block(50, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 51 -----
	tSprite.push_back("spikey0_0");
	iDelay.push_back(135);
	tSprite.push_back("spikey0_1");
	iDelay.push_back(135);
	vMinion.push_back(new Block(51, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 52 -----
	tSprite.push_back("spikey1_0");
	iDelay.push_back(75);
	tSprite.push_back("spikey1_1");
	iDelay.push_back(75);
	vMinion.push_back(new Block(52, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 53 -----
	tSprite.push_back("beetle_0");
	iDelay.push_back(155);
	tSprite.push_back("beetle_1");
	iDelay.push_back(155);
	vMinion.push_back(new Block(53, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 54 -----
	tSprite.push_back("beetle_2");
	iDelay.push_back(0);
	vMinion.push_back(new Block(54, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 55 -----
	tSprite.push_back("beetle1_0");
	iDelay.push_back(155);
	tSprite.push_back("beetle1_1");
	iDelay.push_back(155);
	vMinion.push_back(new Block(55, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 56 -----
	tSprite.push_back("beetle1_2");
	iDelay.push_back(0);
	vMinion.push_back(new Block(56, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 57 -----
	tSprite.push_back("beetle2_0");
	iDelay.push_back(155);
	tSprite.push_back("beetle2_1");
	iDelay.push_back(155);
	vMinion.push_back(new Block(57, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 58 -----
	tSprite.push_back("beetle2_2");
	iDelay.push_back(0);
	vMinion.push_back(new Block(58, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 59 -----
	tSprite.push_back("bulletbill");
	iDelay.push_back(0);
	vMinion.push_back(new Block(59, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 60 -----
	tSprite.push_back("bulletbill1");
	iDelay.push_back(0);
	vMinion.push_back(new Block(60, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 61 -----
	tSprite.push_back("hammer1_0");
	iDelay.push_back(0);
	vMinion.push_back(new Block(61, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 62 -----
	tSprite.push_back("fireball_0");
	iDelay.push_back(155);
	tSprite.push_back("fireball_1");
	iDelay.push_back(155);
	tSprite.push_back("fireball_2");
	iDelay.push_back(155);
	tSprite.push_back("fireball_3");
	iDelay.push_back(155);
	vMinion.push_back(new Block(62, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 63 -----
	tSprite.push_back("firework0");
	iDelay.push_back(0);
	vMinion.push_back(new Block(63, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 64 -----
	tSprite.push_back("firework1");
	iDelay.push_back(0);
	vMinion.push_back(new Block(64, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();
	// ----- 65 -----
	tSprite.push_back("firework1");
	iDelay.push_back(0);
	vMinion.push_back(new Block(65, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
	tSprite.clear();
	iDelay.clear();

	iBlockSize = vBlock.size();
	iMinionSize = vMinion.size();
}

/* ******************************************** */

void Map::clearMap() {
	for(int i = 0; i < iMapWidth; i++) {
		for(int j = 0; j < iMapHeight; j++) {
			delete lMap[i][j];
		}
		lMap[i].clear();
	}
	lMap.clear();

	this->iMapWidth = this->iMapHeight = 0;

	if(oFlag != NULL) {
		delete oFlag;
		oFlag = NULL;
	}

	oEvent->eventTypeID = oEvent->eNormal;

	clearLevelText();
}

void Map::clearMinions() {
	for(int i = 0; i < iMinionListSize; i++) {
		for(int j = 0, jSize = lMinion[i].size(); j < jSize; j++) {
			delete lMinion[i][j];
			jSize = lMinion[i].size();
		}
		lMinion[i].clear();
	}

	clearPlatforms();
}

void Map::clearPlatforms() {
	for(unsigned int i = 0; i < vPlatform.size(); i++) {
		delete vPlatform[i];
	}

	vPlatform.clear();
}

void Map::clearBubbles() {
	for(unsigned int i = 0; i < lBubble.size(); i++) {
		delete lBubble[i];
	}

	lBubble.clear();
}

/* ******************************************** */

void Map::setBackgroundColor(SDL_Renderer* rR) {
	switch(iLevelType) {
		case 0: case 2:
			SDL_SetRenderDrawColor(rR, 93, 148, 252, 255);
			break;
		case 1: case 3: case 4:
			SDL_SetRenderDrawColor(rR, 0, 0, 0, 255);
			break;
		default:
			SDL_SetRenderDrawColor(rR, 93, 148, 252, 255);
			break;
	}
}

std::string Map::getLevelName() {
	return "" + std::to_string(1 + currentLevelID/4) + "-" + std::to_string(currentLevelID%4 + 1);
}

void Map::loadMinionsLVL_1_1() {
	clearMinions();

	addGoombas(704, 368, true);

	addGoombas(1280, 368, true);

	addGoombas(1632, 368, true);
	addGoombas(1680, 368, true);

	addGoombas(2560, 112, true);
	addGoombas(2624, 112, true);

	addGoombas(3104, 368, true);
	addGoombas(3152, 368, true);

	addKoppa(107*32, 368, 1, true);

	addGoombas(3648, 368, true);
	addGoombas(3696, 368, true);

	addGoombas(3968, 368, true);
	addGoombas(4016, 368, true);

	addGoombas(4096, 368, true);
	addGoombas(4144, 368, true);

	addGoombas(5568, 368, true);
	addGoombas(5612, 368, true);
}

void Map::loadMinionsLVL_1_2() {
	clearMinions();

	this->iLevelType = 1;

	addGoombas(16*32, 368, true);
	addGoombas(17*32 + 8, 368 - 32, true);

	addGoombas(29*32, 368, true);

	addKoppa(44*32, 368, 1, true);
	addKoppa(45*32 + 16, 368, 1, true);

	addKoppa(59*32, 368, 1, true);

	addGoombas(62*32, 368, true);
	addGoombas(64*32, 368, true);

	addGoombas(73*32, 368 - 8*32, true);

	addGoombas(76*32, 368 - 4*32, true);
	addGoombas(77*32 + 16, 368 - 4*32, true);

	addGoombas(99*32, 368, true);
	addGoombas(100*32 + 16, 368, true);
	addGoombas(102*32, 368, true);

	addGoombas(113*32, 368, true);

	addGoombas(135*32, 368 - 3*32, true);
	addGoombas(136*32 + 16, 368 - 4*32, true);

	this->iLevelType = 3;

	addKoppa(146*32, 368, 1, false);

	this->iLevelType = 1;
	addPlant(103*32 + 16, CCFG::GAME_HEIGHT - 10 - 4*32);
	addPlant(109*32 + 16, CCFG::GAME_HEIGHT - 10 - 5*32);
	addPlant(115*32 + 16, CCFG::GAME_HEIGHT - 10 - 3*32);

	this->iLevelType = 0;
	addPlant(284*32 + 16, CCFG::GAME_HEIGHT - 10 - 3*32);

	this->iLevelType = 1;
}

void Map::loadMinionsLVL_1_3() {
	clearMinions();

	this->iLevelType = 3;

	addKoppa(30*32 - 8, CCFG::GAME_HEIGHT - 16 - 10*32, 1, true);
	addKoppa(110*32 - 8, CCFG::GAME_HEIGHT - 16 - 8*32, 1, true);

	addKoppa(74*32 - 8, CCFG::GAME_HEIGHT - 16 - 10*32, 3, false);
	addKoppa(114*32 - 8, CCFG::GAME_HEIGHT - 16 - 9*32, 3, false);

	addKoppa(133*32 - 8, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);
	this->iLevelType = 0;

	addGoombas(44*32, CCFG::GAME_HEIGHT - 16 - 11*32, true);
	addGoombas(46*32, CCFG::GAME_HEIGHT - 16 - 11*32, true);
	addGoombas(80*32, CCFG::GAME_HEIGHT - 16 - 9*32, true);
}

void Map::loadMinionsLVL_1_4() {
	clearMinions();

	addBowser(135*32, CCFG::GAME_HEIGHT - 16 - 6*32);
	
	addToad(153*32, CCFG::GAME_HEIGHT - 3*32, false);
	
	addFireBall(30*32, CCFG::GAME_HEIGHT - 16 - 4*32, 6, rand()%360, true);
	addFireBall(49*32, CCFG::GAME_HEIGHT - 16 - 8*32, 6, rand()%360, true);
	addFireBall(60*32, CCFG::GAME_HEIGHT - 16 - 8*32, 6, rand()%360, true);
	addFireBall(67*32, CCFG::GAME_HEIGHT - 16 - 8*32, 6, rand()%360, true);
	addFireBall(76*32, CCFG::GAME_HEIGHT - 16 - 5*32, 6, rand()%360, true);
	addFireBall(84*32, CCFG::GAME_HEIGHT - 16 - 5*32, 6, rand()%360, true);
	addFireBall(88*32, CCFG::GAME_HEIGHT - 16 - 10*32, 6, rand()%360, false);
}

void Map::loadMinionsLVL_2_1() {
	clearMinions();

	addSpring(188*32, 336);

	addGoombas(24*32, CCFG::GAME_HEIGHT - 16 - 7*32, true);

	addGoombas(42*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(43*32 + 16, CCFG::GAME_HEIGHT - 16 - 2*32, true);

	addGoombas(59*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(60*32 + 16, CCFG::GAME_HEIGHT - 16 - 2*32, true);

	addGoombas(68*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(69*32 + 16, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(71*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);

	addGoombas(87*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(88*32 + 16, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(90*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);

	addGoombas(102*32 + 16, CCFG::GAME_HEIGHT - 16 - 6*32, true);
	addGoombas(114*32 + 16, CCFG::GAME_HEIGHT - 16 - 4*32, true);

	addGoombas(120*32 + 16, CCFG::GAME_HEIGHT - 16 - 2*32, true);

	addGoombas(162*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(163*32 + 16, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	
	addKoppa(32*32 - 2, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);
	addKoppa(33*32, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);

	addKoppa(55*32, CCFG::GAME_HEIGHT - 16 - 6*32, 1, true);
	addKoppa(66*32, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);

	addKoppa(137*32, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);
	addKoppa(151*32, CCFG::GAME_HEIGHT - 16 - 4*32, 0, true);
	addKoppa(169*32, CCFG::GAME_HEIGHT - 16 - 2*32, 0, true);
	addKoppa(171*32, CCFG::GAME_HEIGHT - 16 - 2*32, 0, true);

	addKoppa(185*32, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);
	
	addPlant(46*32 + 16, CCFG::GAME_HEIGHT - 10 - 5*32);
	addPlant(74*32 + 16, CCFG::GAME_HEIGHT - 10 - 5*32);
	addPlant(103*32 + 16, CCFG::GAME_HEIGHT - 10 - 5*32);
	addPlant(115*32 + 16, CCFG::GAME_HEIGHT - 10 - 3*32);
	addPlant(122*32 + 16, CCFG::GAME_HEIGHT - 10 - 5*32);
	addPlant(130*32 + 16, CCFG::GAME_HEIGHT - 10 - 6*32);
	addPlant(176*32 + 16, CCFG::GAME_HEIGHT - 10 - 4*32);
}

void Map::loadMinionsLVL_2_2() {
	clearMinions();

	addSquid(22*32, CCFG::GAME_HEIGHT - 16 - 3*32);
	addSquid(46*32, CCFG::GAME_HEIGHT - 16 - 5*32);
	addSquid(55*32, CCFG::GAME_HEIGHT - 16 - 4*32);
	addSquid(83*32, CCFG::GAME_HEIGHT - 16 - 6*32);
	addSquid(94*32, CCFG::GAME_HEIGHT - 16 - 11*32);
	addSquid(105*32, CCFG::GAME_HEIGHT - 16 - 3*32);

	addCheep(75*32 + 28, CCFG::GAME_HEIGHT - 16 - 4*32, 0, 1);
	addCheep(78*32 + 28, CCFG::GAME_HEIGHT - 16 - 7*32, 0, 1);
	addCheep(81*32 + 28, CCFG::GAME_HEIGHT - 16 - 2*32 - 28, 0, 1);
	addCheep(94*32 + 14, CCFG::GAME_HEIGHT - 16 - 8*32, 0, 1);
	addCheep(101*32 + 28, CCFG::GAME_HEIGHT - 16 - 4*32, 0, 1);
	addCheep(97*32 + 8, CCFG::GAME_HEIGHT - 16 - 11*32, 1, 1);
	addCheep(117*32 + 8, CCFG::GAME_HEIGHT - 16 - 10*32, 0, 1);
	addCheep(127*32 + 24, CCFG::GAME_HEIGHT - 16 - 4*32, 1, 1);
	addCheep(131*32 + 8, CCFG::GAME_HEIGHT - 16 - 3*32 - 4, 0, 1);
	addCheep(136*32 + 16, CCFG::GAME_HEIGHT - 16 - 6*32, 0, 1);
	addCheep(145*32 + 8, CCFG::GAME_HEIGHT - 16 - 4*32, 0, 1);
	addCheep(149*32 + 28, CCFG::GAME_HEIGHT - 16 - 8*32 - 4, 1, 1);
	addCheep(164*32, CCFG::GAME_HEIGHT - 16 - 11*32, 0, 1);
	addCheep(167*32, CCFG::GAME_HEIGHT - 16 - 3*32, 1, 1);
	addCheep(175*32, CCFG::GAME_HEIGHT - 16 - 6*32 - 4, 0, 1);
	addCheep(183*32, CCFG::GAME_HEIGHT - 16 - 10*32, 1, 1);
	addCheep(186*32 + 16, CCFG::GAME_HEIGHT - 16 - 7*32, 1, 1);

	this->iLevelType = 0;
	addPlant(274*32 + 16, CCFG::GAME_HEIGHT - 10 - 3*32);

	this->iLevelType = 2;
}

void Map::loadMinionsLVL_2_3() {
	clearMinions();

	addCheepSpawner(5*32, 200*32);
}

void Map::loadMinionsLVL_2_4() {
	clearMinions();

	addBowser(135*32, CCFG::GAME_HEIGHT - 16 - 6*32);
	
	addToad(153*32, CCFG::GAME_HEIGHT - 3*32, false);

	addFireBall(49*32, CCFG::GAME_HEIGHT - 16 - 5*32, 6, rand()%360, true);
	addFireBall(55*32, CCFG::GAME_HEIGHT - 16 - 9*32, 6, rand()%360, true);
	addFireBall(61*32, CCFG::GAME_HEIGHT - 16 - 5*32, 6, rand()%360, true);
	addFireBall(73*32, CCFG::GAME_HEIGHT - 16 - 5*32, 6, rand()%360, true);
	addFireBall(82*32, CCFG::GAME_HEIGHT - 16 - 8*32, 6, rand()%360, true);
	addFireBall(92*32, CCFG::GAME_HEIGHT - 16 - 4*32, 6, rand()%360, false);

	addUpFire(16*32 + 4, 9*32);
	addUpFire(30*32, 9*32);
}

void Map::loadMinionsLVL_3_1() {
	clearMinions();

	addSpring(126*32, 336);

	addGoombas(37*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(53*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(54*32 + 16, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(56*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(82*32, CCFG::GAME_HEIGHT - 16 - 6*32, true);
	addGoombas(83*32 + 16, CCFG::GAME_HEIGHT - 16 - 6*32, true);
	addGoombas(85*32, CCFG::GAME_HEIGHT - 16 - 6*32, true);
	addGoombas(94*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(95*32 + 16, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(139*32 - 4, CCFG::GAME_HEIGHT - 16 - 6*32, true);
	addGoombas(140*32, CCFG::GAME_HEIGHT - 16 - 7*32, true);
	addGoombas(154*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(155*32 + 16, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(157*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);

	addPlant(32*32 + 16, CCFG::GAME_HEIGHT - 10 - 4*32);
	addPlant(38*32 + 16, CCFG::GAME_HEIGHT - 10 - 5*32);
	addPlant(57*32 + 16, CCFG::GAME_HEIGHT - 10 - 4*32);
	addPlant(67*32 + 16, CCFG::GAME_HEIGHT - 10 - 3*32);
	addPlant(103*32 + 16, CCFG::GAME_HEIGHT - 10 - 5*32);

	addKoppa(25*32, CCFG::GAME_HEIGHT - 16 - 2*32, 0, true);
	addKoppa(28*32, CCFG::GAME_HEIGHT - 16 - 3*32, 0, true);
	addKoppa(65*32, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);
	addKoppa(101*32, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);
	addKoppa(149*32, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);
	addKoppa(152*32, CCFG::GAME_HEIGHT - 16 - 10*32, 1, true);
	addKoppa(165*32, CCFG::GAME_HEIGHT - 16 - 2*32, 0, true);
	addKoppa(168*32, CCFG::GAME_HEIGHT - 16 - 3*32, 0, true);
	addKoppa(170*32, CCFG::GAME_HEIGHT - 16 - 6*32, 1, true);
	addKoppa(171*32, CCFG::GAME_HEIGHT - 16 - 2*32, 0, true);
	addKoppa(188*32, CCFG::GAME_HEIGHT - 16 - 8*32, 1, true);
	addKoppa(191*32, CCFG::GAME_HEIGHT - 16 - 10*32, 1, true);

	addHammerBro(113*32, CCFG::GAME_HEIGHT - 16 - 7*32);
	addHammerBro(116*32, CCFG::GAME_HEIGHT - 16 - 3*32);
}

void Map::loadMinionsLVL_3_2() {
	clearMinions();

	addKoppa(17*32, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);
	addKoppa(33*32, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);
	addKoppa(34*32 + 16, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);
	addKoppa(36*32, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);
	addKoppa(43*32, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);
	addKoppa(44*32 + 16, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);
	addKoppa(66*32, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);
	addKoppa(78*32, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);
	addKoppa(98*32, CCFG::GAME_HEIGHT - 16 - 2*32, 0, true);
	addKoppa(111*32, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);
	addKoppa(134*32, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);
	addKoppa(140*32, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);
	addKoppa(141*32 + 16, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);
	addKoppa(143*32, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);
	addKoppa(150*32, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);
	addKoppa(151*32 + 16, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);
	addKoppa(162*32, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);
	addKoppa(163*32 + 16, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);
	addKoppa(165*32, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);
	addKoppa(175*32, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);
	
	addGoombas(24*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(25*32 + 16, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(27*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(71*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(72*32 + 16, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(74*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(119*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(120*32 + 16, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(122*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(179*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(180*32 + 16, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(182*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(188*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(189*32 + 16, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(191*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);

	addPlant(169*32 + 16, CCFG::GAME_HEIGHT - 10 - 4*32);
}

void Map::loadMinionsLVL_3_3() {
	clearMinions();

	addGoombas(26*32, CCFG::GAME_HEIGHT - 16 - 8*32, true);
	
	iLevelType = 3;

	addKoppa(52*32, CCFG::GAME_HEIGHT - 16 - 8*32, 1, true);
	addKoppa(54*32, CCFG::GAME_HEIGHT - 16 - 4*32, 1, true);
	addKoppa(73*32, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);

	addKoppa(114*32 - 8, CCFG::GAME_HEIGHT - 16 - 9*32, 3, false);

	addKoppa(124*32, CCFG::GAME_HEIGHT - 16 - 5*32, 1, true);
	addKoppa(126*32, CCFG::GAME_HEIGHT - 16 - 5*32, 1, true);

	iLevelType = 4;
}

void Map::loadMinionsLVL_3_4() {
	clearMinions();
	
	addBowser(135*32, CCFG::GAME_HEIGHT - 16 - 6*32);
	
	addToad(153*32, CCFG::GAME_HEIGHT - 3*32, false);

	addFireBall(19*32, CCFG::GAME_HEIGHT - 16 - 3*32, 6, rand()%360, true);
	addFireBall(24*32, CCFG::GAME_HEIGHT - 16 - 3*32, 6, rand()%360, true);
	addFireBall(29*32, CCFG::GAME_HEIGHT - 16 - 3*32, 6, rand()%360, true);
	addFireBall(54*32, CCFG::GAME_HEIGHT - 16 - 3*32, 6, rand()%360, true);
	addFireBall(54*32, CCFG::GAME_HEIGHT - 16 - 9*32, 6, rand()%360, false);
	addFireBall(64*32, CCFG::GAME_HEIGHT - 16 - 3*32, 6, rand()%360, true);
	addFireBall(64*32, CCFG::GAME_HEIGHT - 16 - 9*32, 6, rand()%360, false);
	addFireBall(80*32, CCFG::GAME_HEIGHT - 16 - 3*32, 6, rand()%360, true);
	addFireBall(80*32, CCFG::GAME_HEIGHT - 16 - 9*32, 6, rand()%360, false);

	addUpFire(16*32, 9*32);
	addUpFire(26*32, 9*32);
	addUpFire(88*32, 9*32);
	addUpFire(97*32, 9*32);
	addUpFire(103*32, 9*32);
	addUpFire(109*32, 9*32);
}

void Map::loadMinionsLVL_4_1() {
	clearMinions();

	addPlant(21*32 + 16, CCFG::GAME_HEIGHT - 10 - 4*32);
	addPlant(116*32 + 16, CCFG::GAME_HEIGHT - 10 - 5*32);
	addPlant(132*32 + 16, CCFG::GAME_HEIGHT - 10 - 5*32);
	addPlant(163*32 + 16, CCFG::GAME_HEIGHT - 10 - 3*32);

	addLakito(26*32, CCFG::GAME_HEIGHT - 16 - 11*32, 207*32);
}

void Map::loadMinionsLVL_4_2() {
	clearMinions();

	addGoombas(43*32, CCFG::GAME_HEIGHT - 16 - 6*32, true);
	addGoombas(44*32 + 16, CCFG::GAME_HEIGHT - 16 - 6*32, true);
	addGoombas(46*32, CCFG::GAME_HEIGHT - 16 - 6*32, true);

	addKoppa(77*32, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);
	addKoppa(100*32, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);
	addKoppa(101*32 + 16, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);
	addKoppa(137*32, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);
	addKoppa(168*32, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);
	addKoppa(169*32 + 16, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);

	addPlant(72*32 + 16, CCFG::GAME_HEIGHT - 10 - 4*32);
	addPlant(78*32 + 16, CCFG::GAME_HEIGHT - 10 - 8*32);
	addPlant(84*32 + 16, CCFG::GAME_HEIGHT - 10 - 4*32);
	addPlant(107*32 + 16, CCFG::GAME_HEIGHT - 10 - 5*32);
	addPlant(138*32 + 16, CCFG::GAME_HEIGHT - 10 - 4*32);
	addPlant(142*32 + 16, CCFG::GAME_HEIGHT - 10 - 5*32);
	addPlant(180*32 + 16, CCFG::GAME_HEIGHT - 10 - 8*32);

	addBeetle(83*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addBeetle(88*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addBeetle(154*32, CCFG::GAME_HEIGHT - 16 - 5*32, true);
	addBeetle(179*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);

	this->iLevelType = 0;

	addPlant(394*32 + 16, CCFG::GAME_HEIGHT - 10 - 3*32);

	this->iLevelType = 1;
}

void Map::loadMinionsLVL_4_3() {
	clearMinions();

	this->iLevelType = 3;

	addKoppa(28*32 - 2, CCFG::GAME_HEIGHT - 16 - 6*32, 1, true);
	addKoppa(29*32, CCFG::GAME_HEIGHT - 16 - 6*32, 1, true);
	addKoppa(39*32, CCFG::GAME_HEIGHT - 16 - 3*32, 1, true);
	addKoppa(68*32, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);
	addKoppa(78*32, CCFG::GAME_HEIGHT - 16 - 10*32, 1, true);

	addKoppa(35*32, CCFG::GAME_HEIGHT - 16 - 11*32, 3, false);
	
	this->iLevelType = 0;
}


void Map::loadMinionsLVL_4_4() {
	clearMinions();

	addBowser(167*32, CCFG::GAME_HEIGHT - 16 - 6*32);
	
	addToad(186*32, CCFG::GAME_HEIGHT - 3*32, false);

	addFireBall(53*32, CCFG::GAME_HEIGHT - 16 - 8*32, 6, rand()%360, true);
	addFireBall(60*32, CCFG::GAME_HEIGHT - 16 - 5*32, 6, rand()%360, false);
	addFireBall(115*32, CCFG::GAME_HEIGHT - 16 - 8*32, 6, rand()%360, true);
	addFireBall(122*32, CCFG::GAME_HEIGHT - 16 - 4*32, 6, rand()%360, true);
	addFireBall(162*32, CCFG::GAME_HEIGHT - 16 - 4*32, 6, rand()%360, true);

	addUpFire(165*32, 9*32);

	this->iLevelType = 1;

	addPlant(40*32 + 16, CCFG::GAME_HEIGHT - 10 - 4*32);

	this->iLevelType = 3;
}

void Map::loadMinionsLVL_5_1() {
	clearMinions();

	addGoombas(19*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(20*32 + 16, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(22*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(30*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(31*32 + 16, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(33*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(65*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(66*32 + 16, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(68*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(76*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(77*32 + 16, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(103*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(104*32 + 16, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(106*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(121*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(122*32 + 16, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(124*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(135*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(136*32 + 16, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(138*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	
	addKoppa(16*32, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);
	addKoppa(41*32, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);
	addKoppa(42*32 + 16, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);
	addKoppa(61*32, CCFG::GAME_HEIGHT - 16 - 3*32, 0, true);
	addKoppa(87*32, CCFG::GAME_HEIGHT - 16 - 3*32, 0, true);
	addKoppa(127*32, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);
	addKoppa(144*32, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);
	addKoppa(145*32 + 16, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);
	addKoppa(178*32, CCFG::GAME_HEIGHT - 16 - 2*32, 0, true);
	addKoppa(182*32, CCFG::GAME_HEIGHT - 16 - 6*32, 1, true);

	addPlant(44*32 + 16, CCFG::GAME_HEIGHT - 10 - 4*32);
	addPlant(51*32 + 16, CCFG::GAME_HEIGHT - 10 - 4*32);
	addPlant(156*32 + 16, CCFG::GAME_HEIGHT - 10 - 7*32);
	addPlant(163*32 + 16, CCFG::GAME_HEIGHT - 10 - 3*32);
	
}

void Map::loadMinionsLVL_5_2() {
	clearMinions();

	addGoombas(143*32, CCFG::GAME_HEIGHT - 16 - 4*32, true);
	addGoombas(145*32, CCFG::GAME_HEIGHT - 16 - 6*32, true);
	addGoombas(235*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(236*32 + 16, CCFG::GAME_HEIGHT - 16 - 2*32, true);

	addKoppa(103*32, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);
	addKoppa(120*32, CCFG::GAME_HEIGHT - 16 - 3*32, 0, true);
	addKoppa(186*32, CCFG::GAME_HEIGHT - 16 - 4*32, 0, true);
	addKoppa(243*32, CCFG::GAME_HEIGHT - 16 - 3*32, 0, true);
	addKoppa(246*32, CCFG::GAME_HEIGHT - 16 - 2*32, 0, true);
	addKoppa(266*32, CCFG::GAME_HEIGHT - 16 - 7*32, 0, true);

	addHammerBro(126*32, CCFG::GAME_HEIGHT - 16 - 6*32);
	addHammerBro(161*32, CCFG::GAME_HEIGHT - 16 - 7*32);
	addHammerBro(200*32, CCFG::GAME_HEIGHT - 16 - 7*32);
	addHammerBro(204*32, CCFG::GAME_HEIGHT - 16 - 11*32);

	addPlant(135*32 + 16, CCFG::GAME_HEIGHT - 10 - 4*32);
	addPlant(195*32 + 16, CCFG::GAME_HEIGHT - 10 - 3*32);

	addBeetle(216*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addBeetle(217*32 + 4, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addBeetle(218*32 + 8, CCFG::GAME_HEIGHT - 16 - 2*32, true);

	this->iLevelType = 3;

	addKoppa(237*32, CCFG::GAME_HEIGHT - 16 - 10*32, 1, true);

	this->iLevelType = 0;

	// -- MAP 5-2-2

	addSquid(17*32, CCFG::GAME_HEIGHT - 16 - 4*32);
	addSquid(34*32, CCFG::GAME_HEIGHT - 16 - 4*32);
	addSquid(43*32 + 16, CCFG::GAME_HEIGHT - 16 - 4*32);

	addCheep(27*32 + 16, CCFG::GAME_HEIGHT - 9*32, 0, 1);
	addCheep(38*32 + 28, CCFG::GAME_HEIGHT - 4*32, 0, 1);
	addCheep(48*32 + 16, CCFG::GAME_HEIGHT - 6*32, 1, 1);
	addCheep(53*32 + 16, CCFG::GAME_HEIGHT - 11*32, 0, 1);
}

void Map::loadMinionsLVL_5_3() {
	clearMinions();

	this->iLevelType = 3;

	addKoppa(30*32 - 8, CCFG::GAME_HEIGHT - 16 - 10*32, 1, true);
	addKoppa(110*32 - 8, CCFG::GAME_HEIGHT - 16 - 8*32, 1, true);

	addKoppa(74*32 - 8, CCFG::GAME_HEIGHT - 16 - 10*32, 3, false);
	addKoppa(114*32 - 8, CCFG::GAME_HEIGHT - 16 - 9*32, 3, false);

	addKoppa(133*32 - 8, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);
	this->iLevelType = 0;

	addGoombas(44*32, CCFG::GAME_HEIGHT - 16 - 11*32, true);
	addGoombas(46*32, CCFG::GAME_HEIGHT - 16 - 11*32, true);
	addGoombas(80*32, CCFG::GAME_HEIGHT - 16 - 9*32, true);

	addBulletBillSpawner(174, 3, 1);
}

void Map::loadMinionsLVL_5_4() {
	clearMinions();

	addBowser(135*32, CCFG::GAME_HEIGHT - 16 - 6*32);
	
	addToad(153*32, CCFG::GAME_HEIGHT - 3*32, false);

	addFireBall(49*32, CCFG::GAME_HEIGHT - 16 - 5*32, 6, rand()%360, true);
	addFireBall(55*32, CCFG::GAME_HEIGHT - 16 - 9*32, 6, rand()%360, true);
	addFireBall(61*32, CCFG::GAME_HEIGHT - 16 - 5*32, 6, rand()%360, true);
	addFireBall(73*32, CCFG::GAME_HEIGHT - 16 - 5*32, 6, rand()%360, true);
	addFireBall(82*32, CCFG::GAME_HEIGHT - 16 - 8*32, 6, rand()%360, true);
	addFireBall(92*32, CCFG::GAME_HEIGHT - 16 - 4*32, 6, rand()%360, false);

	addFireBall(23*32, CCFG::GAME_HEIGHT - 16 - 7*32, 12, rand()%360, true);

	addFireBall(43*32, CCFG::GAME_HEIGHT - 16 - 1*32, 6, rand()%360, false);
	addFireBall(55*32, CCFG::GAME_HEIGHT - 16 - 1*32, 6, rand()%360, false);
	addFireBall(67*32, CCFG::GAME_HEIGHT - 16 - 1*32, 6, rand()%360, true);

	addFireBall(103*32, CCFG::GAME_HEIGHT - 16 - 3*32, 6, rand()%360, true);

	addUpFire(16*32 + 4, 9*32);
	addUpFire(30*32, 9*32);

	addUpFire(109*32, 9*32);
	addUpFire(113*32, 9*32);
	addUpFire(131*32, 9*32);
}

void Map::loadMinionsLVL_6_1() {
	clearMinions();

	this->iLevelType = 0;

	addLakito(23*32, CCFG::GAME_HEIGHT - 16 - 11*32, 176*32);

	addPlant(102*32 + 16, CCFG::GAME_HEIGHT - 10 - 4*32);

	this->iLevelType = 4;
}

void Map::loadMinionsLVL_6_2() {
	clearMinions();

	addSquid(17*32, CCFG::GAME_HEIGHT - 16 - 4*32);
	addSquid(34*32, CCFG::GAME_HEIGHT - 16 - 4*32);
	addSquid(43*32 + 16, CCFG::GAME_HEIGHT - 16 - 4*32);

	addCheep(27*32 + 16, CCFG::GAME_HEIGHT - 9*32, 0, 1);
	addCheep(38*32 + 28, CCFG::GAME_HEIGHT - 4*32, 0, 1);
	addCheep(48*32 + 16, CCFG::GAME_HEIGHT - 6*32, 1, 1);
	addCheep(53*32 + 16, CCFG::GAME_HEIGHT - 11*32, 0, 1);

	addKoppa(111*32, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);
	addKoppa(128*32, CCFG::GAME_HEIGHT - 16 - 3*32, 0, true);
	addKoppa(291*32, CCFG::GAME_HEIGHT - 16 - 11*32, 0, true);

	addPlant(104*32 + 16, CCFG::GAME_HEIGHT - 10 - 5*32);
	addPlant(113*32 + 16, CCFG::GAME_HEIGHT - 10 - 5*32);
	addPlant(117*32 + 16, CCFG::GAME_HEIGHT - 10 - 8*32);
	addPlant(120*32 + 16, CCFG::GAME_HEIGHT - 10 - 3*32);
	addPlant(122*32 + 16, CCFG::GAME_HEIGHT - 10 - 3*32);
	addPlant(131*32 + 16, CCFG::GAME_HEIGHT - 10 - 5*32);
	addPlant(141*32 + 16, CCFG::GAME_HEIGHT - 10 - 6*32);
	addPlant(147*32 + 16, CCFG::GAME_HEIGHT - 10 - 7*32);
	addPlant(152*32 + 16, CCFG::GAME_HEIGHT - 10 - 3*32);
	addPlant(165*32 + 16, CCFG::GAME_HEIGHT - 10 - 4*32);
	addPlant(169*32 + 16, CCFG::GAME_HEIGHT - 10 - 3*32);
	addPlant(172*32 + 16, CCFG::GAME_HEIGHT - 10 - 7*32);
	addPlant(179*32 + 16, CCFG::GAME_HEIGHT - 10 - 4*32);
	addPlant(190*32 + 16, CCFG::GAME_HEIGHT - 10 - 3*32);
	addPlant(196*32 + 16, CCFG::GAME_HEIGHT - 10 - 8*32);
	addPlant(200*32 + 16, CCFG::GAME_HEIGHT - 10 - 3*32);
	addPlant(216*32 + 16, CCFG::GAME_HEIGHT - 10 - 3*32);
	addPlant(220*32 + 16, CCFG::GAME_HEIGHT - 10 - 3*32);
	addPlant(238*32 + 16, CCFG::GAME_HEIGHT - 10 - 4*32);
	addPlant(252*32 + 16, CCFG::GAME_HEIGHT - 10 - 8*32);
	addPlant(259*32 + 16, CCFG::GAME_HEIGHT - 10 - 5*32);
	addPlant(264*32 + 16, CCFG::GAME_HEIGHT - 10 - 3*32);
	addPlant(266*32 + 16, CCFG::GAME_HEIGHT - 10 - 4*32);
	addPlant(268*32 + 16, CCFG::GAME_HEIGHT - 10 - 5*32);
	addPlant(274*32 + 16, CCFG::GAME_HEIGHT - 10 - 4*32);
	addPlant(286*32 + 16, CCFG::GAME_HEIGHT - 10 - 5*32);
	

	addBeetle(139*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addBeetle(177*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addBeetle(205*32, CCFG::GAME_HEIGHT - 16 - 10*32, true);
	addBeetle(248*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);

	addGoombas(152*32, CCFG::GAME_HEIGHT - 16 - 10*32, true);
	addGoombas(254*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);

}

void Map::loadMinionsLVL_6_3() {
	clearMinions();

	addBulletBillSpawner(174, 3, 1);
}

void Map::loadMinionsLVL_6_4() {
	clearMinions();

	addBowser(135*32, CCFG::GAME_HEIGHT - 16 - 6*32, true);
	
	addToad(153*32, CCFG::GAME_HEIGHT - 3*32, false);
	
	addFireBall(30*32, CCFG::GAME_HEIGHT - 16 - 4*32, 6, rand()%360, true);
	addFireBall(49*32, CCFG::GAME_HEIGHT - 16 - 8*32, 6, rand()%360, true);
	addFireBall(60*32, CCFG::GAME_HEIGHT - 16 - 8*32, 6, rand()%360, true);
	addFireBall(67*32, CCFG::GAME_HEIGHT - 16 - 8*32, 6, rand()%360, true);
	addFireBall(76*32, CCFG::GAME_HEIGHT - 16 - 5*32, 6, rand()%360, true);
	addFireBall(84*32, CCFG::GAME_HEIGHT - 16 - 5*32, 6, rand()%360, true);
	addFireBall(88*32, CCFG::GAME_HEIGHT - 16 - 10*32, 6, rand()%360, false);

	addFireBall(23*32, CCFG::GAME_HEIGHT - 16 - 8*32, 6, rand()%360, true);
	addFireBall(37*32, CCFG::GAME_HEIGHT - 16 - 8*32, 6, rand()%360, true);
	addFireBall(80*32, CCFG::GAME_HEIGHT - 16 - 10*32, 6, rand()%360, false);
	addFireBall(92*32, CCFG::GAME_HEIGHT - 16 - 5*32, 6, rand()%360, true);
	
	addUpFire(27*32, 9*32);
	addUpFire(33*32, 9*32);
	addUpFire(131*32, 9*32);
}


void Map::loadMinionsLVL_7_1() {
	clearMinions();


	addKoppa(26*32, CCFG::GAME_HEIGHT - 16 - 3*32, 0, true);
	addKoppa(44*32, CCFG::GAME_HEIGHT - 16 - 4*32, 0, true);
	addKoppa(53*32, CCFG::GAME_HEIGHT - 16 - 2*32, 0, true);
	addKoppa(65*32, CCFG::GAME_HEIGHT - 16 - 3*32, 0, true);

	addKoppa(114*32, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);

	addPlant(76*32 + 16, CCFG::GAME_HEIGHT - 10 - 4*32);
	addPlant(93*32 + 16, CCFG::GAME_HEIGHT - 10 - 4*32);
	addPlant(109*32 + 16, CCFG::GAME_HEIGHT - 10 - 4*32);
	addPlant(115*32 + 16, CCFG::GAME_HEIGHT - 10 - 3*32);
	addPlant(128*32 + 16, CCFG::GAME_HEIGHT - 10 - 3*32);

	addHammerBro(85*32, CCFG::GAME_HEIGHT - 16 - 7*32);
	addHammerBro(87*32, CCFG::GAME_HEIGHT - 16 - 11*32);
	addHammerBro(137*32, CCFG::GAME_HEIGHT - 16 - 7*32);
	addHammerBro(135*32, CCFG::GAME_HEIGHT - 16 - 3*32);

	addBeetle(169*32, CCFG::GAME_HEIGHT - 16 - 10*32, true);
}

void Map::loadMinionsLVL_7_2() {
	clearMinions();

	addSquid(22*32, CCFG::GAME_HEIGHT - 16 - 3*32);
	addSquid(24*32, CCFG::GAME_HEIGHT - 16 - 7*32);
	addSquid(46*32, CCFG::GAME_HEIGHT - 16 - 5*32);
	addSquid(52*32, CCFG::GAME_HEIGHT - 16 - 7*32);
	addSquid(55*32, CCFG::GAME_HEIGHT - 16 - 4*32);
	addSquid(77*32, CCFG::GAME_HEIGHT - 16 - 8*32);
	addSquid(83*32, CCFG::GAME_HEIGHT - 16 - 6*32);
	addSquid(90*32, CCFG::GAME_HEIGHT - 16 - 3*32);
	addSquid(94*32, CCFG::GAME_HEIGHT - 16 - 11*32);
	addSquid(105*32, CCFG::GAME_HEIGHT - 16 - 3*32);
	addSquid(150*32, CCFG::GAME_HEIGHT - 16 - 6*32);
	addSquid(173*32, CCFG::GAME_HEIGHT - 16 - 3*32);
	addSquid(179*32, CCFG::GAME_HEIGHT - 16 - 3*32);

	addCheep(75*32 + 28, CCFG::GAME_HEIGHT - 16 - 4*32, 0, 1);
	addCheep(78*32 + 28, CCFG::GAME_HEIGHT - 16 - 7*32, 0, 1);
	addCheep(81*32 + 28, CCFG::GAME_HEIGHT - 16 - 2*32 - 28, 0, 1);
	addCheep(94*32 + 14, CCFG::GAME_HEIGHT - 16 - 8*32, 0, 1);
	addCheep(101*32 + 28, CCFG::GAME_HEIGHT - 16 - 4*32, 0, 1);
	addCheep(97*32 + 8, CCFG::GAME_HEIGHT - 16 - 11*32, 1, 1);
	addCheep(117*32 + 8, CCFG::GAME_HEIGHT - 16 - 10*32, 0, 1);
	addCheep(127*32 + 24, CCFG::GAME_HEIGHT - 16 - 4*32, 1, 1);
	addCheep(131*32 + 8, CCFG::GAME_HEIGHT - 16 - 3*32 - 4, 0, 1);
	addCheep(136*32 + 16, CCFG::GAME_HEIGHT - 16 - 6*32, 0, 1);
	addCheep(145*32 + 8, CCFG::GAME_HEIGHT - 16 - 4*32, 0, 1);
	addCheep(149*32 + 28, CCFG::GAME_HEIGHT - 16 - 8*32 - 4, 1, 1);
	addCheep(164*32, CCFG::GAME_HEIGHT - 16 - 11*32, 0, 1);
	addCheep(167*32, CCFG::GAME_HEIGHT - 16 - 3*32, 1, 1);
	addCheep(175*32, CCFG::GAME_HEIGHT - 16 - 6*32 - 4, 0, 1);
	addCheep(183*32, CCFG::GAME_HEIGHT - 16 - 10*32, 1, 1);
	addCheep(186*32 + 16, CCFG::GAME_HEIGHT - 16 - 7*32, 1, 1);

	this->iLevelType = 0;
	addPlant(274*32 + 16, CCFG::GAME_HEIGHT - 10 - 3*32);

	this->iLevelType = 2;
}

void Map::loadMinionsLVL_7_3() {
	clearMinions();

	addCheepSpawner(5*32, 200*32);

	addKoppa(52*32, CCFG::GAME_HEIGHT - 16 - 6*32, 0, true);
	addKoppa(140*32, CCFG::GAME_HEIGHT - 16 - 7*32, 0, true);
	addKoppa(156*32, CCFG::GAME_HEIGHT - 16 - 5*32, 0, true);

	this->iLevelType = 3;

	addKoppa(79*32, CCFG::GAME_HEIGHT - 16 - 5*32, 1, true);
	addKoppa(95*32, CCFG::GAME_HEIGHT - 16 - 5*32, 1, true);
	addKoppa(119*32, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);

	this->iLevelType = 0;
}

void Map::loadMinionsLVL_7_4() {
	clearMinions();

	addUpFire(20*32, 9*32);
	addUpFire(260*32, 9*32);

	addFireBall(167*32, CCFG::GAME_HEIGHT - 16 - 7*32, 6, rand()%360, true);

	addBowser(263*32, CCFG::GAME_HEIGHT - 16 - 6*32, true);

	addToad(281*32, CCFG::GAME_HEIGHT - 3*32, false);
}

void Map::loadMinionsLVL_8_1() {
	clearMinions();

	addGoombas(23*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(24*32 + 16, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(26*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(30*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(31*32 + 16, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(33*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(69*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(70*32 + 16, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(72*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(108*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(109*32 + 16, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(111*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(148*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(149*32 + 16, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(151*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(232*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(233*32 + 16, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(235*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(257*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(258*32 + 16, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(260*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(264*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(265*32 + 16, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(267*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(272*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(273*32 + 16, CCFG::GAME_HEIGHT - 16 - 2*32, true);

	addKoppa(43*32, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);
	addKoppa(44*32 + 16, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);
	addKoppa(61*32, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);
	addKoppa(119*32, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);
	addKoppa(124*32, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);
	addKoppa(125*32 + 16, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);
	addKoppa(127*32, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);
	addKoppa(130*32, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);
	addKoppa(131*32 + 16, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);
	addKoppa(133*32, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);
	addKoppa(161*32, CCFG::GAME_HEIGHT - 16 - 3*32, 0, true);
	addKoppa(172*32, CCFG::GAME_HEIGHT - 16 - 4*32, 0, true);
	addKoppa(177*32, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);
	addKoppa(207*32, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);
	addKoppa(208*32 + 16, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);
	addKoppa(305*32, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);
	addKoppa(332*32, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);
	addKoppa(339*32, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);
	addKoppa(340*32 + 16, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);
	addKoppa(342*32, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);


	addBeetle(18*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addBeetle(81*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addBeetle(254*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addBeetle(283*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);

	addPlant(35*32 + 16, CCFG::GAME_HEIGHT - 10 - 5*32);
	addPlant(76*32 + 16, CCFG::GAME_HEIGHT - 10 - 5*32);
	addPlant(82*32 + 16, CCFG::GAME_HEIGHT - 10 - 4*32);
	addPlant(94*32 + 16, CCFG::GAME_HEIGHT - 10 - 5*32);
	addPlant(104*32 + 16, CCFG::GAME_HEIGHT - 10 - 5*32);
	addPlant(140*32 + 16, CCFG::GAME_HEIGHT - 10 - 4*32);
	addPlant(238*32 + 16, CCFG::GAME_HEIGHT - 10 - 4*32);
	addPlant(242*32 + 16, CCFG::GAME_HEIGHT - 10 - 5*32);
	addPlant(344*32 + 16, CCFG::GAME_HEIGHT - 10 - 4*32);
	addPlant(355*32 + 16, CCFG::GAME_HEIGHT - 10 - 3*32);
}

void Map::loadMinionsLVL_8_2() {
	clearMinions();

	addGoombas(184*32, CCFG::GAME_HEIGHT - 16 - 5*32, true);
	addGoombas(186*32, CCFG::GAME_HEIGHT - 16 - 7*32, true);

	addKoppa(18*32 - 8, CCFG::GAME_HEIGHT - 16 - 5*32, 0, true);
	addKoppa(24*32, CCFG::GAME_HEIGHT - 16 - 10*32, 0, true);
	addKoppa(57*32, CCFG::GAME_HEIGHT - 16 - 3*32, 0, true);
	addKoppa(66*32, CCFG::GAME_HEIGHT - 16 - 3*32, 0, true);
	addKoppa(69*32, CCFG::GAME_HEIGHT - 16 - 3*32, 0, true);
	addKoppa(92*32, CCFG::GAME_HEIGHT - 16 - 4*32, 0, true);
	addKoppa(95*32, CCFG::GAME_HEIGHT - 16 - 3*32, 0, true);
	addKoppa(139*32, CCFG::GAME_HEIGHT - 16 - 2*32, 0, true);
	addKoppa(170*32, CCFG::GAME_HEIGHT - 16 - 4*32, 0, true);
	addKoppa(172*32, CCFG::GAME_HEIGHT - 16 - 3*32, 0, true);
	addKoppa(175*32, CCFG::GAME_HEIGHT - 16 - 6*32, 0, true);
	addKoppa(203*32, CCFG::GAME_HEIGHT - 16 - 8*32, 0, true);

	addBeetle(111*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addBeetle(121*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addBeetle(123*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addBeetle(189*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);

	addLakito(16*32, CCFG::GAME_HEIGHT - 16 - 11*32, 216*32);

	addPlant(131*32 + 16, CCFG::GAME_HEIGHT - 10 - 3*32);
	addPlant(142*32 + 16, CCFG::GAME_HEIGHT - 10 - 3*32);
	addPlant(156*32 + 16, CCFG::GAME_HEIGHT - 10 - 5*32);
	addPlant(163*32 + 16, CCFG::GAME_HEIGHT - 10 - 3*32);
	addPlant(131*32 + 16, CCFG::GAME_HEIGHT - 10 - 3*32);
}

void Map::loadMinionsLVL_8_3() {
	clearMinions();

	addKoppa(30*32, CCFG::GAME_HEIGHT - 16 - 4*32, 0, true);
	addKoppa(93*32, CCFG::GAME_HEIGHT - 16 - 3*32, 0, true);
	addKoppa(137*32, CCFG::GAME_HEIGHT - 16 - 2*32, 1, true);

	addPlant(53*32 + 16, CCFG::GAME_HEIGHT - 10 - 5*32);
	addPlant(126*32 + 16, CCFG::GAME_HEIGHT - 10 - 5*32);
	addPlant(168*32 + 16, CCFG::GAME_HEIGHT - 10 - 4*32);

	addHammerBro(63*32, CCFG::GAME_HEIGHT - 16 - 3*32);
	addHammerBro(65*32, CCFG::GAME_HEIGHT - 16 - 7*32);
	addHammerBro(117*32, CCFG::GAME_HEIGHT - 16 - 7*32);
	addHammerBro(119*32, CCFG::GAME_HEIGHT - 16 - 3*32);
	addHammerBro(146*32, CCFG::GAME_HEIGHT - 16 - 3*32);
	addHammerBro(159*32, CCFG::GAME_HEIGHT - 16 - 3*32);
	addHammerBro(177*32, CCFG::GAME_HEIGHT - 16 - 3*32);
	addHammerBro(185*32, CCFG::GAME_HEIGHT - 16 - 3*32);
}

void Map::loadMinionsLVL_8_4() {
	clearMinions();

	addPlant(19*32 + 16, CCFG::GAME_HEIGHT - 10 - 3*32);
	addPlant(51*32 + 16, CCFG::GAME_HEIGHT - 10 - 3*32);
	addPlant(81*32 + 16, CCFG::GAME_HEIGHT - 10 - 6*32);
	addPlant(126*32 + 16, CCFG::GAME_HEIGHT - 10 - 3*32);
	addPlant(133*32 + 16, CCFG::GAME_HEIGHT - 10 - 4*32);
	addPlant(143*32 + 16, CCFG::GAME_HEIGHT - 10 - 3*32);
	addPlant(153*32 + 16, CCFG::GAME_HEIGHT - 10 - 4*32);
	addPlant(163*32 + 16, CCFG::GAME_HEIGHT - 10 - 8*32);
	addPlant(215*32 + 16, CCFG::GAME_HEIGHT - 10 - 3*32);
	addPlant(302*32 + 16, CCFG::GAME_HEIGHT - 10 - 3*32);
	addPlant(224*32 + 16, CCFG::GAME_HEIGHT - 10 - 6*32);
	addPlant(232*32 + 16, CCFG::GAME_HEIGHT - 10 - 7*32);
	addPlant(248*32 + 16, CCFG::GAME_HEIGHT - 10 - 6*32);
	addPlant(309*32 + 16, CCFG::GAME_HEIGHT - 10 - 3*32);

	addBeetle(139*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addBeetle(141*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);

	addGoombas(56*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(57*32 + 16, CCFG::GAME_HEIGHT - 16 - 2*32, true);
	addGoombas(59*32, CCFG::GAME_HEIGHT - 16 - 2*32, true);

	addHammerBro(316*32, CCFG::GAME_HEIGHT - 16 - 3*32);

	this->iLevelType = 1;

	addKoppa(150*32 - 8, CCFG::GAME_HEIGHT - 16 - 4*32, 0, true);
	addKoppa(152*32 - 8, CCFG::GAME_HEIGHT - 16 - 3*32, 0, true);
	addKoppa(165*32 - 8, CCFG::GAME_HEIGHT - 16 - 3*32, 0, true);
	addKoppa(167*32 - 8, CCFG::GAME_HEIGHT - 16 - 4*32, 0, true);

	this->iLevelType = 3;

	addCheepSpawner(224*32, 237*32);

	addBowser(338*32, CCFG::GAME_HEIGHT - 16 - 6*32, true);

	addToad(356*32, CCFG::GAME_HEIGHT - 3*32, true);

	addUpFire(332*32, 9*32);

	addFireBall(410*32, CCFG::GAME_HEIGHT - 16 - 7*32, 6, rand()%360, true);
	addFireBall(421*32, CCFG::GAME_HEIGHT - 16 - 4*32, 6, rand()%360, false);
	addFireBall(430*32, CCFG::GAME_HEIGHT - 16 - 8*32, 6, rand()%360, true);
	addFireBall(446*32, CCFG::GAME_HEIGHT - 16 - 6*32, 6, rand()%360, true);
	addFireBall(454*32, CCFG::GAME_HEIGHT - 16 - 7*32, 6, rand()%360, false);

	addSquid(418*32, CCFG::GAME_HEIGHT - 16 - 3*32);
	addSquid(441*32, CCFG::GAME_HEIGHT - 16 - 4*32);
	addSquid(443*32, CCFG::GAME_HEIGHT - 16 - 8*32);
}

void Map::createMap() {
	// ----- MIONION LIST -----

	for(int i = 0; i < iMapWidth; i += 5) {
		std::vector<Minion*> temp;
		lMinion.push_back(temp);
	}

	iMinionListSize = lMinion.size();

	// ----- MIONION LIST -----
	// ----- CREATE MAP -----

	for(int i = 0; i < iMapWidth; i++) {
		std::vector<MapLevel*> temp;
		for(int i = 0; i < iMapHeight; i++) {
			temp.push_back(new MapLevel(0));
		}

		lMap.push_back(temp);
	}

	// ----- CREATE MAP -----

	this->underWater = false;
	this->bTP = false;
}

void Map::checkSpawnPoint() {
	if(getNumOfSpawnPoints() > 1) {
		for(int i = iSpawnPointID + 1; i < getNumOfSpawnPoints(); i++) {
			if(getSpawnPointXPos(i) > oPlayer->getXPos() - fXPos && getSpawnPointXPos(i) < oPlayer->getXPos() - fXPos + 128) {
				iSpawnPointID = i;
			}
		}
	}
}

int Map::getNumOfSpawnPoints() {
	switch(currentLevelID) {
		case 0: case 1: case 2: case 4: case 5: case 8: case 9: case 10: case 13: case 14: case 16: case 17: case 18: case 20: case 21: case 22: case 24: case 25: case 26:
			return 2;
	}

	return 1;
}

int Map::getSpawnPointXPos(int iID) {
	switch(currentLevelID) {
		case 0:
			switch(iID) {
				case 0:
					return 84;
				case 1:
					return 82*32;
			}
		case 1:
			switch(iID) {
				case 0:
					return 84;
				case 1:
					return 89*32;
			}
		case 2:
			switch(iID) {
				case 0:
					return 84;
				case 1:
					return 66*32;
			}
		case 3:
			return 64;
		case 4:
			switch(iID) {
				case 0:
					return 84;
				case 1:
					return 98*32;
			}
		case 5:
			switch(iID) {
				case 0:
					return 84;
				case 1:
					return 86*32;
			}
		case 6:
			switch(iID) {
				case 0:
					return 84;
				case 1:
					return 114*32;
			}
		case 7:
			return 64;
		case 8:
			switch(iID) {
				case 0:
					return 84;
				case 1:
					return 90*32;
			}
		case 9:
			switch(iID) {
				case 0:
					return 84;
				case 1:
					return 98*32;
			}
		case 10:
			switch(iID) {
				case 0:
					return 84;
				case 1:
					return 66*32;
			}
		case 13:
			switch(iID) {
				case 0:
					return 84;
				case 1:
					return 95*32;
			}
		case 14:
			switch(iID) {
				case 0:
					return 84;
				case 1:
					return 65*32;
			}
		case 16:
			switch(iID) {
				case 0:
					return 84;
				case 1:
					return 97*32;
			}
		case 17:
			switch(iID) {
				case 0:
					return 84 + 80*32;
				case 1:
					return 177*32;
			}
		case 18:
			switch(iID) {
				case 0:
					return 84;
				case 1:
					return 66*32;
			}
		case 20:
			switch(iID) {
				case 0:
					return 84;
				case 1:
					return 98*32;
			}
		case 21:
			switch(iID) {
				case 0:
					return 84 + 85*32;
				case 1:
					return 183*32;
			}
		case 22:
			switch(iID) {
				case 0:
					return 84;
				case 1:
					return 98*32;
			}
		case 24:
			switch(iID) {
				case 0:
					return 84;
				case 1:
					return 98*32;
			}
		case 25:
			switch(iID) {
				case 0:
					return 84;
				case 1:
					return 86*32;
			}
		case 26:
			switch(iID) {
				case 0:
					return 84;
				case 1:
					return 113*32;
			}
	}

	return 84;
}

int Map::getSpawnPointYPos(int iID) {
	switch(currentLevelID) {
		case 1:
			switch(iID) {
				case 0:
					return 64;
			}
		case 5: case 25:
			switch(iID) {
				case 0:
					return 64;
				case 1:
					return CCFG::GAME_HEIGHT - 48 - oPlayer->getHitBoxY();;
			}
		case 3: case 7: case 11: case 15: case 19: case 23: case 27: case 31:
			return 150;
	}

	return CCFG::GAME_HEIGHT - 48 - oPlayer->getHitBoxY();
}

void Map::setSpawnPoint() {
	float X = (float)getSpawnPointXPos(iSpawnPointID);

	if(X > 6*32) {
		fXPos = -(X - 6*32);
		oPlayer->setXPos(6*32);
		oPlayer->setYPos((float)getSpawnPointYPos(iSpawnPointID));
	} else {
		fXPos = 0;
		oPlayer->setXPos(X);
		oPlayer->setYPos((float)getSpawnPointYPos(iSpawnPointID));
	}

	oPlayer->setMoveDirection(true);
}

void Map::resetGameData() {
	this->currentLevelID = 0;
	this->iSpawnPointID = 0;

	oPlayer->setCoins(0);
	oPlayer->setScore(0);
	oPlayer->resetPowerLVL();

	oPlayer->setNumOfLives(3);

	setSpawnPoint();

	loadLVL();
}

void Map::loadLVL() {
	clearPipeEvents();

	switch(currentLevelID) {
		case 0:
			loadLVL_1_1();
			break;
		case 1:
			loadLVL_1_2();
			break;
		case 2:
			loadLVL_1_3();
			break;
		case 3:
			loadLVL_1_4();
			break;
		case 4:
			loadLVL_2_1();
			break;
		case 5:
			loadLVL_2_2();
			break;
		case 6:
			loadLVL_2_3();
			break;
		case 7:
			loadLVL_2_4();
			break;
		case 8:
			loadLVL_3_1();
			break;
		case 9:
			loadLVL_3_2();
			break;
		case 10:
			loadLVL_3_3();
			break;
		case 11:
			loadLVL_3_4();
			break;
		case 12:
			loadLVL_4_1();
			break;
		case 13:
			loadLVL_4_2();
			break;
		case 14:
			loadLVL_4_3();
			break;
		case 15:
			loadLVL_4_4();
			break;
		case 16:
			loadLVL_5_1();
			break;
		case 17:
			loadLVL_5_2();
			break;
		case 18:
			loadLVL_5_3();
			break;
		case 19:
			loadLVL_5_4();
			break;
		case 20:
			loadLVL_6_1();
			break;
		case 21:
			loadLVL_6_2();
			break;
		case 22:
			loadLVL_6_3();
			break;
		case 23:
			loadLVL_6_4();
			break;
		case 24:
			loadLVL_7_1();
			break;
		case 25:
			loadLVL_7_2();
			break;
		case 26:
			loadLVL_7_3();
			break;
		case 27:
			loadLVL_7_4();
			break;
		case 28:
			loadLVL_8_1();
			break;
		case 29:
			loadLVL_8_2();
			break;
		case 30:
			loadLVL_8_3();
			break;
		case 31:
			loadLVL_8_4();
			break;
	}
}

// ---------- LEVELTEXT -----

void Map::clearLevelText() {
	for(unsigned int i = 0; i < vLevelText.size(); i++) {
		delete vLevelText[i];
	}

	vLevelText.clear();
}

// ---------- LOADPIPEEVENTS -----

void Map::clearPipeEvents() {
	for(unsigned int i = 0; i < lPipe.size(); i++) {
		delete lPipe[i];
	}

	lPipe.clear();
}

void Map::loadPipeEventsLVL_1_1() {
	lPipe.push_back(new Pipe(0, 57, 5, 58, 5, 242 * 32, 32, currentLevelID, 1, false, 350, 1, false));

	lPipe.push_back(new Pipe(1, 253, 3, 253, 2, 163 * 32, CCFG::GAME_HEIGHT - 16 - 3 * 32, currentLevelID, 0, true, 350, 1, false));
}

void Map::loadPipeEventsLVL_1_2() {
	lPipe.push_back(new Pipe(0, 103, 4, 104, 4, 242 * 32, 32, currentLevelID, 1, false, 350, 1, false));

	lPipe.push_back(new Pipe(1, 253, 3, 253, 2, 115 * 32, CCFG::GAME_HEIGHT - 16 - 3 * 32, currentLevelID, 1, true, 350, 1, false));

	lPipe.push_back(new Pipe(1, 166, 6, 166, 5, 284 * 32, CCFG::GAME_HEIGHT - 16 - 3 * 32, currentLevelID, 0, true, 350, 1, false));

	// -- WARP ZONE 2
	lPipe.push_back(new Pipe(0, 186, 4, 187, 4, 84, CCFG::GAME_HEIGHT - 80, 4, 0, true, 350, 1, false));
	// -- WARP ZONE 3
	lPipe.push_back(new Pipe(0, 182, 4, 183, 4, 84, CCFG::GAME_HEIGHT - 80, 8, 4, true, 350, 1, false));
	// -- WARP ZONE 4
	lPipe.push_back(new Pipe(0, 178, 4, 179, 4, 84, CCFG::GAME_HEIGHT - 80, 12, 0, true, 350, 1, false));
}

void Map::loadPipeEventsLVL_2_1() {
	lPipe.push_back(new Pipe(0, 103, 5, 104, 5, 232 * 32, 32, currentLevelID, 1, false, 350, 1, false));

	lPipe.push_back(new Pipe(1, 243, 3, 243, 2, 115 * 32, CCFG::GAME_HEIGHT - 16 - 3 * 32, currentLevelID, 0, true, 350, 1, false));
}

void Map::loadPipeEventsLVL_2_2() {
	lPipe.push_back(new Pipe(1, 189, 7, 189, 6, 274 * 32, CCFG::GAME_HEIGHT - 16 - 3 * 32, currentLevelID, 0, true, 350, 1, false));
}

void Map::loadPipeEventsLVL_3_1() {
	lPipe.push_back(new Pipe(0, 38, 5, 39, 5, 242 * 32, 32, currentLevelID, 1, false, 350, 1, false));

	lPipe.push_back(new Pipe(1, 253, 3, 253, 2, 67 * 32, CCFG::GAME_HEIGHT - 16 - 3 * 32, currentLevelID, 4, true, 350, 1, false));
}

void Map::loadPipeEventsLVL_4_1() {
	lPipe.push_back(new Pipe(0, 132, 5, 133, 5, 252 * 32, 32, currentLevelID, 1, false, 350, 1, false));

	lPipe.push_back(new Pipe(1, 263, 3, 263, 2, 163 * 32, CCFG::GAME_HEIGHT - 16 - 3 * 32, currentLevelID, 0, true, 350, 1, false));
}

void Map::loadPipeEventsLVL_4_2() {
	lPipe.push_back(new Pipe(0, 84, 4, 85, 4, 272 * 32, 32, currentLevelID, 1, false, 350, 1, false));

	lPipe.push_back(new Pipe(1, 283, 3, 283, 2, 131 * 32, CCFG::GAME_HEIGHT - 16 - 3 * 32, currentLevelID, 1, true, 350, 1, false));

	lPipe.push_back(new Pipe(1, 187, 6, 187, 5, 394 * 32, CCFG::GAME_HEIGHT - 16 - 3 * 32, currentLevelID, 0, true, 350, 1, false));

	// -- WARP ZONE 5
	lPipe.push_back(new Pipe(0, 214, 4, 215, 4, 84, CCFG::GAME_HEIGHT - 80, 16, 0, true, 350, 1, false));
	// -- WARP ZONE 6
	lPipe.push_back(new Pipe(0, 368, 4, 369, 4, 84, CCFG::GAME_HEIGHT - 80, 20, 4, true, 350, 1, false));
	// -- WARP ZONE 7
	lPipe.push_back(new Pipe(0, 364, 4, 365, 4, 84, CCFG::GAME_HEIGHT - 80, 24, 0, true, 350, 1, false));
	// -- WARP ZONE 8
	lPipe.push_back(new Pipe(0, 360, 4, 361, 4, 84, CCFG::GAME_HEIGHT - 80, 28, 0, true, 350, 1, false));
}

void Map::loadPipeEventsLVL_5_1() {
	lPipe.push_back(new Pipe(0, 156, 7, 157, 7, 230 * 32, 32, currentLevelID, 1, false, 350, 1, false));

	lPipe.push_back(new Pipe(1, 241, 3, 241, 2, 163 * 32, CCFG::GAME_HEIGHT - 16 - 3 * 32, currentLevelID, 0, true, 350, 1, false));
}

void Map::loadPipeEventsLVL_5_2() {
	lPipe.push_back(new Pipe(0, 135, 4, 136, 4, 0, 32, currentLevelID, 2, true, 350, 1, true));

	lPipe.push_back(new Pipe(1, 62, 7, 62, 6, 195 * 32, CCFG::GAME_HEIGHT - 16 - 3 * 32, currentLevelID, 0, true, 350, 1, false));
}

void Map::loadPipeEventsLVL_6_2() {
	lPipe.push_back(new Pipe(0, 141, 6, 142, 6, 0, 32, currentLevelID, 2, true, 350, 1, true));

	lPipe.push_back(new Pipe(1, 62, 7, 62, 6, 200 * 32, CCFG::GAME_HEIGHT - 16 - 3 * 32, currentLevelID, 4, true, 350, 1, false));

	lPipe.push_back(new Pipe(0, 104, 5, 105, 5, 332 * 32, 32, currentLevelID, 1, false, 350, 1, false));

	lPipe.push_back(new Pipe(1, 343, 3, 343, 2, 120 * 32, CCFG::GAME_HEIGHT - 16 - 3 * 32, currentLevelID, 4, true, 350, 1, false));

	lPipe.push_back(new Pipe(0, 238, 4, 239, 4, 362 * 32, 32, currentLevelID, 1, false, 350, 1, false));

	lPipe.push_back(new Pipe(1, 373, 3, 373, 2, 264 * 32, CCFG::GAME_HEIGHT - 16 - 3 * 32, currentLevelID, 4, true, 350, 1, false));
}

void Map::loadPipeEventsLVL_7_1() {
	lPipe.push_back(new Pipe(0, 93, 4, 94, 4, 212 * 32, 32, currentLevelID, 1, false, 350, 1, false));

	lPipe.push_back(new Pipe(1, 223, 3, 223, 2, 115 * 32, CCFG::GAME_HEIGHT - 16 - 3 * 32, currentLevelID, 0, true, 350, 1, false));
}

void Map::loadPipeEventsLVL_7_2() {
	lPipe.push_back(new Pipe(1, 189, 7, 189, 6, 274 * 32, CCFG::GAME_HEIGHT - 16 - 3 * 32, currentLevelID, 0, true, 350, 1, false));
}

void Map::loadPipeEventsLVL_8_1() {
	lPipe.push_back(new Pipe(0, 104, 5, 105, 5, 412 * 32, 32, currentLevelID, 1, false, 350, 1, false));

	lPipe.push_back(new Pipe(1, 423, 3, 423, 2, 115 * 32, CCFG::GAME_HEIGHT - 16 - 3 * 32, currentLevelID, 0, true, 350, 1, false));
}

void Map::loadPipeEventsLVL_8_2() {
	lPipe.push_back(new Pipe(0, 156, 5, 157, 5, 242 * 32, 32, currentLevelID, 1, false, 350, 1, false));

	lPipe.push_back(new Pipe(1, 253, 3, 253, 2, 163 * 32, CCFG::GAME_HEIGHT - 16 - 3 * 32, currentLevelID, 0, true, 350, 1, false));
}

void Map::loadPipeEventsLVL_8_4() {
	lPipe.push_back(new Pipe(2, 81, 6, 82, 6, 126 * 32, CCFG::GAME_HEIGHT - 16 - 3 * 32, currentLevelID, 3, true, 250, 1, false));

	lPipe.push_back(new Pipe(2, 163, 8, 164, 8, 215 * 32, CCFG::GAME_HEIGHT - 16 - 3 * 32, currentLevelID, 3, true, 250, 1, false));

	lPipe.push_back(new Pipe(2, 248, 6, 249, 6, 393 * 32, CCFG::GAME_HEIGHT - 16 - 3 * 32, currentLevelID, 2, true, 250, 1, true));
	lPipe.push_back(new Pipe(1, 458, 7, 458, 6, 302 * 32, CCFG::GAME_HEIGHT - 16 - 3 * 32, currentLevelID, 3, true, 350, 1, false));

	lPipe.push_back(new Pipe(2, 51, 4, 52, 4, 19 * 32, CCFG::GAME_HEIGHT - 16 - 3 * 32, currentLevelID, 3, true, 250, 1, false));
	lPipe.push_back(new Pipe(2, 143, 3, 144, 3, 19 * 32, CCFG::GAME_HEIGHT - 16 - 3 * 32, currentLevelID, 3, true, 250, 1, false));
	lPipe.push_back(new Pipe(2, 232, 7, 232, 7, 19 * 32, CCFG::GAME_HEIGHT - 16 - 3 * 32, currentLevelID, 3, true, 250, 1, false));
	lPipe.push_back(new Pipe(2, 309, 3, 310, 3, 19 * 32, CCFG::GAME_HEIGHT - 16 - 3 * 32, currentLevelID, 3, true, 250, 1, false));
}

// ---------- LOADPIPEEVENTS -----

void Map::loadLVL_1_1() {
	clearMap();

	this->iMapWidth = 260;
	this->iMapHeight = 25;
	this->iLevelType = 0;
	this->iMapTime = 400;

	// ---------- LOAD LISTS ----------
	createMap();

	// ----- MINIONS
	loadMinionsLVL_1_1();

	// ----- PIPEEVENT
	loadPipeEventsLVL_1_1();

	// ----- Bush -----

	structBush(0, 2, 2);
	structBush(16, 2, 1);
	structBush(48, 2, 2);
	structBush(64, 2, 1);
	structBush(96, 2, 2);
	structBush(112, 2, 1);
	structBush(144, 2, 2);
	structBush(160, 2, 1);
	structBush(192, 2, 2);
	structBush(208, 2, 1);

	// ----- Bush -----

	// ----- Clouds -----

	structCloud(8, 10, 1);
	structCloud(19, 11, 1);
	structCloud(27, 10, 3);
	structCloud(36, 11, 2);
	structCloud(56, 10, 1);
	structCloud(67, 11, 1);
	structCloud(75, 10, 3);
	structCloud(84, 11, 2);
	structCloud(104, 10, 1);
	structCloud(115, 11, 1);
	structCloud(123, 10, 3);
	structCloud(132, 11, 2);
	structCloud(152, 10, 1);
	structCloud(163, 11, 1);
	structCloud(171, 10, 3);
	structCloud(180, 11, 2);
	structCloud(200, 10, 1);
	structCloud(211, 11, 1);
	structCloud(219, 10, 3);

	// ----- Clouds -----

	// ----- Grass -----

	structGrass(11, 2, 3);
	structGrass(23, 2, 1);
	structGrass(41, 2, 2);
	structGrass(59, 2, 3);
	structGrass(71, 2, 1);
	structGrass(89, 2, 2);
	structGrass(107, 2, 3);
	structGrass(119, 2, 1);
	structGrass(137, 2, 2);
	structGrass(157, 2, 1);
	structGrass(167, 2, 1);
	structGrass(205, 2, 1);
	structGrass(215, 2, 1);

	// ----- Grass -----

	// ----- GND -----

	structGND(0, 0, 69, 2);

	structGND(71, 0, 15, 2);

	structGND(89, 0, 64, 2);

	structGND(155, 0, 85, 2);

	// ----- GND -----

	// ----- GND 2 -----

	structGND2(134, 2, 4, true);
	structGND2(140, 2, 4, false);
	structGND2(148, 2, 4, true);
	structGND2(152, 2, 1, 4);
	structGND2(155, 2, 4, false);
	structGND2(181, 2, 8, true);
	structGND2(189, 2, 1, 8);

	structGND2(198, 2, 1, 1);

	// ----- GND 2 -----

	// ----- BRICK -----

	struckBlockQ(16, 5, 1);
	structBrick(20, 5, 1, 1);
	struckBlockQ(21, 5, 1);
	lMap[21][5]->setSpawnMushroom(true);
	structBrick(22, 5, 1, 1);
	struckBlockQ(22, 9, 1);
	struckBlockQ(23, 5, 1);
	structBrick(24, 5, 1, 1);

	struckBlockQ2(64, 6, 1);
	lMap[64][6]->setSpawnMushroom(true);
	lMap[64][6]->setPowerUP(false);

	structBrick(77, 5, 1, 1);
	struckBlockQ(78, 5, 1);
	lMap[78][5]->setSpawnMushroom(true);
	structBrick(79, 5, 1, 1);

	structBrick(80, 9, 8, 1);
	structBrick(91, 9, 3, 1);
	struckBlockQ(94, 9, 1);
	structBrick(94, 5, 1, 1);
	lMap[94][5]->setNumOfUse(4);

	structBrick(100, 5, 2, 1);

	struckBlockQ(106, 5, 1);
	struckBlockQ(109, 5, 1);
	struckBlockQ(109, 9, 1);
	lMap[109][9]->setSpawnMushroom(true);
	struckBlockQ(112, 5, 1);

	structBrick(118, 5, 1, 1);

	structBrick(121, 9, 3, 1);

	structBrick(128, 9, 1, 1);
	struckBlockQ(129, 9, 2);
	structBrick(131, 9, 1, 1);

	structBrick(129, 5, 2, 1);

	structBrick(168, 5, 2, 1);
	struckBlockQ(170, 5, 1);
	structBrick(171, 5, 1, 1);

	lMap[101][5]->setSpawnStar(true);

	// ----- BRICK -----

	// ----- PIPES -----

	structPipe(28, 2, 1);
	structPipe(38, 2, 2);
	structPipe(46, 2, 3);
	structPipe(57, 2, 3);
	structPipe(163, 2, 1);
	structPipe(179, 2, 1);

	// ----- PIPES -----

	// ----- END

	structEnd(198, 3, 9);
	structCastleSmall(202, 2);

	// ----- MAP 1_1_2 -----

	this->iLevelType = 1;

	structGND(240, 0, 17, 2);

	structBrick(240, 2, 1, 11);
	structBrick(244, 2, 7, 3);
	structBrick(244, 12, 7, 1);

	structPipeVertical(255, 2, 10);
	structPipeHorizontal(253, 2, 1);

	structCoins(244, 5, 7, 1);
	structCoins(244, 7, 7, 1);
	structCoins(245, 9, 5, 1);

	// ----- END LEVEL

	this->iLevelType = 0;
}

void Map::loadLVL_1_2() {
	clearMap();

	this->iMapWidth = 330;
	this->iMapHeight = 25;
	this->iLevelType = 1;
	this->iMapTime = 400;

	// ---------- LOAD LISTS ----------

	createMap();

	// ---------- LOAD LISTS ----------

	// ----- MINIONS
	loadMinionsLVL_1_2();

	// ----- PIPEEVENTS
	loadPipeEventsLVL_1_2();

	vPlatform.push_back(new Platform(6, 0, 139*32 + 16, 139*32 + 16, -32, CCFG::GAME_HEIGHT + 32, 139*32 + 16, 9*32 + 16, true));
	vPlatform.push_back(new Platform(6, 0, 139*32 + 16, 139*32 + 16, -32, CCFG::GAME_HEIGHT + 32, 139*32 + 16, 1*32 + 16, true));

	vPlatform.push_back(new Platform(6, 1, 154*32 + 16, 154*32 + 16, -32, CCFG::GAME_HEIGHT + 32, 154*32 + 16, 10*32, true));
	vPlatform.push_back(new Platform(6, 1, 154*32 + 16, 154*32 + 16, -32, CCFG::GAME_HEIGHT + 32, 154*32 + 16, 2*32, true));

	vLevelText.push_back(new LevelText(178*32, CCFG::GAME_HEIGHT - 16 - 8*32, "WELCOME TO WARP ZONEz"));

	vLevelText.push_back(new LevelText(178*32 + 16, CCFG::GAME_HEIGHT - 6*32, "4"));
	vLevelText.push_back(new LevelText(182*32 + 16, CCFG::GAME_HEIGHT - 6*32, "3"));
	vLevelText.push_back(new LevelText(186*32 + 16, CCFG::GAME_HEIGHT - 6*32, "2"));

	// ----- GND -----

	structGND(0, 0, 80, 2);

	structGND(83, 0, 37, 2);

	structGND(122, 0, 2, 2);

	structGND(126, 0, 12, 2);

	structGND(145, 0, 8, 2);

	structGND(160, 0, 43, 2);

	// ----- GND -----

	// ----- Brick -----

	structBrick(0, 2, 1, 11);
	structBrick(122, 2, 2, 3);

	structBrick(6, 12, 132, 1);

	structBrick(29, 6, 1, 1);
	lMap[29][6]->setNumOfUse(10);

	structBrick(39, 5, 1, 3);
	structBrick(40, 5, 1, 1);
	structBrick(41, 5, 1, 3);
	structBrick(42, 7, 2, 1);
	structBrick(44, 5, 1, 3);
	structBrick(45, 5, 1, 1);
	structBrick(46, 5, 1, 3);

	structBrick(52, 5, 2, 5);
	structBrick(54, 3, 2, 3);
	structBrick(54, 10, 2, 2);
	structBrick(58, 10, 6, 2);
	structBrick(58, 5, 6, 1);
	structBrick(62, 6, 2, 4);

	structBrick(66, 10, 4, 2);
	structBrick(67, 5, 1, 5);
	structBrick(68, 5, 1, 1);
	structBrick(69, 5, 1, 2);
	lMap[69][6]->setSpawnMushroom(true);

	structBrick(72, 5, 2, 5);
	lMap[73][6]->setNumOfUse(10);

	structBrick(76, 5, 4, 1);
	structBrick(76, 10, 4, 2);

	structBrick(84, 6, 6, 2);
	lMap[89][12]->setSpawnMushroom(true);
	lMap[89][12]->setPowerUP(false);

	structBrick(145, 6, 6, 1);
	lMap[150][6]->setSpawnMushroom(true);

	structBrick(160, 2, 17, 3);
	structBrick(170, 5, 7, 8);
	structBrick(161, 12, 7, 1);
	structBrick(177, 12, 10, 1);

	structBrick(190, 2, 13, 17);

	lMap[46][7]->setSpawnStar(true);

	// ----- Brick -----

	// ----- GND2 -----

	structGND2(17, 2, 1, 1);
	structGND2(19, 2, 1, 2);
	structGND2(21, 2, 1, 3);
	structGND2(23, 2, 1, 4);
	structGND2(25, 2, 1, 4);
	structGND2(27, 2, 1, 3);
	structGND2(31, 2, 1, 3);
	structGND2(33, 2, 1, 2);

	structGND2(133, 2, 4, true);
	structGND2(137, 2, 1, 4);

	// ----- GND2 -----

	// ----- BLOCKQ -----

	struckBlockQ(10, 5, 5);
	lMap[10][5]->setSpawnMushroom(true);

	// ----- BLOCKQ -----

	// ----- PIPES -----

	structPipe(103, 2, 2);
	structPipe(109, 2, 3);
	structPipe(115, 2, 1);

	structPipeVertical(168, 5, 7);
	structPipeHorizontal(166, 5, 1);

	structPipe(178, 2, 2);
	structPipe(182, 2, 2);
	structPipe(186, 2, 2);

	// ----- PIPES -----

	// ----- COIN -----

	structCoins(40, 6, 1, 1);
	structCoins(41, 9, 4, 1);
	structCoins(45, 6, 1, 1);
	structCoins(58, 6, 4, 1);
	structCoins(68, 6, 1, 1);

	structCoins(84, 9, 6, 1);

	// ----- MAP 1-1-2

	this->iLevelType = 0;

	structGND(210, 0, 24, 2);

	structCastleSmall(210, 2);

	structCloud(213, 10, 2);
	structCloud(219, 6, 1);
	structCloud(228, 11, 2);

	structPipe(222, 2, 3);
	structPipeHorizontal(220, 2, 1);

	// ----- MAP 1-1-2
	// ----- MAP 1-1-3

	this->iLevelType = 1;

	structGND(240, 0, 17, 2);

	structBrick(240, 2, 1, 11);
	structBrick(243, 5, 10, 1);
	structBrick(243, 9, 10, 4);
	structBrick(253, 4, 2, 9);

	lMap[252][5]->setNumOfUse(10);

	structCoins(243, 2, 9, 1);
	structCoins(244, 6, 8, 1);

	structPipeVertical(255, 2, 10);
	structPipeHorizontal(253, 2, 1);

	// ----- MAP 1-1-3
	// ----- MAP 1-1-4

	this->iLevelType = 0;

	structCloud(285, 11, 2);
	structCloud(305, 10, 1);
	structCloud(316, 11, 1);

	structGrass(310, 2, 1);
	structGrass(320, 2, 1);

	structBush(297, 2, 2);
	structBush(313, 2, 1);

	structGND(281, 0, 42, 2);

	structPipe(284, 2, 1);

	structGND2(286, 2, 8, true);
	structGND2(294, 2, 1, 8);

	structGND2(303, 2, 1, 1);
	structEnd(303, 3, 9);

	structCastleSmall(307, 2);

	// ----- MAP 1-1-4

	// ----- END

	this->iLevelType = 1;
}

void Map::loadLVL_1_3() {
	clearMap();

	this->iMapWidth = 270;
	this->iMapHeight = 25;
	this->iLevelType = 0;
	this->iMapTime = 300;

	// ---------- LOAD LISTS ----------

	createMap();

	loadMinionsLVL_1_3();

	// ---------- LOAD LISTS ----------

	vPlatform.push_back(new Platform(6, 2, 55*32, 55*32, CCFG::GAME_HEIGHT - 16 - 9*32, CCFG::GAME_HEIGHT - 32, 55*32, (float)CCFG::GAME_HEIGHT - 32, true));

	vPlatform.push_back(new Platform(6, 3, 83*32 - 16, 86*32 + 16, CCFG::GAME_HEIGHT - 16 - 6*32, CCFG::GAME_HEIGHT - 16 - 6*32, 83*32 - 16, (float)CCFG::GAME_HEIGHT - 16 - 6*32, true));
	
	vPlatform.push_back(new Platform(6, 3, 91*32 - 16, 93*32 + 16, CCFG::GAME_HEIGHT - 16 - 5*32, CCFG::GAME_HEIGHT - 16 - 5*32, 93*32 + 16, (float)CCFG::GAME_HEIGHT - 16 - 5*32, false));

	vPlatform.push_back(new Platform(6, 3, 127*32 - 16, 131*32 + 16, CCFG::GAME_HEIGHT - 16 - 8*32, CCFG::GAME_HEIGHT - 16 - 8*32, 127*32 - 16, (float)CCFG::GAME_HEIGHT - 16 - 8*32, false));

	// ----- CLOUDS

	structCloud(3, 10, 2);
	structCloud(9, 6, 1);
	structCloud(11, 18, 2);
	structCloud(18, 11, 2);
	structCloud(35, 6, 1);
	structCloud(38, 7, 1);
	structCloud(46, 2, 1);
	structCloud(51, 10, 2);
	structCloud(57, 6, 1);
	structCloud(66, 11, 2);
	structCloud(76, 2, 1);
	structCloud(83, 6, 1);
	structCloud(86, 7, 1);
	structCloud(94, 2, 1);
	structCloud(99, 10, 2);
	structCloud(131, 6, 1);
	structCloud(134, 7, 1);
	structCloud(142, 2, 1);
	structCloud(147, 10, 2);
	structCloud(153, 6, 1);
	structCloud(162, 11, 2);

	// ----- GND

	structGND(0, 0, 16, 2);
	structGND(129, 0, 47, 2);
	// ----- T

	structT(18, 0, 4, 3);
	structT(24, 0, 8, 6);
	structT(26, 6, 5, 4);
	structT(32, 0, 3, 3);
	structT(35, 0, 5, 7);
	structT(40, 0, 7, 11);
	structT(50, 0, 4, 2);
	structT(59, 0, 5, 2);
	structT(60, 2, 4, 8);
	structT(50, 0, 4, 2);
	structT(65, 0, 5, 2);
	structT(70, 0, 3, 6);
	structT(76, 0, 6, 9);
	structT(98, 0, 4, 4);
	structT(104, 0, 8, 8);
	structT(113, 0, 3, 2);
	structT(116, 0, 4, 6);
	structT(122, 0, 4, 6);

	// ----- GND2

	struckBlockQ(59, 4, 1);
	lMap[59][4]->setSpawnMushroom(true);

	structGND2(138, 2, 2, 4);
	structGND2(140, 2, 2, 6);
	structGND2(142, 2, 2, 8);

	// ----- COINS
	
	structCoins(27, 10, 3, 1);
	structCoins(33, 3, 1, 1);
	structCoins(37, 12, 2, 1);
	structCoins(50, 8, 2, 1);
	structCoins(60, 10, 4, 1);
	structCoins(85, 9, 2, 1);
	structCoins(93, 10, 2, 1);
	structCoins(97, 10, 2, 1);
	structCoins(113, 2, 3, 1);
	structCoins(120, 9, 2, 1);

	// ----- END

	structGND2(152, 2, 1, 1);
	structEnd(152, 3, 9);

	// ----- CASTLE

	structCastleSmall(0, 2);
	structCastleBig(155, 2);
	structCastleWall(164, 2, 12, 6);

	this->iLevelType = 0;
}

void Map::loadLVL_1_4() {
	clearMap();

	this->iMapWidth = 180;
	this->iMapHeight = 25;
	this->iLevelType = 3;
	this->iMapTime = 300;

	// ---------- LOAD LISTS ----------

	createMap();

	loadMinionsLVL_1_4();

	vPlatform.push_back(new Platform(4, 3, 136*32 - 16, 138*32 + 16, CCFG::GAME_HEIGHT - 16 - 8*32, CCFG::GAME_HEIGHT - 16 - 8*32, 136*32 - 16, (float)CCFG::GAME_HEIGHT - 16 - 8*32, false));

	structGND(0, 0, 3, 8);
	structGND(3, 0, 1, 7);
	structGND(4, 0, 1, 6);
	structGND(5, 0, 8, 5);
	structGND(15, 0, 11, 5);
	structGND(29, 0, 3, 5);
	structGND(35, 0, 37, 6);
	structGND(72, 0, 32, 5);
	structGND(104, 0, 24, 2);
	structGND(116, 2, 4, 3);
	structGND(123, 2, 5, 3);
	structGND(141, 0, 3, 6);
	structGND(144, 0, 19, 2);

	structGND(0, 10, 24, 3);
	structGND(23, 9, 1, 4);
	structGND(24, 12, 13, 1);
	structGND(37, 9, 35, 4);
	structGND(72, 12, 91, 1);
	structGND(80, 11, 1, 1);
	structGND(88, 11, 1, 1);
	structGND(97, 10, 7, 2);
	structGND(123, 10, 5, 2);
	structGND(142, 9, 2, 3);

	structBridge(128, 4, 13);

	structLava(13, 0, 2, 3);
	structLava(26, 0, 3, 2);
	structLava(32, 0, 3, 2);
	structLava(128, 0, 13, 2);

	lMap[23][8]->setBlockID(56);
	lMap[37][8]->setBlockID(56);
	lMap[80][10]->setBlockID(56);
	lMap[92][5]->setBlockID(56);

	lMap[30][4]->setBlockID(56);
	lMap[49][8]->setBlockID(56);
	lMap[60][8]->setBlockID(56);
	lMap[67][8]->setBlockID(56);
	lMap[76][5]->setBlockID(56);
	lMap[84][5]->setBlockID(56);
	lMap[88][10]->setBlockID(56);
	
	struckBlockQ(30, 8, 1);
	lMap[30][8]->setSpawnMushroom(true);

	struckBlockQ2(106, 5, 1);
	struckBlockQ2(109, 5, 1);
	struckBlockQ2(112, 5, 1);
	struckBlockQ2(107, 9, 1);
	struckBlockQ2(110, 9, 1);
	struckBlockQ2(113, 9, 1);

	this->iLevelType = 3;
}

void Map::loadLVL_2_1() {
	clearMap();

	this->iMapWidth = 365;
	this->iMapHeight = 25;
	this->iLevelType = 0;
	this->iMapTime = 400;

	// ---------- LOAD LISTS ----------

	createMap();

	structGND(0, 0, 92, 2);
	structGND(96, 0, 10, 2);
	structGND(109, 0, 30, 2);
	structGND(142, 0, 10, 2);
	structGND(154, 0, 70, 2);

	loadMinionsLVL_2_1();
	loadPipeEventsLVL_2_1();

	structCloud(18, 10, 1);
	structCloud(27, 11, 1);
	structCloud(30, 10, 2);
	structCloud(45, 11, 1);
	structCloud(48, 10, 2);
	structCloud(66, 10, 1);
	structCloud(75, 11, 1);
	structCloud(78, 10, 2);
	structCloud(93, 11, 1);
	structCloud(96, 10, 2);
	structCloud(114, 10, 1);
	structCloud(123, 11, 1);
	structCloud(126, 10, 2);
	structCloud(141, 11, 1);
	structCloud(144, 10, 2);
	structCloud(162, 10, 1);
	structCloud(171, 11, 1);
	structCloud(174, 10, 2);
	structCloud(189, 11, 1);
	structCloud(192, 10, 2);
	structCloud(210, 10, 1);
	structCloud(219, 11, 1);

	structFence(14, 2, 4);
	structFence(38, 2, 2);
	structFence(41, 2, 1);
	structFence(62, 2, 4);
	structFence(86, 2, 2);
	structFence(89, 2, 1);
	structFence(110, 2, 4);
	structFence(134, 2, 2);
	structFence(137, 2, 1);
	structFence(158, 2, 4);
	structFence(182, 2, 2);
	structFence(185, 2, 1);
	structFence(209, 2, 1);

	structPipe(46, 2, 3);
	structPipe(74, 2, 3);
	structPipe(103, 2, 3);
	structPipe(115, 2, 1);
	structPipe(122, 2, 3);
	structPipe(126, 2, 2);
	structPipe(130, 2, 4);
	structPipe(176, 2, 2);

	structCastleBig(-2, 2);

	structTree(11, 2, 1, false);
	structTree(13, 2, 1, true);
	structTree(21, 2, 1, true);
	structTree(40, 2, 1, false);
	structTree(43, 2, 1, true);
	structTree(59, 2, 1, false);
	structTree(61, 2, 1, true);
	structTree(69, 2, 1, true);
	structTree(71, 2, 1, false);
	structTree(72, 2, 1, false);
	structTree(88, 2, 1, false);
	structTree(91, 2, 1, true);
	structTree(109, 2, 1, true);
	structTree(117, 2, 1, true);
	structTree(119, 2, 1, false);
	structTree(120, 2, 1, false);
	structTree(136, 2, 1, false);
	structTree(155, 2, 1, false);
	structTree(157, 2, 1, true);
	structTree(165, 2, 1, true);
	structTree(167, 2, 1, false);
	structTree(168, 2, 1, false);
	structTree(184, 2, 1, false);
	structTree(187, 2, 1, true);
	structTree(203, 2, 1, false);
	structTree(213, 2, 1, true);
	structTree(215, 2, 1, false);
	structTree(216, 2, 1, false);

	structBrick(15, 5, 3, 1);
	structBrick(28, 9, 4, 1);
	structBrick(68, 5, 1, 1);
	structBrick(69, 9, 4, 1);
	structBrick(81, 9, 5, 1);
	structBrick(92, 9, 4, 1);
	structBrick(125, 9, 4, 1);
	structBrick(161, 5, 1, 1);
	lMap[161][5]->setNumOfUse(7);
	structBrick(164, 9, 5, 1);
	structBrick(172, 9, 1, 1);
	structBrick(185, 5, 2, 1);

	lMap[69][9]->setSpawnStar(true);

	lMap[16][5]->setSpawnMushroom(true);
	lMap[28][9]->setSpawnMushroom(true);
	lMap[28][9]->setPowerUP(false);
	lMap[53][5]->setSpawnMushroom(true);
	lMap[125][9]->setSpawnMushroom(true);
	lMap[172][9]->setSpawnMushroom(true);

	struckBlockQ(53, 5, 5);
	struckBlockQ(53, 9, 5);
	struckBlockQ(79, 5, 4);
	struckBlockQ(85, 5, 3);
	struckBlockQ(170, 5, 1);

	struckBlockQ2(28, 5, 1);
	struckBlockQ2(186, 9, 1);

	structGND2(20,2, 5, true);
	structGND2(34, 2, 1, 4);
	structGND2(35, 2, 1, 2);
	structGND2(154, 2, 1, 3);
	structGND2(190, 2, 2, 10);

	structGND2(200, 2, 1, 1);
	structEnd(200, 3, 9);
	structCastleSmall(204, 2);

	// -- MAP 2-1-2

	this->iLevelType = 1;

	structGND(230, 0, 17, 2);

	structBrick(230, 2, 1, 11);
	structBrick(234, 2, 7, 3);
	structBrick(234, 12, 7, 1);

	structPipeVertical(245, 2, 10);
	structPipeHorizontal(243, 2, 1);

	structCoins(234, 5, 7, 1);
	structCoins(234, 7, 7, 1);
	structCoins(235, 9, 5, 1);

	// -- MAP 2-1-3

	this->iLevelType = 0;

	lMap[83][9]->setBlockID(128);

	structBonus(270, 1, 4);
	structBonus(275, 1, 57);

	lMap[274][0]->setBlockID(130); // VINE
	lMap[274][1]->setBlockID(83);

	structCoins(285, 8, 16, 1);
	structCoins(302, 10, 3, 1);
	structCoins(306, 9, 16, 1);
	structCoins(323, 10, 3, 1);
	structCoins(333, 2, 3, 1);

	structBonusEnd(332);

	vPlatform.push_back(new Platform(6, 4, 286*32 + 16, 335*32 + 16, CCFG::GAME_HEIGHT - 16 - 4*32, CCFG::GAME_HEIGHT - 16 - 4*32, 286*32.0f + 16, CCFG::GAME_HEIGHT - 16.0f - 4*32, true));

	// -- BONUS END

	this->iLevelType = 0;
}

void Map::loadLVL_2_2() {
	clearMap();

	this->iMapWidth = 320;
	this->iMapHeight = 25;
	this->iLevelType = 2;
	this->iMapTime = 400;

	// ---------- LOAD LISTS ----------

	createMap();

	loadMinionsLVL_2_2();
	loadPipeEventsLVL_2_2();

	this->underWater = true;

	structWater(0, 0, 200, 13);

	structGND(0, 0, 66, 2);
	structGND(71, 0, 60, 2);
	structGND(140, 0, 17, 2);
	structGND(164, 0, 38, 2);

	structGND(64, 2, 1, 3);
	structGND(65, 2, 1, 5);
	structGND(71, 2, 1, 5);
	structGND(72, 2, 1, 3);

	structGND(78, 2, 2, 3);
	structGND(78, 10, 2, 3);
	structGND(82, 9, 3, 1);
	
	structGND(129, 2, 1, 4);
	structGND(130, 2, 1, 2);
	structGND(141, 2, 1, 4);
	structGND(140, 2, 1, 2);

	structGND(131, 10, 1, 3);
	structGND(132, 10, 8, 1);

	structGND(18, 5, 3, 1);
	structGND(42, 5, 2, 1);
	structGND(102, 5, 2, 1);
	structGND(115, 6, 2, 1);

	structGND(156, 2, 1, 8);
	structGND(157, 9, 2, 1);
	structGND(164, 2, 1, 8);
	structGND(162, 9, 2, 1);

	structGND(172, 5, 5, 1);
	structGND(172, 9, 5, 1);
	structGND(180, 5, 4, 1);
	structGND(180, 9, 4, 1);

	structCoins(14, 2, 2, 1);
	structCoins(27, 9, 3, 1);
	structCoins(36, 2, 3, 1);
	structCoins(67, 4, 3, 1);
	structCoins(101, 3, 3, 1);
	structCoins(113, 8, 3, 1);
	structCoins(133, 3, 1, 1);
	structCoins(134, 2, 3, 1);
	structCoins(137, 3, 1, 1);
	structCoins(159, 5, 3, 1);
	structCoins(159, 2, 3, 1);

	structUW1(11, 2, 1, 3);
	structUW1(33, 2, 1, 5);
	structUW1(42, 6, 1, 2);
	structUW1(50, 2, 1, 4);
	structUW1(83, 10, 1, 2);
	structUW1(89, 2, 1, 3);
	structUW1(102, 6, 1, 4);
	structUW1(120, 2, 1, 4);
	structUW1(147, 2, 1, 2);
	structUW1(149, 2, 1, 3);
	structUW1(173, 10, 1, 2);

	structPipeHorizontal(189, 6, 1);

	structGND(185, 2, 17, 1);
	structGND(186, 3, 16, 1);
	structGND(187, 4, 15, 1);
	structGND(188, 5, 14, 1);
	structGND(190, 6, 12, 3);
	structGND(188, 9, 14, 4);

	// -- MAP 2-2-2

	this->iLevelType = 0;

	structGND(220, 0, 24, 2);

	structCastleSmall(220, 2);

	structCloud(223, 10, 2);
	structCloud(229, 6, 1);
	structCloud(238, 11, 2);

	structPipe(232, 2, 3);
	structPipeHorizontal(230, 2, 1);

	// -- MAP 2-2-3

	structCloud(275, 11, 2);
	structCloud(295, 10, 1);
	structCloud(306, 11, 1);

	structGrass(300, 2, 1);
	structGrass(310, 2, 1);

	structBush(287, 2, 2);
	structBush(303, 2, 1);

	structGND(271, 0, 42, 2);

	structPipe(274, 2, 1);

	structGND2(276, 2, 8, true);
	structGND2(284, 2, 1, 8);

	structGND2(293, 2, 1, 1);
	structEnd(293, 3, 9);

	structCastleSmall(297, 2);

	// -- END

	this->iLevelType = 2;
}

void Map::loadLVL_2_3() {
	clearMap();

	this->iMapWidth = 250;
	this->iMapHeight = 25;
	this->iLevelType = 0;
	this->iMapTime = 300;

	// ---------- LOAD LISTS ----------

	createMap();

	loadMinionsLVL_2_3();

	structGND(0, 0, 7, 2);
	structGND(207, 0, 43, 2);

	structCloud(3, 10, 2);
	structCloud(9, 6, 1);
	structCloud(18, 11, 2);
	structCloud(28, 2, 1);
	structCloud(35, 6, 1);
	structCloud(38, 7, 1);
	structCloud(46, 2, 1);
	structCloud(51, 10, 2);
	structCloud(57, 6, 1);
	structCloud(66, 11, 2);
	structCloud(76, 2, 1);
	structCloud(83, 6, 1);
	structCloud(86, 7, 1);
	structCloud(94, 2, 1);
	structCloud(99, 10, 2);
	structCloud(105, 6, 1);
	structCloud(114, 11, 2);
	structCloud(124, 2, 1);
	structCloud(131, 6, 1);
	structCloud(134, 7, 1);
	structCloud(142, 2, 1);
	structCloud(147, 10, 2);
	structCloud(153, 6, 1);
	structCloud(162, 11, 2);
	structCloud(172, 2, 1);
	structCloud(179, 6, 1);
	structCloud(182, 7, 1);
	structCloud(190, 2, 1);
	structCloud(195, 10, 2);
	structCloud(201, 6, 1);
	structCloud(210, 11, 2);
	structCloud(220, 2, 1);
	structCloud(227, 6, 1);
	structCloud(239, 9, 2);

	structT(8, 0, 8, 2);
	structGND2(10, 2, 3, true);
	structGND2(13, 2, 2, 3);

	structBridge2(15, 4, 16);
	structBridge2(32, 4, 15);
	structBridge2(48, 4, 15);
	structBridge2(69, 4, 10);
	structBridge2(85, 4, 10);
	structBridge2(100, 5, 5);

	structT(112, 0, 8, 2);

	structBridge2(122, 4, 3);
	structBridge2(128, 4, 15);
	structBridge2(147, 2, 8);
	structBridge2(160, 4, 8);

	structBridge2(171, 4, 2);
	structBridge2(175, 4, 2);
	structBridge2(179, 4, 2);

	structBridge2(184, 4, 9);

	structGND2(31, 0, 1, 5);
	structGND2(47, 0, 1, 5);
	structGND2(63, 0, 1, 5);
	structGND2(68, 0, 1, 5);
	structGND2(79, 0, 1, 5);
	structGND2(84, 0, 1, 5);
	structGND2(95, 0, 1, 5);
	structGND2(99, 0, 1, 6);
	structGND2(105, 0, 1, 6);
	structGND2(127, 0, 1, 5);
	structGND2(143, 0, 1, 5);
	structGND2(146, 0, 1, 3);
	structGND2(155, 0, 1, 3);
	structGND2(159, 0, 1, 5);
	structGND2(168, 0, 1, 5);
	structGND2(183, 0, 1, 5);

	structT(192, 0, 13, 2);

	structGND2(193, 2, 1, 3);
	structGND2(194, 2, 3, false);

	structGND2(208, 2, 8, true);
	structGND2(216, 2, 1, 8);

	structCoins(36, 9, 4, 1);
	structCoins(55, 9, 1, 1);
	structCoins(57, 9, 1, 1);
	structCoins(59, 9, 1, 1);
	structCoins(56, 8, 1, 1);
	structCoins(58, 8, 1, 1);
	structCoins(72, 9, 1, 1);
	structCoins(73, 10, 2, 1);
	structCoins(75, 9, 1, 1);
	structCoins(97, 9, 3, 1);
	structCoins(108, 9, 3, 1);
	structCoins(133, 9, 6, 1);
	structCoins(149, 6, 4, 1);
	structCoins(173, 8, 6, 1);

	struckBlockQ(102, 9, 1);
	lMap[102][9]->setSpawnMushroom(true);

	structCastleSmall(0, 2);

	structGND2(225, 2, 1, 1);
	structEnd(225, 3, 9);

	structCastleBig(228, 2);
	structCastleWall(237, 2, 13, 6);
	
	// -- END

	this->iLevelType = 0;
}

void Map::loadLVL_2_4() {
	clearMap();

	this->iMapWidth = 190;
	this->iMapHeight = 25;
	this->iLevelType = 3;
	this->iMapTime = 300;

	// ---------- LOAD LISTS ----------
	createMap();

	// ----- MINIONS
	loadMinionsLVL_2_4();

	vPlatform.push_back(new Platform(3, 1, 85*32 + 24, 85*32 + 16, -32, CCFG::GAME_HEIGHT + 32, 85*32 + 24, 9*32 + 16, true));
	vPlatform.push_back(new Platform(3, 1, 85*32 + 24, 85*32 + 16, -32, CCFG::GAME_HEIGHT + 32, 85*32 + 24, 1*32 + 16, true));

	vPlatform.push_back(new Platform(3, 0, 88*32 + 24, 88*32 + 16, -32, CCFG::GAME_HEIGHT + 32, 88*32 + 24, 10*32, true));
	vPlatform.push_back(new Platform(3, 0, 88*32 + 24, 88*32 + 16, -32, CCFG::GAME_HEIGHT + 32, 88*32 + 24, 2*32, true));

	vPlatform.push_back(new Platform(4, 3, 136*32 - 16, 138*32 + 16, CCFG::GAME_HEIGHT - 16 - 8*32, CCFG::GAME_HEIGHT - 16 - 8*32, 136*32 - 16, (float)CCFG::GAME_HEIGHT - 16 - 8*32, false));

	structGND(32, 0, 52, 2);
	structGND(92, 0, 17, 2);
	structGND(115, 0, 13, 2);
	structGND(141, 0, 40, 2);

	structGND(0, 0, 16, 5);
	structGND(0, 5, 5, 1);
	structGND(0, 6, 4, 1);
	structGND(0, 7, 3, 1);
	structGND(18, 5, 2, 1);
	structGND(22, 7, 3, 1);
	structGND(27, 5, 2, 1);

	structGND(32, 2, 2, 3);
	structGND(37, 5, 37, 1);
	structGND(80, 2, 4, 3);
	structGND(92, 2, 7, 3);
	structGND(108, 2, 1, 3);
	structGND(111, 0, 2, 5);
	structGND(115, 2, 5, 3);
	structGND(122, 2, 2, 3);
	structGND(126, 2, 2, 3);
	structGND(141, 2, 3, 4);

	structGND(0, 10, 16, 3);
	structGND(34, 9, 49, 4);
	structGND(93, 10, 6, 3);
	structGND(115, 10, 13, 3);
	structGND(128, 12, 42, 1);
	structGND(142, 9, 2, 3);

	lMap[23][7]->setBlockID(56);
	lMap[43][1]->setBlockID(56);
	lMap[49][5]->setBlockID(56);
	lMap[55][9]->setBlockID(56);
	lMap[55][1]->setBlockID(56);
	lMap[61][5]->setBlockID(56);
	lMap[67][1]->setBlockID(56);
	lMap[73][5]->setBlockID(56);
	lMap[82][8]->setBlockID(56);
	lMap[92][4]->setBlockID(56);
	lMap[103][3]->setBlockID(56);
	
	structCoins(102, 2, 3, 1);
	structCoins(102, 6, 3, 1);

	struckBlockQ(23, 11, 1);
	lMap[23][11]->setSpawnMushroom(true);

	structBridge(128, 4, 13);

	structBrick(128, 9, 6, 1);

	structLava(16, 0, 16, 2);
	structLava(109, 0, 2, 2);
	structLava(113, 0, 2, 2);
	structLava(128, 0, 13, 2);

	structPlatformLine(86);
	structPlatformLine(89);

	this->iLevelType = 3;
}

void Map::loadLVL_3_1() {
	clearMap();

	this->iMapWidth = 375;
	this->iMapHeight = 25;
	this->iLevelType = 4;
	this->iMapTime = 400;

	// ---------- LOAD LISTS ----------
	createMap();

	loadMinionsLVL_3_1();
	loadPipeEventsLVL_3_1();

	structGND(0, 0, 45, 2);
	structGND(48, 0, 29, 2);
	structGND(85, 0, 1, 2);
	structGND(88, 0, 40, 2);
	structGND(132, 0, 10, 2);
	structGND(144, 0, 33, 2);
	structGND(180, 0, 42, 2);

	structCloud(18, 10, 1);
	structCloud(27, 11, 1);
	structCloud(30, 10, 2);
	structCloud(45, 11, 1);
	structCloud(48, 10, 2);
	structCloud(66, 10, 1);
	structCloud(75, 11, 1);
	structCloud(78, 10, 2);
	structCloud(93, 11, 1);
	structCloud(96, 10, 2);
	structCloud(114, 10, 1);
	structCloud(123, 11, 1);
	structCloud(126, 10, 2);
	structCloud(141, 11, 1);
	structCloud(144, 10, 2);
	structCloud(162, 10, 1);
	structCloud(171, 11, 1);
	structCloud(174, 10, 2);
	structCloud(189, 11, 1);
	structCloud(192, 10, 2);
	structCloud(210, 10, 1);
	structCloud(219, 11, 1);

	structCastleBig(-2, 2);

	structTree(11, 2, 1, false);
	structTree(13, 2, 1, true);
	structTree(21, 2, 1, true);
	structTree(23, 2, 1, false);
	structTree(24, 2, 1, false);
	structTree(40, 2, 1, false);
	structTree(43, 2, 1, true);
	structTree(59, 2, 1, false);
	structTree(61, 2, 1, true);
	structTree(69, 2, 1, true);
	structTree(71, 2, 1, false);
	structTree(72, 2, 1, false);
	structTree(91, 2, 1, true);
	structTree(107, 2, 1, false);
	structTree(109, 2, 1, true);
	structTree(117, 2, 1, true);
	structTree(119, 2, 1, false);
	structTree(120, 2, 1, false);
	structTree(136, 2, 1, false);
	structTree(155, 2, 1, false);
	structTree(157, 2, 1, true);
	structTree(165, 2, 1, true);
	structTree(167, 2, 1, false);
	structTree(168, 2, 1, false);
	structTree(203, 2, 1, false);
	structTree(213, 2, 1, true);
	structTree(215, 2, 1, false);
	structTree(216, 2, 1, false);

	structFence(14, 2, 4);
	structFence(41, 2, 1);
	structFence(62, 2, 4);
	structFence(110, 2, 4);
	structFence(134, 2, 2);
	structFence(158, 2, 4);
	structFence(182, 2, 1);
	structFence(209, 2, 1);

	structGND2(73, 2, 4, true);
	structBridge2(77, 5, 8);
	structGND2(85, 2, 1, 4);
	structGND2(88, 2, 1, 4);
	structGND2(89, 2, 1, 2);

	structWater(77, 0, 8, 3);
	structWater(86, 0, 2, 3);

	structGND2(136, 2, 6, true);

	structGND2(174, 2, 1, 3);
	structGND2(175, 2, 1, 6);

	structGND2(183, 2, 8, true);
	structGND2(191, 2, 1, 8);

	structPipe(32, 2, 2);
	structPipe(38, 2, 3);
	structPipe(57, 2, 2);
	structPipe(67, 2, 1);
	structPipe(103, 2, 3);

	structBrick(26, 5, 3, 1);
	structBrick(61, 5, 1, 1);
	structBrick(90, 9, 3, 1);
	lMap[90][9]->setSpawnStar(true);
	structBrick(111, 5, 12, 1);
	structBrick(111, 9, 11, 1);
	structBrick(129, 6, 3, 1);
	structBrick(129, 9, 3, 1);
	structBrick(150, 5, 3, 1);
	structBrick(155, 5, 3, 1);
	structBrick(150, 9, 3, 1);
	structBrick(155, 9, 3, 1);
	structBrick(166, 5, 5, 1);
	lMap[167][5]->setNumOfUse(8);

	struckBlockQ(16, 5, 1);
	struckBlockQ(19, 6, 1);
	struckBlockQ(22, 6, 1);
	lMap[22][6]->setSpawnMushroom(true);
	struckBlockQ(113, 9, 1);
	struckBlockQ(117, 9, 1);
	lMap[117][9]->setSpawnMushroom(true);
	struckBlockQ(151, 5, 1);
	struckBlockQ(156, 5, 1);
	struckBlockQ(151, 9, 1);
	struckBlockQ(156, 9, 1);
	lMap[156][5]->setSpawnMushroom(true);

	struckBlockQ2(82, 9, 1);
	lMap[82][9]->setSpawnMushroom(true);
	lMap[82][9]->setPowerUP(false);

	structGND2(200, 2, 1, 1);
	structEnd(200, 3, 9);

	structCastleSmall(204, 2);

	// -- MAP 3-1-2

	this->iLevelType = 1;

	structGND(240, 0, 17, 2);

	structBrick(240, 2, 1, 11);

	structBrick(243, 6, 1, 2);
	structBrick(243, 8, 2, 1);
	structBrick(243, 9, 3, 1);

	structBrick(252, 6, 1, 2);
	structBrick(251, 8, 2, 1);
	structBrick(250, 9, 3, 1);

	structBrick(244, 5, 1, 1);
	structBrick(245, 6, 1, 1);
	structBrick(246, 7, 1, 1);

	structBrick(251, 5, 1, 1);
	structBrick(250, 6, 1, 1);
	structBrick(249, 7, 1, 1);

	structBrick(247, 8, 2, 2);

	structCoins(244, 6, 1, 1);
	structCoins(245, 7, 1, 1);
	structCoins(246, 8, 1, 1);

	structCoins(251, 6, 1, 1);
	structCoins(250, 7, 1, 1);
	structCoins(249, 8, 1, 1);

	structCoins(246, 10, 4, 1);
	structCoins(247, 11, 2, 1);

	lMap[245][9]->setSpawnMushroom(true);

	structPipeVertical(255, 2, 10);
	structPipeHorizontal(253, 2, 1);

	// -- MAP 3-1-3 BONUS

	this->iLevelType = 4;

	lMap[131][9]->setBlockID(128);

	structBonus(270, 1, 4);
	lMap[274][0]->setBlockID(131); // VINE
	lMap[274][1]->setBlockID(83);
	structBonus(275, 1, 78);

	structBonus(301, 6, 1);
	structBonus(320, 6, 1);
	structBonus(320, 7, 1);
	structBonus(331, 6, 1);
	structBonus(331, 7, 1);
	structBonus(337, 8, 2);
	structBonus(341, 8, 1);
	structBonus(343, 8, 1);
	structBonus(345, 8, 1);
	structBonus(347, 8, 1);
	structBonus(349, 8, 1);

	structCoins(284, 8, 16, 1);
	structCoins(303, 8, 16, 1);
	structCoins(322, 9, 8, 1);
	structCoins(341, 9, 10, 1);
	structCoins(355, 3, 3, 1);

	structBonusEnd(353);

	vPlatform.push_back(new Platform(6, 4, 286*32 + 16, 355*32 + 16, CCFG::GAME_HEIGHT - 16 - 4*32, CCFG::GAME_HEIGHT - 16 - 4*32, 286*32.0f + 16, CCFG::GAME_HEIGHT - 16.0f - 4*32, true));

	// -- END

	this->iLevelType = 4;
}

void Map::loadLVL_3_2() {
	clearMap();

	this->iMapWidth = 230;
	this->iMapHeight = 25;
	this->iLevelType = 4;
	this->iMapTime = 300;

	// ---------- LOAD LISTS ----------
	createMap();

	loadMinionsLVL_3_2();

	structGND(0, 0, 80, 2);
	structGND(82, 0, 41, 2);
	structGND(125, 0, 3, 2);
	structGND(130, 0, 100, 2);
	
	structCloud(0, 10, 2);
	structCloud(18, 10, 1);
	structCloud(27, 11, 1);
	structCloud(30, 10, 2);
	structCloud(45, 11, 1);
	structCloud(48, 10, 2);
	structCloud(66, 10, 1);
	structCloud(75, 11, 1);
	structCloud(78, 10, 2);
	structCloud(93, 11, 1);
	structCloud(96, 10, 2);
	structCloud(114, 10, 1);
	structCloud(123, 11, 1);
	structCloud(126, 10, 2);
	structCloud(141, 11, 1);
	structCloud(144, 10, 2);
	structCloud(162, 10, 1);
	structCloud(171, 11, 1);
	structCloud(174, 10, 2);
	structCloud(189, 11, 1);
	structCloud(192, 10, 2);
	structCloud(210, 10, 1);
	structCloud(219, 11, 1);
	structCloud(222, 10, 1);

	structFence(14, 2, 4);
	structFence(38, 2, 2);
	structFence(41, 2, 1);
	structFence(62, 2, 4);
	structFence(86, 2, 2);
	structFence(89, 2, 1);
	structFence(110, 2, 4);
	structFence(134, 2, 2);
	structFence(137, 2, 1);
	structFence(158, 2, 4);
	structFence(182, 2, 2);
	structFence(185, 2, 1);
	structFence(206, 2, 3);

	structTree(11, 2, 1, false);
	structTree(13, 2, 1, true);
	structTree(21, 2, 1, true);
	structTree(23, 2, 1, false);
	structTree(24, 2, 1, false);
	structTree(40, 2, 1, false);
	structTree(43, 2, 1, true);
	structTree(59, 2, 1, false);
	structTree(61, 2, 1, true);
	structTree(69, 2, 1, true);
	structTree(71, 2, 1, false);
	structTree(72, 2, 1, false);
	structTree(88, 2, 1, false);
	structTree(91, 2, 1, true);
	structTree(107, 2, 1, false);
	structTree(109, 2, 1, true);
	structTree(117, 2, 1, true);
	structTree(119, 2, 1, false);
	structTree(120, 2, 1, false);
	structTree(136, 2, 1, false);
	structTree(139, 2, 1, true);
	structTree(155, 2, 1, false);
	structTree(157, 2, 1, true);
	structTree(165, 2, 1, true);
	structTree(167, 2, 1, false);
	structTree(168, 2, 1, false);
	structTree(184, 2, 1, false);
	structTree(187, 2, 1, true);
	structTree(203, 2, 1, false);
	structTree(205, 2, 1, true);
	
	structGND2(49, 2, 1, 1);
	structGND2(60, 2, 1, 3);
	structGND2(75, 2, 1, 2);
	structGND2(79, 2, 1, 2);
	structGND2(126, 2, 1, 2);
	structGND2(192, 2, 8, true);
	structGND2(200, 2, 1, 8);

	structCoins(55, 5, 3, 1);
	structCoins(168, 8, 4, 1);
	
	struckBlockQ(60, 8, 1);
	lMap[60][8]->setSpawnMushroom(true);

	structBrick(77, 5, 1, 1);
	lMap[77][5]->setNumOfUse(8);
	structBrick(77, 9, 1, 1);
	lMap[77][9]->setSpawnStar(true);
	structBrick(126, 8, 1, 1);

	structPipe(169, 2, 2);

	structGND2(209, 2, 1, 1);
	structEnd(209, 3, 9);

	structCastleSmall(0, 2);
	structCastleSmall(213, 2);
}

void Map::loadLVL_3_3() {
	clearMap();

	this->iMapWidth = 180;
	this->iMapHeight = 25;
	this->iLevelType = 4;
	this->iMapTime = 300;

	// ---------- LOAD LISTS ----------
	createMap();

	loadMinionsLVL_3_3();

	structGND(0, 0, 16, 2);
	structGND(144, 0, 31, 2);

	structCloud(3, 10, 2);
	structCloud(9, 6, 1);
	structCloud(18, 11, 2);
	structCloud(28, 2, 1);
	structCloud(35, 6, 1);
	structCloud(38, 7, 1);
	structCloud(51, 10, 2);
	structCloud(57, 6, 1);
	structCloud(76, 2, 1);
	structCloud(83, 6, 1);
	structCloud(86, 7, 1);
	structCloud(94, 2, 1);
	structCloud(99, 10, 2);
	structCloud(105, 6, 1);
	structCloud(114, 11, 1);
	structCloud(131, 6, 1);
	structCloud(134, 7, 1);
	structCloud(142, 2, 1);
	structCloud(147, 10, 2);
	structCloud(153, 6, 1);
	structCloud(162, 11, 2);

	structT(18, 0, 5, 5);
	structT(22, 0, 6, 8);
	structT(30, 0, 3, 2);
	structT(36, 0, 7, 3);
	structT(43, 0, 4, 6);
	structT(47, 0, 6, 8);
	structT(46, 0, 10, 4);
	structT(55, 0, 4, 12);
	structT(65, 0, 3, 8);
	structT(69, 0, 3, 8);
	structT(73, 0, 3, 8);
	structT(77, 0, 3, 11);
	structT(65, 0, 16, 2);
	structT(84, 0, 4, 4);
	structT(97, 0, 3, 6);
	structT(104, 0, 4, 10);
	structT(108, 0, 3, 8);
	structT(107, 0, 5, 4);
	structT(116, 0, 3, 2);
	structT(119, 0, 12, 5);

	structCoins(31, 2, 2, 1);
	structCoins(37, 8, 1, 1);
	structCoins(42, 8, 3, 1);
	structCoins(52, 5, 3, 1);
	structCoins(56, 12, 2, 1);
	structCoins(66, 8, 1, 1);
	structCoins(70, 8, 1, 1);
	structCoins(74, 8, 1, 1);
	structCoins(78, 11, 1, 1);
	structCoins(81, 9, 1, 1);
	structCoins(88, 9, 1, 1);
	structCoins(90, 9, 1, 1);
	structCoins(105, 10, 2, 1);
	structCoins(108, 4, 1, 1);

	struckBlockQ(49, 11, 1);
	lMap[49][11]->setSpawnMushroom(true);

	structCastleSmall(0, 2);
	structCastleBig(154, 2);
	structCastleWall(163, 2, 10, 6);

	this->iLevelType = 0;

	structGND2(151, 2, 1, 1);
	structEnd(151, 3, 9);

	// -- SEESAW
	structSeeSaw(82, 12, 8);
	
	vPlatform.push_back(new Platform(6, 6, 81*32, 81*32, CCFG::GAME_HEIGHT - 16, CCFG::GAME_HEIGHT - 16 - 11*32, 81*32, (float)CCFG::GAME_HEIGHT - 16 - 8*32, true, 1));
	vPlatform.push_back(new Platform(6, 6, 88*32, 88*32, CCFG::GAME_HEIGHT - 16, CCFG::GAME_HEIGHT - 16 - 11*32, 88*32, (float)CCFG::GAME_HEIGHT - 16 - 6*32, true, 0));

	structSeeSaw(137, 12, 5);
	vPlatform.push_back(new Platform(6, 6, 136*32, 136*32, CCFG::GAME_HEIGHT - 16, CCFG::GAME_HEIGHT - 16 - 11*32, 136*32, (float)CCFG::GAME_HEIGHT - 16 - 9*32, true, 3));
	vPlatform.push_back(new Platform(6, 6, 140*32, 140*32, CCFG::GAME_HEIGHT - 16, CCFG::GAME_HEIGHT - 16 - 11*32, 140*32, (float)CCFG::GAME_HEIGHT - 16 - 6*32, true, 2));
	// -- SEESAW

	vPlatform.push_back(new Platform(6, 3, 27*32 - 16, 30*32 + 16, CCFG::GAME_HEIGHT - 16 - 10*32, CCFG::GAME_HEIGHT - 16 - 10*32, 27*32 - 16, (float)CCFG::GAME_HEIGHT - 16 - 10*32, false));
	vPlatform.push_back(new Platform(6, 3, 30*32 - 16, 33*32 + 16, CCFG::GAME_HEIGHT - 16 - 6*32, CCFG::GAME_HEIGHT - 16 - 6*32, 33*32 + 16, (float)CCFG::GAME_HEIGHT - 16 - 6*32, true));
	vPlatform.push_back(new Platform(6, 3, 91*32 - 32, 94*32, CCFG::GAME_HEIGHT - 16 - 5*32, CCFG::GAME_HEIGHT - 16 - 5*32, 94*32, (float)CCFG::GAME_HEIGHT - 16 - 5*32, true));
	vPlatform.push_back(new Platform(6, 3, 92*32, 96*32, CCFG::GAME_HEIGHT - 16 - 9*32, CCFG::GAME_HEIGHT - 16 - 9*32, 92*32, (float)CCFG::GAME_HEIGHT - 16 - 9*32, false));
	vPlatform.push_back(new Platform(6, 3, 100*32 - 16, 103*32 + 16, CCFG::GAME_HEIGHT - 16 - 3*32, CCFG::GAME_HEIGHT - 16 - 3*32, 103*32 + 16, (float)CCFG::GAME_HEIGHT - 16 - 3*32, true));
	vPlatform.push_back(new Platform(6, 3, 129*32 - 16, 132*32 + 16, CCFG::GAME_HEIGHT - 16 - 8*32, CCFG::GAME_HEIGHT - 16 - 8*32, 132*32 + 16, (float)CCFG::GAME_HEIGHT - 16 - 8*32, true));

	vPlatform.push_back(new Platform(6, 5, 60*32, 60*32, CCFG::GAME_HEIGHT - 16 - 8*32, CCFG::GAME_HEIGHT - 16 - 8*32, 60*32, (float)CCFG::GAME_HEIGHT - 16 - 8*32, true));

	this->iLevelType = 4;
}

void Map::loadLVL_3_4() {
	clearMap();

	this->iMapWidth = 225;
	this->iMapHeight = 25;
	this->iLevelType = 3;
	this->iMapTime = 300;

	// ---------- LOAD LISTS ----------
	createMap();

	loadMinionsLVL_3_4();

	vPlatform.push_back(new Platform(4, 3, 136*32 - 16, 138*32 + 16, CCFG::GAME_HEIGHT - 16 - 8*32, CCFG::GAME_HEIGHT - 16 - 8*32, 136*32 - 16, (float)CCFG::GAME_HEIGHT - 16 - 8*32, false));

	structGND(0, 0, 16, 5);
	structGND(0, 5, 5, 1);
	structGND(0, 6, 4, 1);
	structGND(0, 7, 3, 1);

	structGND(18, 0, 3, 5);
	structGND(23, 0, 3, 5);
	structGND(28, 0, 3, 5);
	structGND(33, 0, 2, 5);
	structGND(33, 0, 13, 2);
	structGND(53, 2, 3, 1);
	structGND(53, 10, 3, 2);
	structGND(63, 2, 3, 1);
	structGND(63, 10, 3, 2);
	structGND(79, 2, 3, 1);
	structGND(79, 10, 3, 2);
	structGND(90, 0, 6, 5);
	structGND(99, 0, 3, 5);
	structGND(105, 0, 3, 5);
	structGND(111, 10, 15, 2);
	structGND(111, 0, 5, 5);
	structGND(116, 0, 2, 2);
	structGND(118, 0, 10, 5);
	structGND(141, 0, 3, 6);
	structGND(142, 9, 2, 3);

	structGND(0, 10, 16, 3);
	structGND(16, 12, 149, 1);
	structGND(35, 10, 11, 2);
	structGND(90, 10, 6, 2);
	structGND(144, 0, 26, 2);

	structGND(48, 0, 40, 2);
	structLava(46, 0, 2, 2);
	structLava(88, 0, 2, 2);
	structLava(96, 0, 3, 2);
	structLava(102, 0, 3, 2);
	structLava(108, 0, 3, 2);

	structBrick(132, 9, 2, 3);

	structLava(128, 0, 13, 2);
	structBridge(128, 4, 13);

	lMap[19][3]->setBlockID(56);
	lMap[24][3]->setBlockID(56);
	lMap[29][3]->setBlockID(56);
	lMap[54][9]->setBlockID(56);
	lMap[54][3]->setBlockID(56);
	lMap[64][3]->setBlockID(56);
	lMap[64][9]->setBlockID(56);
	lMap[80][3]->setBlockID(56);
	lMap[80][9]->setBlockID(56);

	struckBlockQ(42, 5, 3);
	lMap[43][5]->setSpawnMushroom(true);

	structCoins(81, 8, 3, 1);
}

void Map::loadLVL_4_1() {
	clearMap();

	this->iMapWidth = 325;
	this->iMapHeight = 25;
	this->iLevelType = 0;
	this->iMapTime = 400;

	// ---------- LOAD LISTS ----------
	createMap();

	loadMinionsLVL_4_1();
	loadPipeEventsLVL_4_1();

	structGND(0, 0, 32, 2);
	structGND(34, 0, 44, 2);
	structGND(82, 0, 67, 2);
	structGND(151, 0, 23, 2);
	structGND(177, 0, 3, 2);
	structGND(182, 0, 8, 2);
	structGND(192, 0, 58, 2);

	structCloud(8, 10, 1);
	structCloud(19, 11, 1);
	structCloud(27, 10, 3);
	structCloud(36, 11, 2);
	structCloud(56, 10, 1);
	structCloud(67, 11, 1);
	structCloud(75, 10, 3);
	structCloud(85, 11, 2);
	structCloud(104, 10, 1);
	structCloud(115, 11, 1);
	structCloud(123, 10, 3);
	structCloud(132, 11, 2);
	structCloud(152, 10, 1);
	structCloud(163, 11, 1);
	structCloud(171, 10, 3);
	structCloud(180, 11, 2);
	structCloud(200, 10, 1);
	structCloud(211, 11, 1);
	structCloud(219, 10, 3);
	structCloud(228, 11, 2);
	
	structGrass(11, 2, 3);
	structGrass(23, 2, 1);
	structGrass(41, 2, 2);
	structGrass(59, 2, 3);
	structGrass(71, 2, 1);
	structGrass(89, 2, 2);
	structGrass(107, 2, 3);
	structGrass(119, 2, 1);
	structGrass(137, 2, 2);
	structGrass(155, 2, 3);
	structGrass(167, 2, 1);
	structGrass(185, 2, 2);
	structGrass(203, 2, 3);
	structGrass(215, 2, 1);
	structGrass(233, 2, 2);

	structBush(16, 2, 1);
	structBush(48, 2, 2);
	structBush(64, 2, 1);
	structBush(96, 2, 2);
	structBush(112, 2, 1);
	structBush(144, 2, 2);
	structBush(160, 2, 1);
	structBush(192, 2, 2);

	structPipe(21, 2, 2);
	structPipe(116, 2, 3);
	structPipe(132, 2, 3);
	structPipe(163, 2, 1);

	struckBlockQ(25, 5, 1);
	lMap[25][5]->setSpawnMushroom(true);
	struckBlockQ(25, 9, 1);

	struckBlockQ(64, 5, 1);
	struckBlockQ(64, 9, 1);
	struckBlockQ(66, 5, 1);
	struckBlockQ(66, 9, 1);

	struckBlockQ(90, 5, 4);
	
	struckBlockQ2(92, 9, 1);
	lMap[92][9]->setSpawnMushroom(true);
	lMap[92][9]->setPowerUP(false);

	struckBlockQ(146, 5, 3);
	struckBlockQ(151, 5, 3);
	struckBlockQ(148, 9, 4);
	lMap[148][5]->setSpawnMushroom(true);

	structBrick(149, 5, 2, 1);
	structBrick(220, 5, 1, 1);
	lMap[220][5]->setNumOfUse(6);

	structCoins(41, 5, 1, 1);
	structCoins(42, 6, 2, 1);
	structCoins(44, 5, 1, 1);
	structCoins(105, 8, 4, 1);
	structCoins(119, 8, 4, 1);
	structCoins(135, 8, 4, 1);

	structGND2(103, 2, 1, 3);
	structGND2(189, 2, 1, 3);
	structGND2(208, 2, 8, true);
	structGND2(216, 2, 1, 8);

	structCastleBig(-2, 2);
	structCastleSmall(229, 2);

	structGND2(225, 2, 1, 1);
	structEnd(225, 3, 9);
	
	// -- MAP 4-1-2

	this->iLevelType = 1;

	structGND(250, 0, 17, 2);

	structBrick(250, 2, 1, 11);
	structBrick(253, 3, 1, 3);
	structBrick(260, 3, 1, 3);
	structBrick(254, 5, 6, 1);
	structBrick(253, 12, 12, 1);

	structCoins(253, 2, 10, 1);
	structCoins(253, 6, 8, 1);

	structBrick(263, 5, 1, 1);
	lMap[263][5]->setSpawnMushroom(true);

	structPipeVertical(265, 2, 10);
	structPipeHorizontal(263, 2, 1);

	// -- END

	this->iLevelType = 0;
}

void Map::loadLVL_4_2() {
	clearMap();

	this->iMapWidth = 435;
	this->iMapHeight = 25;
	this->iLevelType = 1;
	this->iMapTime = 400;

	// ---------- LOAD LISTS ----------

	createMap();

	// ---------- LOAD LISTS ----------

	// ----- MINIONS
	loadMinionsLVL_4_2();

	// ----- PIPEEVENTS
	loadPipeEventsLVL_4_2();

	structGND(0, 0, 11, 2);
	structGND(13, 0, 2, 2);
	structGND(17, 0, 1, 2);
	structGND(21, 0, 36, 2);
	structGND(63, 0, 42, 2);
	structGND(107, 0, 2, 2);
	structGND(111, 0, 2, 2);
	structGND(119, 0, 4, 2);
	structGND(129, 0, 15, 2);
	structGND(146, 0, 9, 2);
	structGND(160, 0, 23, 2);
	structGND(185, 0, 49, 2);

	structBrick(0, 2, 1, 11);
	structBrick(6, 12, 14, 1);
	structBrick(22, 3, 5, 3);
	structBrick(27, 5, 2, 1);
	lMap[28][5]->setSpawnMushroom(true);
	structBrick(30, 2, 18, 4);
	structBrick(20, 9, 28, 4);
	structBrick(48, 12, 9, 1);
	lMap[43][10]->setNumOfUse(8);
	lMap[77][5]->setNumOfUse(6);
	lMap[81][5]->setSpawnStar(true);
	structBrick(65, 9, 2, 1);
	lMap[64][9]->setBlockID(129);
	structBrick(76, 5, 2, 1);
	structBrick(80, 5, 2, 1);
	structBrick(87, 6, 1, 1);
	structBrick(119, 5, 4, 1);
	structBrick(119, 9, 3, 1);
	structBrick(119, 12, 4, 1);
	structBrick(67, 12, 45, 1);
	structBrick(128, 12, 27, 1);
	structBrick(161, 5, 11, 1);
	lMap[161][5]->setSpawnMushroom(true);
	lMap[120][9]->setSpawnMushroom(true);
	structBrick(160, 7, 1, 6);
	structBrick(161, 9, 15, 4);
	structBrick(176, 12, 13, 1);
	structBrick(185, 2, 6, 3);
	structBrick(191, 2, 18, 11);
	structBrick(209, 12, 10, 1);
	structBrick(222, 2, 12, 18);
	
	struckBlockQ2(63, 6, 1);
	struckBlockQ2(64, 7, 1);
	struckBlockQ2(65, 6, 1);
	struckBlockQ2(66, 5, 1);

	struckBlockQ(50, 5, 2);
	struckBlockQ(54, 5, 3);
	struckBlockQ(50, 9, 2);
	lMap[55][5]->setSpawnMushroom(true);

	structGND2(103, 2, 1, 2);
	structGND2(104, 2, 1, 3);
	structGND2(111, 2, 2, 3);
	structGND2(152, 2, 3, true);
	structGND2(173, 2, 4, true);

	structPipe(72, 2, 2);
	structPipe(78, 2, 6);
	structPipe(84, 2, 2);
	structPipe(89, 2, 2);
	structPipe(107, 2, 3);
	structPipe(131, 2, 1);
	structPipe(138, 2, 2);
	structPipe(142, 2, 3);
	structPipe(180, 2, 6);
	structPipe(214, 2, 2);

	structPipeVertical(189, 5, 7);
	structPipeHorizontal(187, 5, 1);

	structCoins(27, 2, 3, 1);
	structCoins(162, 6, 10, 1);

	vPlatform.push_back(new Platform(6, 0, 58*32 + 16, 58*32 + 16, -32, CCFG::GAME_HEIGHT + 32, 58*32 + 16, 9*32 + 16, true));
	vPlatform.push_back(new Platform(6, 0, 58*32 + 16, 58*32 + 16, -32, CCFG::GAME_HEIGHT + 32, 58*32 + 16, 1*32 + 16, true));

	vPlatform.push_back(new Platform(6, 0, 114*32 + 16, 114*32 + 16, -32, CCFG::GAME_HEIGHT + 32, 114*32 + 16, 9*32 + 16, true));
	vPlatform.push_back(new Platform(6, 0, 114*32 + 16, 114*32 + 16, -32, CCFG::GAME_HEIGHT + 32, 114*32 + 16, 1*32 + 16, true));

	vPlatform.push_back(new Platform(6, 1, 124*32, 124*32, -32, CCFG::GAME_HEIGHT + 32, 124*32, 10*32, true));
	vPlatform.push_back(new Platform(6, 1, 124*32, 124*32, -32, CCFG::GAME_HEIGHT + 32, 124*32, 2*32, true));

	vPlatform.push_back(new Platform(6, 0, 156*32, 156*32, -32, CCFG::GAME_HEIGHT + 32, 156*32, 9*32 + 16, true));
	vPlatform.push_back(new Platform(6, 0, 156*32, 156*32, -32, CCFG::GAME_HEIGHT + 32, 156*32, 1*32 + 16, true));

	vLevelText.push_back(new LevelText(210*32, CCFG::GAME_HEIGHT - 16 - 8*32, "WELCOME TO WARP ZONEz"));

	vLevelText.push_back(new LevelText(214*32 + 16, CCFG::GAME_HEIGHT - 6*32, "5"));

	// -- MAP 4-2-2

	this->iLevelType = 0;

	structGND(240, 0, 24, 2);

	structCastleSmall(240, 2);

	structCloud(243, 10, 2);
	structCloud(249, 6, 1);
	structCloud(258, 11, 2);

	structPipe(252, 2, 3);
	structPipeHorizontal(250, 2, 1);

	for(int i = 13; i < 19; i++) {
		lMap[285][i]->setBlockID(83);
	}

	// -- MAP 4-2-3
	
	this->iLevelType = 1;

	structGND(270, 0, 17, 2);

	structBrick(269, 2, 1, 18);
	structBrick(270, 2, 1, 11);
	structBrick(274, 7, 1, 2);
	structBrick(275, 7, 5, 1);
	structBrick(280, 7, 1, 6);
	structBrick(281, 8, 2, 1);
	structBrick(274, 12, 6, 1);

	structBrick(284, 7, 1, 1);
	lMap[284][7]->setNumOfUse(8);

	structCoins(275, 8, 5, 2);

	structPipeVertical(285, 2, 10);
	structPipeHorizontal(283, 2, 1);

	// -- MAP 4-2-4

	this->iLevelType = 0;

	structCloud(308, 2, 1);
	structCloud(313, 10, 2);
	structCloud(319, 6, 1);
	structCloud(328, 11, 2);
	structCloud(338, 2, 1);
	structCloud(345, 6, 1);
	structCloud(348, 7, 1);
	structCloud(361, 10, 2);
	structCloud(367, 6, 1);

	structGND(310, 0, 4, 2);
	structGND(315, 0, 59, 2);

	structTMush(322, 2, 3, 4);
	structTMush(326, 2, 3, 8);
	structTMush(328, 2, 3, 2);
	structTMush(332, 2, 3, 8);
	structTMush(332, 2, 5, 2);
	structTMush(336, 2, 3, 6);
	structTMush(340, 2, 5, 9);
	structTMush(341, 2, 7, 3);

	structGND2(350, 2, 9, true);
	structGND2(359, 10, 11, 1);
	structGND2(372, 2, 2, 11);

	structCoins(322, 6, 3, 1);
	structCoins(326, 10, 3, 1);
	structCoins(332, 10, 3, 1);
	structCoins(336, 8, 3, 1);
	structCoins(340, 11, 5, 1);
	structCoins(345, 5, 2, 1);

	vLevelText.push_back(new LevelText(360*32, CCFG::GAME_HEIGHT - 16 - 8*32, "WELCOME TO WARP ZONEz"));

	vLevelText.push_back(new LevelText(360*32 + 16, CCFG::GAME_HEIGHT - 6*32, "8"));
	vLevelText.push_back(new LevelText(364*32 + 16, CCFG::GAME_HEIGHT - 6*32, "7"));
	vLevelText.push_back(new LevelText(368*32 + 16, CCFG::GAME_HEIGHT - 6*32, "6"));

	for(int i = 0; i < 19; i++) {
		lMap[374][i]->setBlockID(83);
	}

	this->iLevelType = 5;

	structPipe(360, 2, 2);
	structPipe(364, 2, 2);
	structPipe(368, 2, 2);

	lMap[314][0]->setBlockID(130);
	lMap[314][1]->setBlockID(83);

	// -- MAP 4-2-5

	this->iLevelType = 0;

	for(int i = 0; i < 19; i++) {
		lMap[390][i]->setBlockID(83);
	}

	structCloud(395, 11, 2);
	structCloud(415, 10, 1);
	structCloud(426, 11, 1);

	structGrass(420, 2, 1);
	structGrass(420, 2, 1);

	structBush(417, 2, 2);
	structBush(423, 2, 1);

	structGND(391, 0, 42, 2);

	structPipe(394, 2, 1);

	structGND2(396, 2, 8, true);
	structGND2(404, 2, 1, 8);

	structGND2(413, 2, 1, 1);
	structEnd(413, 3, 9);

	structCastleSmall(417, 2);

	// -- END

	this->iLevelType = 1;
}

void Map::loadLVL_4_3() {
	clearMap();

	this->iMapWidth = 170;
	this->iMapHeight = 25;
	this->iLevelType = 0;
	this->iMapTime = 300;

	// ---------- LOAD LISTS ----------

	createMap();

	// ----- MINIONS
	loadMinionsLVL_4_3();

	structGND(0, 0, 15, 2);
	structGND(141, 0, 28, 2);

	structCloud(3, 10, 2);
	structCloud(9, 6, 1);
	structCloud(18, 11, 2);
	structCloud(28, 2, 1);
	structCloud(35, 6, 1);
	structCloud(38, 7, 1);
	structCloud(46, 2, 1);
	structCloud(51, 10, 2);
	structCloud(57, 6, 1);
	structCloud(66, 11, 2);
	structCloud(76, 2, 1);
	structCloud(83, 6, 1);
	structCloud(86, 7, 1);
	structCloud(84, 2, 1);
	structCloud(99, 10, 2);
	structCloud(105, 6, 1);
	structCloud(114, 11, 2);
	structCloud(124, 2, 1);
	structCloud(131, 6, 1);
	structCloud(134, 7, 1);
	
	structTMush(16, 0, 5, 2);
	structTMush(19, 0, 5, 10);
	structTMush(23, 0, 7, 6);
	structTMush(32, 0, 3, 11);
	structTMush(39, 0, 5, 10);
	structTMush(36, 0, 7, 3);
	structTMush(44, 0, 3, 6);
	structTMush(51, 0, 3, 7);
	structTMush(67, 0, 3, 8);
	structTMush(65, 0, 5, 2);
	structTMush(70, 0, 3, 12);
	structTMush(72, 0, 3, 6);
	structTMush(74, 0, 5, 10);
	structTMush(84, 0, 3, 6);
	structTMush(99, 0, 3, 4);
	structTMush(105, 0, 3, 5);
	structTMush(113, 0, 5, 6);
	structTMush(117, 0, 3, 9);
	structTMush(121, 0, 7, 2);
	structTMush(130, 0, 5, 5);
	
	structCoins(20, 10, 3, 1);
	structCoins(24, 6, 5, 1);
	structCoins(38, 3, 3, 1);
	structCoins(48, 7, 1, 1);
	structCoins(57, 11, 1, 1);
	structCoins(67, 8, 3, 1);
	structCoins(73, 6, 1, 1);
	structCoins(70, 12, 3, 1);
	structCoins(96, 7, 1, 1);
	structCoins(104, 10, 1, 1);
	structCoins(113, 6, 5, 1);

	struckBlockQ(43, 12, 1);
	lMap[43][12]->setSpawnMushroom(true);

	// -- SEESAW
	structSeeSaw(49, 12, 8);
	vPlatform.push_back(new Platform(6, 6, 48*32, 48*32, CCFG::GAME_HEIGHT - 16, CCFG::GAME_HEIGHT - 16 - 11*32, 48*32, (float)CCFG::GAME_HEIGHT - 16 - 9*32, true, 1));
	vPlatform.push_back(new Platform(6, 6, 55*32, 55*32, CCFG::GAME_HEIGHT - 16, CCFG::GAME_HEIGHT - 16 - 11*32, 55*32, (float)CCFG::GAME_HEIGHT - 16 - 6*32, true, 0));

	structSeeSaw(81, 12, 9);
	vPlatform.push_back(new Platform(6, 6, 80*32, 80*32, CCFG::GAME_HEIGHT - 16, CCFG::GAME_HEIGHT - 16 - 11*32, 80*32, (float)CCFG::GAME_HEIGHT - 16 - 9*32, true, 3));
	vPlatform.push_back(new Platform(6, 6, 88*32, 88*32, CCFG::GAME_HEIGHT - 16, CCFG::GAME_HEIGHT - 16 - 11*32, 88*32, (float)CCFG::GAME_HEIGHT - 16 - 5*32, true, 2));

	structSeeSaw(92, 12, 6);
	vPlatform.push_back(new Platform(6, 6, 91*32, 91*32, CCFG::GAME_HEIGHT - 16, CCFG::GAME_HEIGHT - 16 - 11*32, 91*32, (float)CCFG::GAME_HEIGHT - 16 - 9*32, true, 5));
	vPlatform.push_back(new Platform(6, 6, 96*32, 96*32, CCFG::GAME_HEIGHT - 16, CCFG::GAME_HEIGHT - 16 - 11*32, 96*32, (float)CCFG::GAME_HEIGHT - 16 - 5*32, true, 4));
	
	structSeeSaw(103, 12, 7);
	vPlatform.push_back(new Platform(6, 6, 102*32, 102*32, CCFG::GAME_HEIGHT - 16, CCFG::GAME_HEIGHT - 16 - 11*32, 102*32, (float)CCFG::GAME_HEIGHT - 16 - 9*32, true, 7));
	vPlatform.push_back(new Platform(6, 6, 108*32, 108*32, CCFG::GAME_HEIGHT - 16, CCFG::GAME_HEIGHT - 16 - 11*32, 108*32, (float)CCFG::GAME_HEIGHT - 16 - 5*32, true, 6));
	// -- SEESAW

	vPlatform.push_back(new Platform(6, 2, 58*32, 58*32, CCFG::GAME_HEIGHT - 16 - 10*32, CCFG::GAME_HEIGHT - 64, 58*32, (float)CCFG::GAME_HEIGHT - 64, true));
	vPlatform.push_back(new Platform(6, 2, 62*32, 62*32, CCFG::GAME_HEIGHT - 12*32, CCFG::GAME_HEIGHT - 128, 62*32, (float)CCFG::GAME_HEIGHT - 12*32, false));
	vPlatform.push_back(new Platform(6, 2, 136*32, 136*32, CCFG::GAME_HEIGHT - 16 - 9*32, CCFG::GAME_HEIGHT - 48, 136*32, (float)CCFG::GAME_HEIGHT - 6*32, false));

	structCastleSmall(0, 2);
	
	structGND2(147, 2, 1, 1);
	structEnd(147, 3, 9);

	structCastleBig(150, 2);
	structCastleWall(159, 2, 11, 6);
}

void Map::loadLVL_4_4() {
	clearMap();

	this->iMapWidth = 200;
	this->iMapHeight = 25;
	this->iLevelType = 3;
	this->iMapTime = 300;

	// ---------- LOAD LISTS ----------

	createMap();

	// ----- MINIONS
	loadMinionsLVL_4_4();

	structGND(0, 12, 147, 1);
	structGND(0, 10, 6, 2);

	structGND(0, 0, 7, 5);
	structGND(0, 5, 5, 1);
	structGND(0, 6, 4, 1);
	structGND(0, 5, 3, 1);
	structGND(0, 7, 3, 1);
	
	structGND(9, 0, 2, 5);
	structGND(13, 0, 3, 5);

	structGND(16, 0, 72, 2);
	structGND(90, 4, 5, 1);
	structGND(91, 4, 4, 1);
	structGND(94, 5, 1, 2);

	structGND(93, 7, 1, 1);
	structGND(95, 7, 1, 2);

	structGND(95, 0, 65, 2);

	structGND(18, 5, 6, 4);
	structGND(25, 5, 1, 4);
	structGND(27, 5, 1, 4);
	structGND(29, 5, 1, 4);
	structGND(31, 5, 1, 4);
	structGND(33, 5, 1, 4);

	structGND(35, 5, 3, 4);

	structGND(38, 8, 6, 1);
	structGND(44, 5, 31, 4);

	structGND(77, 2, 3, 3);
	structGND(86, 4, 2, 1);
	structGND(89, 6, 2, 1);
	structGND(96, 8, 2, 1);
	structGND(96, 4, 48, 1);
	structGND(144, 5, 1, 5);
	structGND(145, 10, 2, 3);

	structGND(77, 9, 3, 3);
	structGND(90, 0, 1, 4);

	structGND(100, 8, 4, 1);
	structGND(105, 5, 1, 4);
	structGND(106, 8, 2, 1);

	structGND(110, 8, 34, 1);
	structGND(152, 2, 4, 3);
	structGND(158, 2, 2, 3);

	structLava(160, 0, 13, 2);
	structBridge(160, 4, 13);

	structGND(152, 12, 41, 1);

	structGND(173, 0, 20, 2);
	structGND(173, 2, 3, 4);
	structGND(174, 9, 2, 3);

	structLava(7, 0, 2, 2);
	structLava(11, 0, 2, 2);
	structLava(88, 0, 2, 2);
	structLava(91, 0, 4, 2);

	this->iLevelType = 4;

	structPipe(40, 2, 2);

	// -- END

	this->iLevelType  = 3;
}

void Map::loadLVL_5_1() {
	clearMap();

	this->iMapWidth = 300;
	this->iMapHeight = 25;
	this->iLevelType = 0;
	this->iMapTime = 300;

	// ---------- LOAD LISTS ----------

	createMap();

	loadMinionsLVL_5_1();
	loadPipeEventsLVL_5_1();

	structGND(0, 0, 49, 2);
	structGND(51, 0, 41, 2);
	structGND(96, 0, 18, 2);
	structGND(116, 0, 36, 2);
	structGND(155, 0, 70, 2);

	structCloud(0, 10, 2);
	structCloud(18, 10, 1);
	structCloud(27, 11, 1);
	structCloud(30, 10, 2);
	structCloud(45, 11, 1);
	structCloud(48, 10, 2);
	structCloud(66, 10, 1);
	structCloud(75, 11, 1);
	structCloud(78, 10, 2);
	structCloud(96, 10, 2);
	structCloud(114, 10, 1);
	structCloud(123, 11, 1);
	structCloud(126, 10, 2);
	structCloud(141, 11, 1);
	structCloud(144, 10, 2);
	structCloud(162, 10, 1);
	structCloud(171, 11, 1);
	structCloud(174, 10, 2);
	structCloud(189, 11, 1);
	structCloud(192, 10, 2);
	structCloud(210, 10, 1);
	structCloud(219, 11, 1);
	structCloud(222, 10, 1);
	
	structFence(14, 2, 4);
	structFence(38, 2, 2);
	structFence(41, 2, 1);
	structFence(62, 2, 4);
	structFence(86, 2, 2);
	structFence(110, 2, 1);
	structFence(112, 2, 2);
	structFence(134, 2, 2);
	structFence(137, 2, 1);
	structFence(158, 2, 1);
	structFence(160, 2, 2);
	structFence(208, 2, 2);
	
	structGND2(90, 5, 4, 1);
	structGND2(89, 2, 1, 4);
	structGND2(116, 2, 1, 3);
	structGND2(147, 2, 1, 4);
	structGND2(156, 5, 2, 1);
	structGND2(182, 2, 5, true);
	structGND2(189, 4, 2, 6);
	
	this->iLevelType = 4;

	structTree(11, 2, 1, false);
	structTree(13, 2, 1, true);
	structTree(21, 2, 1, true);
	structTree(23, 2, 1, false);
	structTree(24, 2, 1, false);
	structTree(40, 2, 1, false);
	structTree(43, 2, 1, true);
	structTree(59, 2, 1, false);
	structTree(61, 2, 1, true);
	structTree(69, 2, 1, true);
	structTree(71, 2, 1, false);
	structTree(72, 2, 1, false);
	structTree(88, 2, 1, false);
	structTree(91, 2, 1, true);
	structTree(107, 2, 1, false);
	structTree(109, 2, 1, false);
	structTree(117, 2, 1, true);
	structTree(119, 2, 1, false);
	structTree(120, 2, 1, false);
	structTree(136, 2, 1, false);
	structTree(139, 2, 1, true);
	structTree(155, 2, 1, false);
	structTree(157, 2, 1, true);
	structTree(165, 2, 1, true);
	structTree(167, 2, 1, false);
	structTree(168, 2, 1, false);
	structTree(187, 2, 1, true);
	structTree(213, 2, 1, true);
	structTree(215, 2, 1, false);
	structTree(216, 2, 1, false);

	structPipe(44, 2, 2);
	structPipe(51, 2, 2);
	structPipe(156, 6, 1);
	structPipe(163, 2, 1);

	structGND2(199, 2, 1, 1);
	structEnd(199, 3, 9);

	this->iLevelType = 0;

	structBulletBill(111, 2, 0);
	structBulletBill(159, 2, 0);
	structBulletBill(170, 2, 0);

	structBrick(90, 9, 3, 1);
	lMap[91][9]->setSpawnStar(true);

	structBrick(148, 5, 3, 1);
	lMap[148][5]->setSpawnMushroom(true);

	structCastleSmall(0, 2);
	structCastleSmall(203, 2);

	// -- MAP 5-1-2

	this->iLevelType = 1;

	structGND(228, 0, 17, 2);

	structBrick(227, 2, 1, 18);
	structBrick(228, 2, 1, 11);
	structBrick(232, 7, 1, 2);
	structBrick(233, 7, 5, 1);
	structBrick(238, 7, 1, 6);
	structBrick(239, 8, 2, 1);
	structBrick(232, 12, 6, 1);

	structBrick(242, 7, 1, 1);
	lMap[242][7]->setNumOfUse(8);

	structCoins(233, 8, 5, 2);

	structPipeVertical(243, 2, 10);
	structPipeHorizontal(241, 2, 1);

	// -- END

	this->iLevelType = 0;
}

void Map::loadLVL_5_2() {
	clearMap();

	this->iMapWidth = 415;
	this->iMapHeight = 25;
	this->iLevelType = 0;
	this->iMapTime = 400;

	// ---------- LOAD LISTS ----------

	createMap();

	loadMinionsLVL_5_2();
	loadPipeEventsLVL_5_2();

	for(int i = 0; i < 16; i++) {
		lMap[79][i]->setBlockID(83);
	}
	
	structGND(74, 0, 33, 2);
	structGND(109, 0, 37, 2);
	structGND(148, 0, 24, 2);
	structGND(176, 0, 31, 2);
	structGND(209, 0, 25, 2);
	structGND(231, 0, 19, 2);
	structGND(253, 0, 2, 2);
	structGND(257, 0, 8, 2);
	structGND(266, 0, 2, 2);
	structGND(269, 0, 35, 2);

	structCastleSmall(80, 2);
	structCastleSmall(284, 2);

	structGrass(75, 2, 2);

	structCloud(75, 11, 2);
	structCloud(80, 10, 2);
	structCloud(98, 10, 1);
	structCloud(107, 11, 1);
	structCloud(110, 10, 2);
	structCloud(125, 11, 1);
	structCloud(128, 10, 1);
	structCloud(146, 10, 1);
	structCloud(155, 11, 1);
	structCloud(158, 10, 2);
	structCloud(173, 11, 1);
	structCloud(176, 10, 2);
	structCloud(194, 10, 1);
	structCloud(203, 11, 1);
	structCloud(206, 10, 2);
	structCloud(221, 11, 1);
	structCloud(224, 10, 2);
	structCloud(242, 10, 1);
	structCloud(251, 11, 1);
	structCloud(254, 10, 2);
	structCloud(269, 11, 1);
	structCloud(272, 10, 2);
	structCloud(290, 10, 1);
	structCloud(299, 11, 1);

	addSpring(106*32, 336);

	structCoins(101, 10, 3, 1);
	structCoins(109, 6, 3, 1);
	structCoins(166, 10, 2, 1);
	structCoins(252, 10, 2, 1);

	structBulletBill(97, 6, 0);
	structBulletBill(187, 2, 0);

	this->iLevelType = 4;

	structGND2(280, 2, 1, 1);
	structEnd(280, 3, 9);

	structTree(91, 2, 1, false);
	structTree(93, 2, 1, true);
	structTree(102, 2, 1, true);
	structTree(104, 2, 1, false);
	structTree(105, 2, 1, false);
	structTree(120, 2, 1, false);
	structTree(123, 2, 1, true);
	structTree(139, 2, 1, false);
	structTree(141, 2, 1, true);
	structTree(151, 2, 1, false);
	structTree(152, 2, 1, false);
	structTree(168, 2, 1, false);
	structTree(171, 2, 1, true);
	structTree(189, 2, 1, true);
	structTree(197, 2, 1, true);
	structTree(199, 2, 1, false);
	structTree(200, 2, 1, false);
	structTree(216, 2, 1, false);
	structTree(219, 2, 1, true);
	structTree(235, 2, 1, false);
	structTree(237, 2, 1, true);
	structTree(245, 2, 1, true);
	structTree(247, 2, 1, false);
	structTree(248, 2, 1, false);
	structTree(283, 2, 1, false);
	structTree(293, 2, 1, true);
	structTree(295, 2, 1, false);
	structTree(296, 2, 1, false);

	structFence(96, 2, 3);
	structFence(118, 2, 2);
	structFence(121, 2, 1);
	structFence(166, 2, 2);
	structFence(190, 2, 4);
	structFence(214, 2, 2);
	structFence(217, 2, 1);
	structFence(238, 2, 4);
	structFence(262, 2, 1);
	structFence(289, 2, 1);

	structGND2(92, 2, 4, true);
	structGND2(96, 5, 3, 1);
	structGND2(124, 2, 4, true);
	structGND2(128, 2, 1, 4);
	structGND2(129, 2, 1, 2);
	structGND2(142, 2, 4, true);
	structGND2(148, 2, 1, 5);
	structGND2(149, 2, 1, 6);
	structGND2(150, 2, 1, 6);
	structGND2(209, 2, 1, 3);

	structGND2(263, 2, 2, true);
	structGND2(266, 2, 1, 4);
	structGND2(267, 2, 1, 5);

	structGND2(269, 2, 1, 7);
	structGND2(270, 2, 2, 8);

	structBrick(109, 5, 6, 1);
	structBrick(109, 9, 6, 1);
	lMap[114][9]->setSpawnMushroom(true);

	structBrick(166, 9, 2, 1);
	structBrick(169, 6, 3, 1);

	structBrick(198, 5, 8, 1);
	structBrick(198, 9, 8, 1);
	lMap[205][9]->setSpawnStar(true);

	structBrick(221, 3, 2, 1);
	lMap[221][3]->setNumOfUse(6);
	lMap[222][3]->setSpawnMushroom(true);

	structBrick(227, 5, 3, 1);
	structBrick(233, 9, 5, 1);
	structBrick(248, 5, 1, 1);
	lMap[248][5]->setSpawnMushroom(true);
	structBrick(252, 9, 4, 1);

	struckBlockQ(158, 5, 5);
	struckBlockQ2(164, 5, 1);

	structPipe(135, 2, 2);
	structPipe(195, 2, 1);
	structPipe(253, 2, 1);

	// -- MAP 5-2-2

	this->iLevelType = 2;

	structWater(0, 0, 65, 13);

	structPipeHorizontal(62, 6, 1);

	structGND(0, 0, 22, 2);
	structGND(26, 0, 2, 5);
	structGND(32, 0, 2, 5);
	structGND(34, 0, 4, 2);
	structGND(40, 0, 2, 2);
	structGND(44, 0, 30, 2);

	structGND(11, 8, 5, 1);
	structGND(26, 10, 2, 3);
	structGND(32, 10, 2, 3);
	structGND(38, 10, 6, 1);
	structGND(50, 5, 4, 1);
	structGND(54, 8, 4, 1);

	structGND(59, 2, 15, 1);
	structGND(60, 3, 14, 1);
	structGND(61, 4, 13, 2);
	structGND(63, 6, 11, 3);
	structGND(61, 9, 13, 4);

	structUW1(12, 2, 1, 3);
	structUW1(15, 9, 1, 2);
	structUW1(20, 2, 1, 4);
	structUW1(38, 6, 1, 4);
	structUW1(43, 6, 1, 4);
	structUW1(46, 2, 1, 2);

	structCoins(22, 7, 10, 1);
	structCoins(40, 2, 2, 1);
	structCoins(50, 6, 4, 1);
	structCoins(54, 9, 4, 1);

	vPlatform.push_back(new Platform(6, 2, 22*32, 22*32, CCFG::GAME_HEIGHT - 12*32, CCFG::GAME_HEIGHT - 16 - 3*32, 22*32 + 16, (float)CCFG::GAME_HEIGHT - 16 - 4*32, true));
	vPlatform.push_back(new Platform(6, 2, 28*32, 28*32, CCFG::GAME_HEIGHT - 12*32, CCFG::GAME_HEIGHT - 16 - 3*32, 28*32 + 16, (float)CCFG::GAME_HEIGHT - 12*32, false));

	this->iLevelType = 0;

	// -- MAP 5-2-3 BONUS

	this->iLevelType = 0;

	lMap[165][9]->setBlockID(128);

	structBonus(325, 1, 4);
	structBonus(330, 1, 57);

	lMap[329][0]->setBlockID(130); // VINE
	lMap[329][1]->setBlockID(83);

	structCoins(340, 8, 16, 1);
	structCoins(357, 10, 3, 1);
	structCoins(361, 9, 16, 1);
	structCoins(378, 10, 3, 1);
	structCoins(394, 2, 3, 1);

	structBonusEnd(387);

	vPlatform.push_back(new Platform(6, 4, 341*32 + 16, 390*32 + 16, CCFG::GAME_HEIGHT - 16 - 4*32, CCFG::GAME_HEIGHT - 16 - 4*32, 341*32.0f + 16, CCFG::GAME_HEIGHT - 16.0f - 4*32, true));
}

void Map::loadLVL_5_3() {
	clearMap();

	this->iMapWidth = 270;
	this->iMapHeight = 25;
	this->iLevelType = 0;
	this->iMapTime = 300;

	// ---------- LOAD LISTS ----------

	createMap();

	loadMinionsLVL_5_3();

	// ---------- LOAD LISTS ----------

	vPlatform.push_back(new Platform(4, 2, 55*32, 55*32, CCFG::GAME_HEIGHT - 16 - 9*32, CCFG::GAME_HEIGHT - 32, 55*32, (float)CCFG::GAME_HEIGHT - 32, true));

	vPlatform.push_back(new Platform(4, 3, 83*32 - 16, 86*32 + 16, CCFG::GAME_HEIGHT - 16 - 6*32, CCFG::GAME_HEIGHT - 16 - 6*32, 83*32 - 16, (float)CCFG::GAME_HEIGHT - 16 - 6*32, true));
	
	vPlatform.push_back(new Platform(4, 3, 91*32 - 16, 93*32 + 16, CCFG::GAME_HEIGHT - 16 - 5*32, CCFG::GAME_HEIGHT - 16 - 5*32, 93*32 + 16, (float)CCFG::GAME_HEIGHT - 16 - 5*32, false));

	vPlatform.push_back(new Platform(4, 3, 127*32 - 16, 131*32 + 16, CCFG::GAME_HEIGHT - 16 - 8*32, CCFG::GAME_HEIGHT - 16 - 8*32, 127*32 - 16, (float)CCFG::GAME_HEIGHT - 16 - 8*32, false));

	// ----- CLOUDS

	structCloud(3, 10, 2);
	structCloud(9, 6, 1);
	structCloud(11, 18, 2);
	structCloud(18, 11, 2);
	structCloud(35, 6, 1);
	structCloud(38, 7, 1);
	structCloud(46, 2, 1);
	structCloud(51, 10, 2);
	structCloud(57, 6, 1);
	structCloud(66, 11, 2);
	structCloud(76, 2, 1);
	structCloud(83, 6, 1);
	structCloud(86, 7, 1);
	structCloud(94, 2, 1);
	structCloud(99, 10, 2);
	structCloud(131, 6, 1);
	structCloud(134, 7, 1);
	structCloud(142, 2, 1);
	structCloud(147, 10, 2);
	structCloud(153, 6, 1);
	structCloud(162, 11, 2);

	// ----- GND

	structGND(0, 0, 16, 2);
	structGND(129, 0, 47, 2);
	// ----- T

	structT(18, 0, 4, 3);
	structT(24, 0, 8, 6);
	structT(26, 6, 5, 4);
	structT(32, 0, 3, 3);
	structT(35, 0, 5, 7);
	structT(40, 0, 7, 11);
	structT(50, 0, 4, 2);
	structT(59, 0, 5, 2);
	structT(60, 2, 4, 8);
	structT(50, 0, 4, 2);
	structT(65, 0, 5, 2);
	structT(70, 0, 3, 6);
	structT(76, 0, 6, 9);
	structT(98, 0, 4, 4);
	structT(104, 0, 8, 8);
	structT(113, 0, 3, 2);
	structT(116, 0, 4, 6);
	structT(122, 0, 4, 6);

	// ----- GND2

	struckBlockQ(59, 4, 1);
	lMap[59][4]->setSpawnMushroom(true);

	structGND2(138, 2, 2, 4);
	structGND2(140, 2, 2, 6);
	structGND2(142, 2, 2, 8);

	// ----- COINS
	
	structCoins(27, 10, 3, 1);
	structCoins(33, 3, 1, 1);
	structCoins(37, 12, 2, 1);
	structCoins(50, 8, 2, 1);
	structCoins(60, 10, 4, 1);
	structCoins(85, 9, 2, 1);
	structCoins(93, 10, 2, 1);
	structCoins(97, 10, 2, 1);
	structCoins(113, 2, 3, 1);
	structCoins(120, 9, 2, 1);

	// ----- END

	structGND2(152, 2, 1, 1);
	structEnd(152, 3, 9);

	// ----- CASTLE

	structCastleSmall(0, 2);
	structCastleBig(155, 2);
	structCastleWall(164, 2, 12, 6);

	this->iLevelType = 0;
}

void Map::loadLVL_5_4() {
	clearMap();

	this->iMapWidth = 190;
	this->iMapHeight = 25;
	this->iLevelType = 3;
	this->iMapTime = 300;

	// ---------- LOAD LISTS ----------
	createMap();

	// ----- MINIONS
	loadMinionsLVL_5_4();

	vPlatform.push_back(new Platform(3, 1, 85*32 + 24, 85*32 + 16, -32, CCFG::GAME_HEIGHT + 32, 85*32 + 24, 9*32 + 16, true));
	vPlatform.push_back(new Platform(3, 1, 85*32 + 24, 85*32 + 16, -32, CCFG::GAME_HEIGHT + 32, 85*32 + 24, 1*32 + 16, true));

	vPlatform.push_back(new Platform(3, 0, 88*32 + 24, 88*32 + 16, -32, CCFG::GAME_HEIGHT + 32, 88*32 + 24, 10*32, true));
	vPlatform.push_back(new Platform(3, 0, 88*32 + 24, 88*32 + 16, -32, CCFG::GAME_HEIGHT + 32, 88*32 + 24, 2*32, true));

	vPlatform.push_back(new Platform(4, 3, 136*32 - 16, 138*32 + 16, CCFG::GAME_HEIGHT - 16 - 8*32, CCFG::GAME_HEIGHT - 16 - 8*32, 136*32 - 16, (float)CCFG::GAME_HEIGHT - 16 - 8*32, false));

	structGND(32, 0, 52, 2);
	structGND(92, 0, 17, 2);
	structGND(115, 0, 13, 2);
	structGND(141, 0, 40, 2);

	structGND(0, 0, 16, 5);
	structGND(0, 5, 5, 1);
	structGND(0, 6, 4, 1);
	structGND(0, 7, 3, 1);
	structGND(18, 5, 2, 1);
	structGND(22, 7, 3, 1);
	structGND(27, 5, 2, 1);

	structGND(32, 2, 2, 3);
	structGND(37, 5, 37, 1);
	structGND(80, 2, 4, 3);
	structGND(92, 2, 7, 3);
	structGND(108, 2, 1, 3);
	structGND(111, 0, 2, 5);
	structGND(115, 2, 5, 3);
	structGND(122, 2, 2, 3);
	structGND(126, 2, 2, 3);
	structGND(141, 2, 3, 4);

	structGND(0, 10, 16, 3);
	structGND(34, 9, 49, 4);
	structGND(93, 10, 6, 3);
	structGND(115, 10, 13, 3);
	structGND(128, 12, 42, 1);
	structGND(142, 9, 2, 3);

	lMap[23][7]->setBlockID(56);
	lMap[43][1]->setBlockID(56);
	lMap[49][5]->setBlockID(56);
	lMap[55][9]->setBlockID(56);
	lMap[55][1]->setBlockID(56);
	lMap[61][5]->setBlockID(56);
	lMap[67][1]->setBlockID(56);
	lMap[73][5]->setBlockID(56);
	lMap[82][8]->setBlockID(56);
	lMap[92][4]->setBlockID(56);
	lMap[103][3]->setBlockID(56);
	
	structCoins(102, 2, 3, 1);
	structCoins(102, 6, 3, 1);

	struckBlockQ(23, 11, 1);
	lMap[23][11]->setSpawnMushroom(true);

	structBridge(128, 4, 13);

	structBrick(128, 9, 6, 1);

	structLava(16, 0, 16, 2);
	structLava(109, 0, 2, 2);
	structLava(113, 0, 2, 2);
	structLava(128, 0, 13, 2);

	structPlatformLine(86);
	structPlatformLine(89);

	this->iLevelType = 3;
}

void Map::loadLVL_6_1() {
	clearMap();

	this->iMapWidth = 210;
	this->iMapHeight = 25;
	this->iLevelType = 4;
	this->iMapTime = 400;

	// ---------- LOAD LISTS ----------
	createMap();

	loadMinionsLVL_6_1();

	structGND(0, 0, 20, 2);
	structGND(22, 0, 9, 2);
	structGND(37, 0, 2, 2);
	structGND(41, 0, 16, 2);
	structGND(59, 0, 15, 2);
	structGND(77, 0, 16, 2);
	structGND(96, 0, 31, 2);
	structGND(134, 0, 2, 2);
	structGND(137, 0, 12, 2);
	structGND(155, 0, 9, 2);
	structGND(167, 0, 7, 2);
	structGND(176, 0, 34, 2);

	structCloud(8, 10, 1);
	structCloud(19, 11, 1);
	structCloud(27, 10, 3);
	structCloud(36, 11, 2);
	structCloud(56, 10, 1);
	structCloud(67, 11, 1);
	structCloud(75, 10, 3);
	structCloud(81, 11, 2);
	structCloud(104, 10, 1);
	structCloud(115, 11, 1);
	structCloud(123, 10, 3);
	structCloud(132, 11, 2);
	structCloud(152, 10, 1);
	structCloud(163, 11, 1);
	structCloud(171, 10, 3);
	structCloud(180, 11, 2);
	structCloud(200, 10, 1);

	structCastleBig(-2, 2);

	structBush(16, 2, 1);
	structBush(48, 2, 2);
	structBush(64, 2, 1);
	structBush(96, 2, 2);
	structBush(112, 2, 1);
	structBush(160, 2, 1);
	
	structGrass(11, 2, 3);
	structGrass(23, 2, 1);
	structGrass(41, 2, 2);
	structGrass(59, 2, 3);
	structGrass(89, 2, 2);
	structGrass(107, 2, 3);
	structGrass(119, 2, 1);
	structGrass(137, 2, 2);
	structGrass(155, 2, 3);
	structGrass(167, 2, 1);
	structGrass(185, 2, 2);
	structGrass(203, 2, 3);
	
	structCastleSmall(192, 2);

	structGND2(26, 2, 6, 1);
	structGND2(29, 3, 5, 1);
	structGND2(32, 4, 4, 1);
	structGND2(35, 5, 3, 1);
	structGND2(69, 2, 4, true);
	structGND2(73, 2, 1, 4);
	structGND2(84, 3, 2, true);
	structGND2(87, 2, 1, 5);
	structGND2(88, 2, 1, 6);
	structGND2(89, 2, 1, 7);
	structGND2(122, 2, 5, true);
	structGND2(143, 2, 6, true);
	structGND2(169, 2, 5, true);
	structGND2(176, 2, 2, 8);
	
	struckBlockQ(16, 5, 2);

	structBrick(36, 9, 2, 1);
	lMap[36][9]->setSpawnMushroom(true);
	structBrick(41, 5, 3, 1);
	lMap[43][5]->setNumOfUse(7);
	structBrick(90, 8, 3, 1);
	structBrick(92, 4, 3, 1);

	structBrick(127, 6, 2, 1);
	struckBlockQ(130, 6, 2);
	lMap[130][6]->setSpawnMushroom(true);
	structBrick(130, 2, 5, 1);

	structBrick(149, 7, 2, 1);
	structBrick(151, 5, 2, 1);
	lMap[152][5]->setNumOfUse(8);
	structBrick(152, 3, 2, 1);

	struckBlockQ2(90, 6, 1);
	lMap[90][6]->setSpawnMushroom(true);
	lMap[90][6]->setPowerUP(false);
	struckBlockQ2(113, 5, 1);
	struckBlockQ2(113, 9, 1);

	structCoins(62, 5, 3, 1);
	structCoins(76, 7, 2, 1);
	structCoins(105, 6, 3, 1);
	
	this->iLevelType = 0;

	structPipe(102, 2, 2);

	structGND2(186, 2, 1, 1);
	structEnd(186, 3, 9);

	this->iLevelType = 4;
}

void Map::loadLVL_6_2() {
	clearMap();

	this->iMapWidth = 495;
	this->iMapHeight = 25;
	this->iLevelType = 0;
	this->iMapTime = 400;

	// ---------- LOAD LISTS ----------

	createMap();

	loadMinionsLVL_6_2();
	loadPipeEventsLVL_6_2();

	for(int i = 0; i < 16; i++) {
		lMap[84][i]->setBlockID(83);
	}

	structGND(77, 0, 131, 2);
	structGND(214, 0, 12, 2);
	structGND(227, 0, 1, 2);
	structGND(229, 0, 8, 2);
	structGND(238, 0, 82, 2);

	structCloud(82, 11, 1);
	structCloud(93, 10, 1);
	structCloud(104, 11, 1);
	structCloud(112, 10, 3);
	structCloud(121, 11, 2);
	structCloud(141, 10, 1);
	structCloud(152, 11, 1);
	structCloud(160, 10, 3);
	structCloud(169, 11, 2);
	structCloud(189, 10, 1);
	structCloud(200, 11, 1);
	structCloud(208, 10, 3);
	structCloud(217, 11, 2);
	structCloud(237, 10, 1);
	structCloud(248, 11, 1);
	structCloud(256, 10, 3);
	structCloud(265, 11, 2);
	structCloud(285, 10, 1);
	structCloud(296, 11, 1);
	structCloud(304, 10, 3);
	structCloud(313, 11, 2);

	structGrass(80, 2, 2);
	structGrass(96, 2, 3);
	structGrass(108, 2, 1);
	structGrass(126, 2, 2);
	structGrass(144, 2, 3);
	structGrass(156, 2, 1);
	structGrass(174, 2, 2);
	structGrass(192, 2, 3);
	structGrass(204, 2, 1);
	structGrass(222, 2, 2);
	structGrass(240, 2, 3);
	structGrass(252, 2, 1);
	structGrass(270, 2, 2);
	structGrass(300, 2, 1);

	structBush(101, 2, 1);
	structBush(133, 2, 2);
	structBush(149, 2, 1);
	structBush(181, 2, 2);
	structBush(197, 2, 1);
	structBush(229, 2, 2);
	structBush(245, 2, 1);
	structBush(277, 2, 2);
	structBush(293, 2, 1);

	structGND2(117, 6, 2, 1);
	structGND2(147, 5, 2, 1);
	structGND2(196, 5, 2, 1);
	structGND2(234, 2, 3, true);
	structGND2(241, 2, 1, 4);
	structGND2(242, 2, 1, 2);
	structGND2(252, 5, 2, 1);
	structGND2(284, 2, 8, true);
	structGND2(292, 2, 1, 8);

	structBrick(95, 5, 3, 1);
	structBrick(108, 9, 3, 1);
	structBrick(136, 5, 2, 1);
	lMap[137][5]->setSpawnMushroom(true);
	structBrick(152, 9, 5, 1);
	structBrick(162, 9, 4, 1);
	structBrick(195, 5, 1, 1);
	structBrick(198, 5, 1, 1);
	structBrick(200, 9, 1, 1);
	structBrick(204, 9, 9, 1);
	structBrick(223, 6, 2, 1);
	structBrick(225, 9, 2, 1);
	lMap[225][9]->setSpawnStar(true);
	structBrick(229, 5, 3, 1);
	structBrick(230, 9, 2, 1);
	structBrick(245, 5, 3, 1);
	structBrick(245, 9, 3, 1);

	lMap[109][9]->setNumOfUse(8);

	structCastleSmall(85, 2);
	structCastleSmall(305, 2);

	struckBlockQ2(109, 5, 1);
	struckBlockQ2(167, 5, 1);

	this->iLevelType = 0;

	structPipe(104, 2, 3);
	structPipe(113, 2, 3);
	structPipe(117, 7, 1);
	structPipe(120, 2, 1);
	structPipe(122, 2, 1);
	structPipe(131, 2, 3);
	structPipe(141, 2, 4);
	structPipe(147, 6, 1);
	structPipe(152, 2, 1);
	structPipe(159, 2, 1);
	structPipe(165, 2, 2);
	structPipe(169, 2, 1);
	structPipe(172, 2, 5);
	structPipe(179, 2, 2);
	structPipe(187, 2, 3);
	structPipe(190, 2, 1);
	structPipe(196, 6, 2);
	structPipe(200, 2, 1);
	structPipe(216, 2, 1);
	structPipe(220, 2, 1);
	structPipe(238, 2, 2);
	structPipe(252, 6, 2);
	structPipe(259, 2, 3);
	structPipe(264, 2, 1);
	structPipe(266, 2, 2);
	structPipe(268, 2, 3);
	structPipe(274, 2, 2);
	structPipe(286, 2, 3);

	structGND2(301, 2, 1, 1);
	structEnd(301, 3, 9);

	// -- MAP 6-2-2

	this->iLevelType = 2;

	structWater(0, 0, 65, 13);

	structPipeHorizontal(62, 6, 1);

	structGND(0, 0, 22, 2);
	structGND(26, 0, 2, 5);
	structGND(32, 0, 2, 5);
	structGND(34, 0, 4, 2);
	structGND(40, 0, 2, 2);
	structGND(44, 0, 30, 2);

	structGND(11, 8, 5, 1);
	structGND(26, 10, 2, 3);
	structGND(32, 10, 2, 3);
	structGND(38, 10, 6, 1);
	structGND(50, 5, 4, 1);
	structGND(54, 8, 4, 1);

	structGND(59, 2, 15, 1);
	structGND(60, 3, 14, 1);
	structGND(61, 4, 13, 2);
	structGND(63, 6, 11, 3);
	structGND(61, 9, 13, 4);

	structUW1(12, 2, 1, 3);
	structUW1(15, 9, 1, 2);
	structUW1(20, 2, 1, 4);
	structUW1(38, 6, 1, 4);
	structUW1(43, 6, 1, 4);
	structUW1(46, 2, 1, 2);

	structCoins(22, 7, 10, 1);
	structCoins(40, 2, 2, 1);
	structCoins(50, 6, 4, 1);
	structCoins(54, 9, 4, 1);

	vPlatform.push_back(new Platform(4, 2, 22*32, 22*32, CCFG::GAME_HEIGHT - 12*32, CCFG::GAME_HEIGHT - 16 - 3*32, 22*32 + 16, (float)CCFG::GAME_HEIGHT - 16 - 4*32, true));
	vPlatform.push_back(new Platform(4, 2, 28*32, 28*32, CCFG::GAME_HEIGHT - 12*32, CCFG::GAME_HEIGHT - 16 - 3*32, 28*32 + 16, (float)CCFG::GAME_HEIGHT - 12*32, false));

	// -- MAP 6-2-3

	this->iLevelType = 1;
	
	structGND(330, 0, 17, 2);

	structBrick(329, 2, 1, 18);
	structBrick(330, 2, 1, 11);
	structBrick(334, 7, 1, 2);
	structBrick(335, 7, 5, 1);
	structBrick(340, 7, 1, 6);
	structBrick(341, 8, 2, 1);
	structBrick(334, 12, 6, 1);

	structBrick(344, 7, 1, 1);
	lMap[344][7]->setNumOfUse(8);

	structCoins(335, 8, 5, 2);

	structPipeVertical(345, 2, 10);
	structPipeHorizontal(343, 2, 1);

	// -- MAP 6-2-4

	this->iLevelType = 1;

	structGND(360, 0, 17, 2);

	structBrick(360, 2, 1, 11);
	structBrick(363, 3, 1, 3);
	structBrick(370, 3, 1, 3);
	structBrick(364, 5, 6, 1);
	structBrick(363, 12, 12, 1);

	structCoins(363, 2, 10, 1);
	structCoins(363, 6, 8, 1);

	structBrick(373, 5, 1, 1);
	lMap[373][5]->setSpawnMushroom(true);

	structPipeVertical(375, 2, 10);
	structPipeHorizontal(373, 2, 1);

	// -- MAP 6-2-5 BONUS

	this->iLevelType = 4;

	lMap[166][9]->setBlockID(128);

	for(int i = 0; i < 16; i++) {
		lMap[389][i]->setBlockID(83);
	}

	structBonus(390, 1, 4);
	lMap[394][0]->setBlockID(130); // VINE
	lMap[394][1]->setBlockID(83);
	structBonus(395, 1, 78);

	structBonus(421, 6, 1);
	structBonus(440, 6, 1);
	structBonus(440, 7, 1);
	structBonus(451, 6, 1);
	structBonus(451, 7, 1);
	structBonus(457, 8, 2);
	structBonus(461, 8, 1);
	structBonus(463, 8, 1);
	structBonus(465, 8, 1);
	structBonus(467, 8, 1);
	structBonus(469, 8, 1);

	structCoins(404, 8, 16, 1);
	structCoins(423, 8, 16, 1);
	structCoins(442, 9, 8, 1);
	structCoins(461, 9, 10, 1);
	structCoins(475, 3, 3, 1);

	structBonusEnd(473);

	vPlatform.push_back(new Platform(6, 4, 406*32 + 16, 475*32 + 16, CCFG::GAME_HEIGHT - 16 - 4*32, CCFG::GAME_HEIGHT - 16 - 4*32, 406*32.0f + 16, CCFG::GAME_HEIGHT - 16.0f - 4*32, true));

	// -- END

	this->iLevelType = 4;
}

void Map::loadLVL_6_3() {
	clearMap();

	this->iMapWidth = 200;
	this->iMapHeight = 25;
	this->iLevelType = 6;
	this->iMapTime = 300;

	// ---------- LOAD LISTS ----------

	createMap();

	loadMinionsLVL_6_3();

	structGND(0, 0, 16, 2);
	structGND(160, 0, 30, 2);

	this->iLevelType = 3;

	structCloud(3, 10, 2);
	structCloud(18, 11, 2);
	structCloud(9, 6, 1);
	structCloud(28, 2, 1);
	structCloud(35, 6, 1);
	structCloud(38, 7, 1);
	structCloud(46, 2, 1);
	structCloud(51, 10, 2);
	structCloud(57, 6, 1);
	structCloud(66, 11, 2);
	structCloud(76, 2, 1);
	structCloud(83, 6, 1);
	structCloud(86, 7, 1);
	structCloud(94, 2, 1);
	structCloud(99, 10, 2);
	structCloud(105, 6, 1);
	structCloud(114, 11, 2);
	structCloud(124, 2, 1);
	structCloud(131, 6, 1);
	structCloud(134, 7, 1);
	structCloud(142, 2, 1);
	structCloud(147, 10, 2);
	structCloud(153, 6, 1);
	
	structT(18, 0, 3, 2);
	structT(21, 0, 3, 6);
	structT(24, 0, 3, 2);
	structT(31, 0, 4, 6);
	structT(37, 0, 3, 2);
	structT(43, 0, 3, 2);
	structT(49, 0, 4, 4);
	structT(65, 0, 5, 6);
	structT(85, 0, 3, 12);
	structT(85, 0, 5, 5);
	structT(90, 0, 3, 7);
	structT(93, 0, 3, 2);
	structT(97, 0, 4, 2);
	structT(103, 0, 3, 2);
	structT(107, 0, 5, 6);
	structT(113, 0, 5, 2);
	structT(123, 0, 3, 2);
	structT(132, 0, 3, 10);
	structT(132, 0, 4, 2);
	structT(135, 0, 4, 6);
	structT(156, 0, 3, 10);

	structCastleBig(170, 2);
	structCastleWall(179, 2, 11, 6);

	structCastleSmall(0, 2);

	// -- SEESAW
	structSeeSaw(71, 12, 5);
	
	vPlatform.push_back(new Platform(4, 6, 70*32 + 16, 70*32 + 16, CCFG::GAME_HEIGHT - 16, CCFG::GAME_HEIGHT - 16 - 11*32, 70*32 + 16, (float)CCFG::GAME_HEIGHT - 16 - 9*32, true, 1));
	vPlatform.push_back(new Platform(4, 6, 74*32 + 16, 74*32 + 16, CCFG::GAME_HEIGHT - 16, CCFG::GAME_HEIGHT - 16 - 11*32, 74*32 + 16, (float)CCFG::GAME_HEIGHT - 16 - 5*32, true, 0));

	structSeeSaw(79, 12, 4);
	
	vPlatform.push_back(new Platform(4, 6, 78*32 + 16, 78*32 + 16, CCFG::GAME_HEIGHT - 16, CCFG::GAME_HEIGHT - 16 - 11*32, 78*32 + 16, (float)CCFG::GAME_HEIGHT - 16 - 9*32, true, 3));
	vPlatform.push_back(new Platform(4, 6, 81*32 + 16, 81*32 + 16, CCFG::GAME_HEIGHT - 16, CCFG::GAME_HEIGHT - 16 - 11*32, 81*32 + 16, (float)CCFG::GAME_HEIGHT - 16 - 5*32, true, 2));

	structSeeSaw(127, 12, 4);
	
	vPlatform.push_back(new Platform(4, 6, 126*32 + 16, 126*32 + 16, CCFG::GAME_HEIGHT - 16, CCFG::GAME_HEIGHT - 16 - 11*32, 126*32 + 16, (float)CCFG::GAME_HEIGHT - 16 - 9*32, true, 5));
	vPlatform.push_back(new Platform(4, 6, 129*32 + 16, 129*32 + 16, CCFG::GAME_HEIGHT - 16, CCFG::GAME_HEIGHT - 16 - 11*32, 129*32 + 16, (float)CCFG::GAME_HEIGHT - 16 - 5*32, true, 4));

	// -- SEESAW

	vPlatform.push_back(new Platform(4, 2, 28*32, 28*32, CCFG::GAME_HEIGHT - 11*32, CCFG::GAME_HEIGHT - 16 - 2*32, 28*32, (float)CCFG::GAME_HEIGHT - 12*32, false));

	vPlatform.push_back(new Platform(4, 3, 39*32 + 16, 45*32 + 16, CCFG::GAME_HEIGHT - 9*32, CCFG::GAME_HEIGHT - 16 - 9*32, 39*32 + 16, (float)CCFG::GAME_HEIGHT - 9*32, false));
	vPlatform.push_back(new Platform(4, 3, 45*32 + 16, 49*32 + 16, CCFG::GAME_HEIGHT - 7*32, CCFG::GAME_HEIGHT - 16 - 7*32, 45*32 + 16, (float)CCFG::GAME_HEIGHT - 7*32, true));
	vPlatform.push_back(new Platform(4, 3, 51*32, 56*32, CCFG::GAME_HEIGHT - 8*32, CCFG::GAME_HEIGHT - 16 - 8*32, 51*32 + 16, (float)CCFG::GAME_HEIGHT - 8*32, false));
	
	vPlatform.push_back(new Platform(4, 2, 60*32, 60*32, CCFG::GAME_HEIGHT - 11*32, CCFG::GAME_HEIGHT - 16 - 2*32, 60*32, (float)CCFG::GAME_HEIGHT - 12*32, false));
	vPlatform.push_back(new Platform(4, 2, 121*32 + 16, 121*32 + 16, CCFG::GAME_HEIGHT - 11*32, CCFG::GAME_HEIGHT - 16 - 2*32, 121*32 + 16, (float)CCFG::GAME_HEIGHT - 12*32, false));

	vPlatform.push_back(new Platform(4, 5, 141*32, 141*32, CCFG::GAME_HEIGHT - 16 - 7*32, CCFG::GAME_HEIGHT - 16 - 7*32, 141*32, (float)CCFG::GAME_HEIGHT - 16 - 7*32, true));
	vPlatform.push_back(new Platform(4, 5, 145*32, 145*32, CCFG::GAME_HEIGHT - 16 - 8*32, CCFG::GAME_HEIGHT - 16 - 8*32, 145*32, (float)CCFG::GAME_HEIGHT - 16 - 8*32, true));
	vPlatform.push_back(new Platform(4, 5, 149*32, 149*32, CCFG::GAME_HEIGHT - 16 - 6*32, CCFG::GAME_HEIGHT - 16 - 6*32, 149*32, (float)CCFG::GAME_HEIGHT - 16 - 6*32, true));
	vPlatform.push_back(new Platform(4, 5, 153*32, 153*32, CCFG::GAME_HEIGHT - 16 - 7*32, CCFG::GAME_HEIGHT - 16 - 7*32, 153*32, (float)CCFG::GAME_HEIGHT - 16 - 7*32, true));

	addSpring(38*32, 336);
	addSpring(116*32, 336);


	structGND2(167, 2, 1, 1);

	this->iLevelType = 4;

	structEnd(167, 3, 9);

	structCoins(28, 12, 2, 1);
	structCoins(43, 10, 7, 1);
	structCoins(73, 9, 2, 1);
	structCoins(85, 12, 3, 1);
	structCoins(100, 6, 4, 1);
	structCoins(128, 9, 2, 1);
	structCoins(145, 7, 2, 1);
	structCoins(154, 11, 2, 1);

	struckBlockQ(55, 11, 1);
	lMap[55][11]->setSpawnMushroom(true);

	// -- END

	this->iLevelType = 4;
}

void Map::loadLVL_6_4() {
	clearMap();

	this->iMapWidth = 180;
	this->iMapHeight = 25;
	this->iLevelType = 3;
	this->iMapTime = 300;

	// ---------- LOAD LISTS ----------

	createMap();

	loadMinionsLVL_6_4();

	vPlatform.push_back(new Platform(4, 3, 136*32 - 16, 138*32 + 16, CCFG::GAME_HEIGHT - 16 - 8*32, CCFG::GAME_HEIGHT - 16 - 8*32, 136*32 - 16, (float)CCFG::GAME_HEIGHT - 16 - 8*32, false));

	structGND(0, 0, 3, 8);
	structGND(3, 0, 1, 7);
	structGND(4, 0, 1, 6);
	structGND(5, 0, 8, 5);
	structGND(15, 0, 11, 5);
	structGND(29, 0, 3, 5);
	structGND(35, 0, 37, 6);
	structGND(72, 0, 32, 5);
	structGND(104, 0, 24, 2);
	structGND(116, 2, 4, 3);
	structGND(123, 2, 5, 3);
	structGND(141, 0, 3, 6);
	structGND(144, 0, 19, 2);

	structGND(0, 10, 24, 3);
	structGND(23, 9, 1, 4);
	structGND(24, 12, 13, 1);
	structGND(37, 9, 35, 4);
	structGND(72, 12, 91, 1);
	structGND(80, 11, 1, 1);
	structGND(88, 11, 1, 1);
	structGND(97, 10, 7, 2);
	structGND(123, 10, 5, 2);
	structGND(142, 9, 2, 3);

	structBridge(128, 4, 13);

	structLava(13, 0, 2, 3);
	structLava(26, 0, 3, 2);
	structLava(32, 0, 3, 2);
	structLava(128, 0, 13, 2);

	lMap[23][8]->setBlockID(56);
	lMap[37][8]->setBlockID(56);
	lMap[80][10]->setBlockID(56);
	lMap[92][5]->setBlockID(56);

	lMap[30][4]->setBlockID(56);
	lMap[49][8]->setBlockID(56);
	lMap[60][8]->setBlockID(56);
	lMap[67][8]->setBlockID(56);
	lMap[76][5]->setBlockID(56);
	lMap[84][5]->setBlockID(56);
	lMap[88][10]->setBlockID(56);
	
	struckBlockQ(30, 8, 1);
	lMap[30][8]->setSpawnMushroom(true);

	struckBlockQ2(106, 5, 1);
	struckBlockQ2(109, 5, 1);
	struckBlockQ2(112, 5, 1);
	struckBlockQ2(107, 9, 1);
	struckBlockQ2(110, 9, 1);
	struckBlockQ2(113, 9, 1);

	this->iLevelType = 3;
}

void Map::loadLVL_7_1() {
	clearMap();

	this->iMapWidth = 300;
	this->iMapHeight = 25;
	this->iLevelType = 0;
	this->iMapTime = 400;

	// ---------- LOAD LISTS ----------

	createMap();

	loadMinionsLVL_7_1();
	loadPipeEventsLVL_7_1();

	structBulletBill(19, 2, 0);
	structBulletBill(28, 1, 0);
	structBulletBill(28, 3, 0);
	structBulletBill(36, 2, 0);
	structBulletBill(46, 2, 1);
	structBulletBill(56, 1, 0);
	structBulletBill(56, 3, 0);
	structBulletBill(64, 6, 0);
	structBulletBill(68, 2, 0);
	structBulletBill(104, 2, 0);
	structBulletBill(122, 2, 0);
	structBulletBill(146, 1, 0);
	structBulletBill(146, 3, 0);

	structGND(0, 0, 73, 2);
	structGND(75, 0, 77, 2);
	structGND(153, 0, 46, 2);

	structGND2(64, 5, 1, 1);

	structCloud(18, 10, 1);
	structCloud(27, 11, 1);
	structCloud(30, 10, 2);
	structCloud(45, 11, 1);
	structCloud(48, 10, 2);
	structCloud(66, 10, 1);
	structCloud(75, 11, 1);
	structCloud(78, 10, 2);
	structCloud(93, 11, 1);
	structCloud(96, 10, 2);
	structCloud(114, 10, 1);
	structCloud(123, 11, 1);
	structCloud(126, 10, 2);
	structCloud(141, 11, 1);
	structCloud(144, 10, 2);
	structCloud(162, 10, 1);
	structCloud(171, 11, 1);
	structCloud(174, 10, 2);
	structCloud(189, 11, 1);
	structCloud(192, 10, 2);

	structCastleBig(-2, 2);

	structFence(14, 2, 4);
	structFence(38, 2, 2);
	structFence(41, 2, 1);
	structFence(62, 2, 4);
	structFence(86, 2, 2);
	structFence(89, 2, 1);
	structFence(111, 2, 3);
	structFence(134, 2, 2);
	structFence(137, 2, 1);
	structFence(159, 2, 3);
	structFence(182, 2, 1);

	this->iLevelType = 4;

	structTree(11, 2, 1, false);
	structTree(13, 2, 1, true);
	structTree(21, 2, 1, true);
	structTree(23, 2, 1, false);
	structTree(24, 2, 1, false);
	structTree(40, 2, 1, false);
	structTree(43, 2, 1, true);
	structTree(59, 2, 1, false);
	structTree(61, 2, 1, true);
	structTree(69, 2, 1, true);
	structTree(71, 2, 1, false);
	structTree(72, 2, 1, false);
	structTree(88, 2, 1, false);
	structTree(91, 2, 1, true);
	structTree(107, 2, 1, false);
	structTree(117, 2, 1, true);
	structTree(119, 2, 1, false);
	structTree(120, 2, 1, false);
	structTree(136, 2, 1, false);
	structTree(139, 2, 1, true);

	structPipe(76, 2, 2);
	structPipe(93, 2, 2);
	structPipe(109, 2, 2);
	structPipe(115, 2, 1);
	structPipe(128, 2, 1);

	structGND2(179, 2, 1, 1);
	structEnd(179, 3, 9);

	this->iLevelType = 0;

	structBrick(27, 9, 3, 1);
	lMap[27][9]->setSpawnMushroom(true);
	structBrick(62, 5, 2, 1);
	structBrick(65, 5, 2, 1);
	lMap[65][5]->setNumOfUse(8);
	structBrick(82, 5, 7, 1);
	structBrick(82, 9, 7, 1);

	structBrick(134, 5, 5, 1);
	structBrick(134, 9, 5, 1);

	structBrick(149, 6, 2, 1);
	structBrick(153, 8, 2, 1);
	structBrick(151, 12, 1, 1);
	lMap[151][12]->setSpawnMushroom(true);

	struckBlockQ2(93, 9, 1);
	lMap[93][9]->setSpawnMushroom(true);
	lMap[93][9]->setPowerUP(false);

	struckBlockQ(39, 5, 4);

	structGND2(141, 2, 1, 3);
	structGND2(153, 2, 6, true);
	structGND2(162, 2, 8, true);
	structGND2(170, 2, 1, 8);

	addSpring(151*32, 336);

	structCastleSmall(183, 2);

	// -- MAP 7-1-2

	this->iLevelType = 1;

	structGND(210, 0, 17, 2);

	structBrick(210, 2, 1, 11);
	structBrick(214, 2, 7, 3);
	structBrick(214, 12, 7, 1);

	structPipeVertical(225, 2, 10);
	structPipeHorizontal(223, 2, 1);

	structCoins(214, 5, 7, 1);
	structCoins(214, 7, 7, 1);
	structCoins(215, 9, 5, 1);

	// -- END

	this->iLevelType = 0;
}

void Map::loadLVL_7_2() {
	clearMap();

	this->iMapWidth = 320;
	this->iMapHeight = 25;
	this->iLevelType = 2;
	this->iMapTime = 400;

	// ---------- LOAD LISTS ----------

	createMap();

	loadMinionsLVL_7_2();
	loadPipeEventsLVL_7_2();

	this->underWater = true;

	structWater(0, 0, 200, 13);

	structGND(0, 0, 66, 2);
	structGND(71, 0, 60, 2);
	structGND(140, 0, 17, 2);
	structGND(164, 0, 38, 2);

	structGND(64, 2, 1, 3);
	structGND(65, 2, 1, 5);
	structGND(71, 2, 1, 5);
	structGND(72, 2, 1, 3);

	structGND(78, 2, 2, 3);
	structGND(78, 10, 2, 3);
	structGND(82, 9, 3, 1);
	
	structGND(129, 2, 1, 4);
	structGND(130, 2, 1, 2);
	structGND(141, 2, 1, 4);
	structGND(140, 2, 1, 2);

	structGND(131, 10, 1, 3);
	structGND(132, 10, 8, 1);

	structGND(18, 5, 3, 1);
	structGND(42, 5, 2, 1);
	structGND(102, 5, 2, 1);
	structGND(115, 6, 2, 1);

	structGND(156, 2, 1, 8);
	structGND(157, 9, 2, 1);
	structGND(164, 2, 1, 8);
	structGND(162, 9, 2, 1);

	structGND(172, 5, 5, 1);
	structGND(172, 9, 5, 1);
	structGND(180, 5, 4, 1);
	structGND(180, 9, 4, 1);

	structCoins(14, 2, 2, 1);
	structCoins(27, 9, 3, 1);
	structCoins(36, 2, 3, 1);
	structCoins(67, 4, 3, 1);
	structCoins(101, 3, 3, 1);
	structCoins(113, 8, 3, 1);
	structCoins(133, 3, 1, 1);
	structCoins(134, 2, 3, 1);
	structCoins(137, 3, 1, 1);
	structCoins(159, 5, 3, 1);
	structCoins(159, 2, 3, 1);

	structUW1(11, 2, 1, 3);
	structUW1(33, 2, 1, 5);
	structUW1(42, 6, 1, 2);
	structUW1(50, 2, 1, 4);
	structUW1(83, 10, 1, 2);
	structUW1(89, 2, 1, 3);
	structUW1(102, 6, 1, 4);
	structUW1(120, 2, 1, 4);
	structUW1(147, 2, 1, 2);
	structUW1(149, 2, 1, 3);
	structUW1(173, 10, 1, 2);

	structPipeHorizontal(189, 6, 1);

	structGND(185, 2, 17, 1);
	structGND(186, 3, 16, 1);
	structGND(187, 4, 15, 1);
	structGND(188, 5, 14, 1);
	structGND(190, 6, 12, 3);
	structGND(188, 9, 14, 4);

	// -- MAP 2-2-2

	this->iLevelType = 0;

	structGND(220, 0, 24, 2);

	structCastleSmall(220, 2);

	structCloud(223, 10, 2);
	structCloud(229, 6, 1);
	structCloud(238, 11, 2);

	structPipe(232, 2, 3);
	structPipeHorizontal(230, 2, 1);

	// -- MAP 2-2-3

	structCloud(275, 11, 2);
	structCloud(295, 10, 1);
	structCloud(306, 11, 1);

	structGrass(300, 2, 1);
	structGrass(310, 2, 1);

	structBush(287, 2, 2);
	structBush(303, 2, 1);

	structGND(271, 0, 42, 2);

	structPipe(274, 2, 1);

	structGND2(276, 2, 8, true);
	structGND2(284, 2, 1, 8);

	structGND2(293, 2, 1, 1);
	structEnd(293, 3, 9);

	structCastleSmall(297, 2);

	// -- END

	this->iLevelType = 2;
}

void Map::loadLVL_7_3() {
	clearMap();

	this->iMapWidth = 250;
	this->iMapHeight = 25;
	this->iLevelType = 0;
	this->iMapTime = 300;

	// ---------- LOAD LISTS ----------

	createMap();

	loadMinionsLVL_7_3();

	structGND(0, 0, 7, 2);
	structGND(207, 0, 43, 2);

	structCloud(3, 10, 2);
	structCloud(9, 6, 1);
	structCloud(18, 11, 2);
	structCloud(28, 2, 1);
	structCloud(35, 6, 1);
	structCloud(38, 7, 1);
	structCloud(46, 2, 1);
	structCloud(51, 10, 2);
	structCloud(57, 6, 1);
	structCloud(66, 11, 2);
	structCloud(76, 2, 1);
	structCloud(83, 6, 1);
	structCloud(86, 7, 1);
	structCloud(94, 2, 1);
	structCloud(99, 10, 2);
	structCloud(105, 6, 1);
	structCloud(114, 11, 2);
	structCloud(124, 2, 1);
	structCloud(131, 6, 1);
	structCloud(134, 7, 1);
	structCloud(142, 2, 1);
	structCloud(147, 10, 2);
	structCloud(153, 6, 1);
	structCloud(162, 11, 2);
	structCloud(172, 2, 1);
	structCloud(179, 6, 1);
	structCloud(182, 7, 1);
	structCloud(190, 2, 1);
	structCloud(195, 10, 2);
	structCloud(201, 6, 1);
	structCloud(210, 11, 2);
	structCloud(220, 2, 1);
	structCloud(227, 6, 1);
	structCloud(239, 9, 2);

	structT(8, 0, 8, 2);
	structGND2(10, 2, 3, true);
	structGND2(13, 2, 2, 3);

	structBridge2(15, 4, 16);
	structBridge2(32, 4, 15);
	structBridge2(48, 4, 15);
	structBridge2(69, 4, 10);
	structBridge2(85, 4, 10);
	structBridge2(100, 5, 5);

	structT(112, 0, 8, 2);

	structBridge2(122, 4, 3);
	structBridge2(128, 4, 15);
	structBridge2(147, 2, 8);
	structBridge2(160, 4, 8);

	structBridge2(171, 4, 2);
	structBridge2(175, 4, 2);
	structBridge2(179, 4, 2);

	structBridge2(184, 4, 9);

	structGND2(31, 0, 1, 5);
	structGND2(47, 0, 1, 5);
	structGND2(63, 0, 1, 5);
	structGND2(68, 0, 1, 5);
	structGND2(79, 0, 1, 5);
	structGND2(84, 0, 1, 5);
	structGND2(95, 0, 1, 5);
	structGND2(99, 0, 1, 6);
	structGND2(105, 0, 1, 6);
	structGND2(127, 0, 1, 5);
	structGND2(143, 0, 1, 5);
	structGND2(146, 0, 1, 3);
	structGND2(155, 0, 1, 3);
	structGND2(159, 0, 1, 5);
	structGND2(168, 0, 1, 5);
	structGND2(183, 0, 1, 5);

	structT(192, 0, 13, 2);

	structGND2(193, 2, 1, 3);
	structGND2(194, 2, 3, false);

	structGND2(208, 2, 8, true);
	structGND2(216, 2, 1, 8);

	structCoins(36, 9, 4, 1);
	structCoins(55, 9, 1, 1);
	structCoins(57, 9, 1, 1);
	structCoins(59, 9, 1, 1);
	structCoins(56, 8, 1, 1);
	structCoins(58, 8, 1, 1);
	structCoins(72, 9, 1, 1);
	structCoins(73, 10, 2, 1);
	structCoins(75, 9, 1, 1);
	structCoins(97, 9, 3, 1);
	structCoins(108, 9, 3, 1);
	structCoins(133, 9, 6, 1);
	structCoins(149, 6, 4, 1);
	structCoins(173, 8, 6, 1);

	struckBlockQ(102, 9, 1);
	lMap[102][9]->setSpawnMushroom(true);

	structCastleSmall(0, 2);

	structGND2(225, 2, 1, 1);
	structEnd(225, 3, 9);

	structCastleBig(228, 2);
	structCastleWall(237, 2, 13, 6);
	
	// -- END

	this->iLevelType = 0;
}

void Map::loadLVL_7_4() {
	clearMap();

	this->iMapWidth = 300;
	this->iMapHeight = 25;
	this->iLevelType = 3;
	this->iMapTime = 400;

	// ---------- LOAD LISTS ----------
	createMap();

	loadMinionsLVL_7_4();

	structGND(0, 0, 16, 5);
	structGND(0, 5, 5, 1);
	structGND(0, 6, 4, 1);
	structGND(0, 7, 3, 1);

	structGND(27, 0, 5, 5);

	structGND(32, 0, 61, 2);
	structGND(93, 2, 3, 3);

	structGND(34, 5, 16, 1);
	structGND(36, 6, 14, 1);
	structGND(37, 7, 13, 1);
	structGND(38, 8, 12, 1);

	structGND(54, 4, 13, 1);
	structGND(55, 8, 11, 1);

	structGND(70, 5, 18, 1);
	structGND(70, 6, 19, 1);
	structGND(70, 7, 20, 1);
	structGND(70, 8, 22, 1);

	structGND(96, 0, 68, 2);
	structGND(157, 2, 3, 3);

	structGND(98, 5, 16, 1);
	structGND(100, 6, 14, 1);
	structGND(101, 7, 13, 1);
	structGND(102, 8, 12, 1);

	structGND(118, 4, 13, 1);
	structGND(119, 8, 11, 1);

	structGND(134, 5, 18, 1);
	structGND(134, 6, 19, 1);
	structGND(134, 7, 20, 1);
	structGND(134, 8, 22, 1);

	structGND(157, 2, 3, 3);
	structGND(163, 8, 2, 1);
	structGND(166, 8, 2, 1);


	structGND(93, 0, 3, 2);

	structGND(0, 10, 16, 3);
	structGND(16, 12, 180, 1);
	structGND(28, 10, 4, 2);

	structLava(164, 0, 3, 2);
	structLava(16, 0, 11, 2);

	vPlatform.push_back(new Platform(4, 5, 18*32, 18*32, CCFG::GAME_HEIGHT - 16 - 7*32, CCFG::GAME_HEIGHT - 16 - 7*32, 18*32, (float)CCFG::GAME_HEIGHT - 16 - 7*32, true));
	vPlatform.push_back(new Platform(4, 5, 22*32, 22*32, CCFG::GAME_HEIGHT - 16 - 6*32, CCFG::GAME_HEIGHT - 16 - 6*32, 22*32, (float)CCFG::GAME_HEIGHT - 16 - 6*32, true));

	lMap[167][7]->setBlockID(56);

	// -- TP

	for(int i = 0; i < 3; i++) {
		lMap[60][5 + i]->setBlockID(171);
		lMap[124][5 + i]->setBlockID(171);
	}

	for(int i = 0; i < 3; i++) {
		lMap[78][9 + i]->setBlockID(169);
	}

	for(int i = 0; i < 3; i++) {
		lMap[138][9 + i]->setBlockID(169);
	}

	for(int i = 0; i < 3; i++) {
		lMap[138][2 + i]->setBlockID(170);
	}

	// -- TP

	structGND(167, 0, 1, 2);
	structLava(168, 0, 3, 2);

	structGND(168, 8, 1, 1);
	structGND(170, 8, 4, 1);
	structGND(174, 5, 6, 4);

	structGND(171, 0, 85, 2);

	structGND(182, 8, 3, 1);
	structGND(183, 4, 3, 1);

	structGND(187, 8, 7, 1);
	structGND(188, 4, 7, 1);

	structGND(196, 8, 3, 1);
	structGND(197, 4, 3, 1);

	structGND(201, 5, 1, 4);
	structGND(203, 2, 1, 1);
	structGND(204, 2, 1, 2);
	structGND(205, 2, 19, 3);
	structGND(202, 8, 15, 1);

	structGND(227, 2, 3, 3);
	structGND(232, 2, 8, 3);
	structGND(242, 2, 2, 3);
	structGND(246, 2, 2, 3);
	structGND(250, 2, 6, 3);

	structGND(196, 12, 94, 1);

	structBridge(256, 4, 13);
	structLava(256, 0, 13, 2);

	structGND(269, 0, 3, 6);
	structGND(270, 9, 2, 3);
	structGND(272, 0, 18, 2);
}

void Map::loadLVL_8_1() {
	clearMap();

	this->iMapWidth = 440;
	this->iMapHeight = 25;
	this->iLevelType = 0;
	this->iMapTime = 300;

	// ---------- LOAD LISTS ----------

	createMap();

	loadMinionsLVL_8_1();
	loadPipeEventsLVL_8_1();

	structGND(0, 0, 46, 2);
	structGND(47, 0, 1, 2);
	structGND(49, 0, 2, 2);
	structGND(52, 0, 2, 2);
	structGND(55, 0, 2, 2);
	structGND(58, 0, 111, 2);
	structGND(170, 0, 1, 2);
	structGND(172, 0, 2, 2);
	structGND(175, 0, 1, 2);
	structGND(177, 0, 2, 2);
	structGND(180, 0, 17, 2);
	structGND(198, 0, 1, 2);
	structGND(200, 0, 1, 2);
	structGND(202, 0, 19, 2);
	structGND(227, 0, 10, 2);
	structGND(238, 0, 2, 2);
	structGND(242, 0, 2, 2);
	structGND(246, 0, 44, 2);
	structGND(293, 0, 2, 2);
	structGND(298, 0, 16, 2);
	structGND(319, 0, 1, 2);
	structGND(324, 0, 35, 2);
	structGND(360, 0, 1, 2);
	structGND(362, 0, 1, 2);
	structGND(364, 0, 1, 2);
	structGND(366, 0, 34, 2);

	structCastleBig(-2, 2);
	structCastleSmall(380, 2);

	structCloud(18, 10, 1);
	structCloud(27, 11, 1);
	structCloud(30, 10, 2);
	structCloud(45, 11, 1);
	structCloud(48, 10, 2);
	structCloud(66, 10, 1);
	structCloud(75, 11, 1);
	structCloud(78, 10, 2);
	structCloud(93, 11, 1);
	structCloud(96, 10, 2);
	structCloud(114, 10, 1);
	structCloud(123, 11, 1);
	structCloud(126, 10, 2);
	structCloud(141, 11, 1);
	structCloud(144, 10, 2);
	structCloud(162, 10, 1);
	structCloud(171, 11, 1);
	structCloud(174, 10, 2);
	structCloud(189, 11, 1);
	structCloud(192, 10, 2);
	structCloud(210, 10, 1);
	structCloud(219, 11, 1);
	structCloud(222, 10, 2);
	structCloud(237, 11, 1);
	structCloud(240, 10, 2);
	structCloud(258, 10, 1);
	structCloud(267, 11, 1);
	structCloud(270, 10, 2);
	structCloud(285, 11, 1);
	structCloud(288, 10, 2);
	structCloud(306, 10, 1);
	structCloud(315, 11, 1);
	structCloud(318, 10, 2);
	structCloud(333, 11, 1);
	structCloud(336, 10, 2);
	structCloud(354, 10, 1);
	structCloud(363, 11, 1);
	structCloud(366, 10, 2);
	structCloud(381, 11, 1);
	structCloud(384, 10, 2);

	structTree(11, 2, 1, false);
	structTree(13, 2, 1, true);
	structTree(21, 2, 1, true);
	structTree(23, 2, 1, false);
	structTree(24, 2, 1, false);
	structTree(40, 2, 1, false);
	structTree(43, 2, 1, true);
	structTree(59, 2, 1, false);
	structTree(61, 2, 1, true);
	structTree(69, 2, 1, true);
	structTree(71, 2, 1, false);
	structTree(72, 2, 1, false);
	structTree(88, 2, 1, false);
	structTree(91, 2, 1, true);
	structTree(107, 2, 1, false);
	structTree(109, 2, 1, false);
	structTree(117, 2, 1, true);
	structTree(119, 2, 1, false);
	structTree(120, 2, 1, false);
	structTree(136, 2, 1, false);
	structTree(139, 2, 1, true);
	structTree(155, 2, 1, false);
	structTree(157, 2, 1, true);
	structTree(165, 2, 1, true);
	structTree(167, 2, 1, false);
	structTree(168, 2, 1, false);
	structTree(184, 2, 1, false);
	structTree(187, 2, 1, true);

	structTree(379, 2, 1, true);
	structTree(357, 2, 1, true);
	structTree(349, 2, 1, true);
	structTree(347, 2, 1, false);
	structTree(331, 2, 1, true);
	structTree(328, 2, 1, false);
	structTree(309, 2, 1, true);
	structTree(311, 2, 1, false);
	structTree(312, 2, 1, false);
	structTree(299, 2, 1, false);
	structTree(301, 2, 1, true);
	structTree(283, 2, 1, true);
	structTree(261, 2, 1, true);
	structTree(263, 2, 1, false);
	structTree(264, 2, 1, false);
	structTree(253, 2, 1, true);
	structTree(251, 2, 1, false);
	structTree(232, 2, 1, false);
	structTree(235, 2, 1, true);
	structTree(213, 2, 1, true);
	structTree(215, 2, 1, false);
	structTree(216, 2, 1, false);
	structTree(203, 2, 1, false);
	structTree(205, 2, 1, true);

	structFence(185, 2, 1);
	structFence(182, 2, 2);
	structFence(158, 2, 4);
	structFence(134, 2, 2);
	structFence(137, 2, 1);
	structFence(110, 2, 4);
	structFence(86, 2, 2);
	structFence(89, 2, 1);
	structFence(62, 2, 4);
	structFence(38, 2, 2);
	structFence(41, 2, 1);
	structFence(14, 2, 4);

	structFence(206, 2, 4);
	structFence(230, 2, 2);
	structFence(233, 2, 1);
	structFence(254, 2, 4);
	structFence(281, 2, 1);
	structFence(302, 2, 4);
	structFence(326, 2, 2);
	structFence(329, 2, 1);
	structFence(350, 2, 4);
	structFence(374, 2, 4);
	
	structPipe(35, 2, 3);
	structPipe(76, 2, 3);
	structPipe(82, 2, 2);
	structPipe(94, 2, 3);
	structPipe(104, 2, 3);
	structPipe(115, 2, 1);
	structPipe(140, 2, 2);

	struckBlockQ2(80, 6, 1);
	lMap[80][6]->setSpawnMushroom(true);
	struckBlockQ2(158, 5, 1);
	
	structCoins(64, 6, 1, 1);
	structCoins(89, 6, 1, 1);
	structCoins(98, 6, 1, 1);
	structCoins(109, 10, 2, 1);

	structBrick(184, 6, 8, 1);
	structBrick(184, 6, 8, 1);
	structBrick(184, 6, 8, 1);
	structBrick(184, 6, 8, 1);
	structBrick(184, 6, 8, 1);
	structBrick(184, 6, 8, 1);
	structBrick(184, 6, 8, 1);
	structBrick(184, 6, 8, 1);
	structBrick(184, 6, 8, 1);
	structBrick(184, 6, 8, 1);
	structBrick(184, 6, 8, 1);
	structBrick(184, 6, 8, 1);
	structBrick(184, 6, 8, 1);
	structBrick(184, 6, 8, 1);
	structBrick(154, 9, 8, 1);
	lMap[158][9]->setNumOfUse(8);
	structBrick(184, 6, 8, 1);
	lMap[186][6]->setSpawnStar(true);

	structGND2(153, 2, 1, 4);
	structGND2(163, 2, 1, 4);

	structGND2(303, 2, 1, 2);
	structGND2(307, 2, 1, 2);

	structGND2(210, 2, 1, 2);
	structGND2(360, 2, 1, 2);
	structGND2(362, 2, 1, 4);
	structGND2(364, 2, 1, 6);
	structGND2(366, 2, 2, 8);
	structGND2(275, 2, 6, true);
	
	structPipe(344, 2, 2);
	structPipe(355, 2, 1);
	structPipe(246, 2, 4);
	structPipe(242, 2, 3);
	structPipe(238, 2, 2);

	structCoins(223, 6, 2, 1);
	structCoins(283, 6, 2, 1);
	structCoins(291, 6, 1, 1);
	structCoins(296, 6, 1, 1);
	structCoins(316, 6, 2, 1);
	structCoins(321, 6, 2, 1);

	structGND2(376, 2, 1, 1);
	structEnd(376, 3, 9);

	this->iLevelType = 1;

	structGND(410, 0, 17, 2);

	structBrick(410, 2, 1, 11);
	structBrick(413, 5, 10, 1);
	structBrick(413, 9, 10, 4);
	structBrick(423, 4, 2, 9);

	lMap[422][5]->setNumOfUse(10);

	structCoins(413, 2, 9, 1);
	structCoins(414, 6, 8, 1);

	structPipeVertical(425, 2, 10);
	structPipeHorizontal(423, 2, 1);

	this->iLevelType = 0;
}

void Map::loadLVL_8_2() {
	clearMap();

	this->iMapWidth = 400;
	this->iMapHeight = 25;
	this->iLevelType = 0;
	this->iMapTime = 400;

	// ---------- LOAD LISTS ----------

	createMap();

	loadMinionsLVL_8_2();
	loadPipeEventsLVL_8_2();

	structBulletBill(85, 2, 0);
	structBulletBill(93, 3, 0);
	structBulletBill(93, 1, 0);
	structBulletBill(105, 2, 0);
	structBulletBill(115, 1, 0);
	structBulletBill(119, 6, 0);
	structBulletBill(125, 2, 1);
	structBulletBill(175, 2, 0);
	structBulletBill(191, 1, 0);
	structBulletBill(191, 3, 0);

	structGND(0, 0, 15, 2);
	structGND(16, 0, 5, 2);
	structGND(22, 0, 14, 2);
	structGND(37, 0, 8, 2);
	structGND(46, 0, 4, 2);
	structGND(51, 0, 1, 2);
	structGND(53, 0, 3, 2);
	structGND(57, 0, 6, 2);
	structGND(64, 0, 14, 2);
	structGND(80, 0, 4, 2);
	structGND(85, 0, 53, 2);
	structGND(139, 0, 5, 2);
	structGND(145, 0, 1, 2);
	structGND(147, 0, 1, 2);
	structGND(154, 0, 20, 2);
	structGND(175, 0, 1, 2);
	structGND(179, 0, 23, 2);
	structGND(203, 0, 1, 2);
	structGND(206, 0, 31, 2);

	structCloud(0, 10, 2);
	structCloud(18, 10, 1);
	structCloud(27, 11, 1);
	structCloud(30, 10, 2);
	structCloud(45, 11, 1);
	structCloud(48, 10, 2);
	structCloud(66, 10, 1);
	structCloud(75, 11, 1);
	structCloud(78, 10, 1);
	structCloud(93, 11, 1);
	structCloud(96, 10, 2);
	structCloud(114, 10, 1);
	structCloud(123, 11, 1);
	structCloud(126, 10, 2);
	structCloud(141, 11, 1);
	structCloud(144, 10, 2);
	structCloud(162, 10, 1);
	structCloud(171, 11, 1);
	structCloud(174, 10, 2);
	structCloud(189, 11, 1);
	structCloud(192, 10, 2);
	structCloud(210, 10, 1);
	structCloud(219, 11, 1);
	structCloud(222, 10, 2);
	
	structFence(14, 2, 1);
	structFence(16, 2, 1);
	structFence(38, 2, 4);
	structFence(62, 2, 1);
	structFence(64, 2, 2);
	structFence(86, 2, 4);
	structFence(110, 2, 4);
	structFence(134, 2, 4);
	structFence(158, 2, 4);
	structFence(208, 2, 2);
	structFence(230, 2, 2);

	structCastleSmall(0, 2);
	structCastleSmall(220, 2);

	structTree(232, 2, 1, false);
	structTree(215, 2, 1, false);
	structTree(213, 2, 1, true);
	structTree(187, 2, 1, true);
	structTree(165, 2, 1, true);
	structTree(167, 2, 1, false);
	structTree(168, 2, 1, false);
	structTree(155, 2, 1, false);
	structTree(136, 2, 1, false);
	structTree(139, 2, 1, true);
	structTree(117, 2, 1, true);
	structTree(119, 2, 1, false);
	structTree(120, 2, 1, false);
	structTree(107, 2, 1, false);
	structTree(109, 2, 1, true);
	structTree(88, 2, 1, false);
	structTree(91, 2, 1, true);
	structTree(69, 2, 1, true);
	structTree(71, 2, 1, false);
	structTree(72, 2, 1, false);
	structTree(59, 2, 1, false);
	structTree(61, 2, 1, true);
	structTree(40, 2, 1, false);
	structTree(43, 2, 1, true);
	structTree(11, 2, 1, false);
	structTree(13, 2, 1, true);

	structGND2(206, 2, 2, 8);
	structGND2(203, 2, 1, 5);
	structGND2(199, 2, 3, true);
	structGND2(182, 2, 5, true);

	structPipe(163, 2, 1);
	structPipe(156, 2, 3);
	structPipe(142, 2, 1);
	structPipe(131, 2, 1);

	structBrick(118, 5, 1, 1);
	structBrick(120, 5, 1, 1);
	lMap[120][5]->setNumOfUse(8);
	structGND2(119, 5, 1, 1);
	structBrick(110, 5, 2, 1);
	structBrick(99, 5, 2, 1);
	lMap[100][5]->setSpawnMushroom(true);

	structBrick(77, 5, 2, 1);
	structBrick(43, 9, 33, 1);
	lMap[44][9]->setSpawnMushroom(true);
	lMap[44][9]->setPowerUP(false);

	addSpring(44*32, 336);

	struckBlockQ(29, 5, 4);

	structGND2(17, 2, 4, true);
	structGND2(22, 2, 1, 6);
	structGND2(23, 2, 1, 7);
	structGND2(24, 2, 2, 8);

	structGND2(216, 2, 1, 1);
	structEnd(216, 3, 9);

	// -- MAP 8-2-2

	this->iLevelType = 1;

	structGND(240, 0, 17, 2);

	structBrick(239, 2, 1, 18);
	structBrick(240, 2, 1, 11);
	structBrick(244, 7, 1, 2);
	structBrick(245, 7, 5, 1);
	structBrick(250, 7, 1, 6);
	structBrick(251, 8, 2, 1);
	structBrick(244, 12, 6, 1);

	structBrick(254, 7, 1, 1);
	lMap[254][7]->setNumOfUse(8);

	structCoins(245, 8, 5, 2);

	structPipeVertical(255, 2, 10);
	structPipeHorizontal(253, 2, 1);

	this->iLevelType = 0;
}

void Map::loadLVL_8_3() {
	clearMap();

	this->iMapWidth = 240;
	this->iMapHeight = 25;
	this->iLevelType = 0;
	this->iMapTime = 300;

	// ---------- LOAD LISTS ----------

	createMap();

	loadMinionsLVL_8_3();

	structGND(0, 0, 69, 2);
	structGND(71, 0, 4, 2);
	structGND(77, 0, 47, 2);
	structGND(126, 0, 2, 2);
	structGND(130, 0, 67, 2);
	structGND(208, 0, 31, 2);
	
	structBulletBill(18, 2, 0);
	structBulletBill(34, 2, 1);
	structBulletBill(86, 2, 0);
	structBulletBill(86, 2, 0);
	structBulletBill(86, 2, 0);
	structBulletBill(86, 2, 0);
	structBulletBill(86, 2, 0);

	structCloud(0, 10, 2);
	structCloud(18, 10, 1);
	structCloud(27, 11, 1);
	structCloud(30, 10, 2);
	structCloud(45, 11, 1);
	structCloud(48, 10, 2);
	structCloud(66, 10, 1);
	structCloud(75, 11, 1);
	structCloud(78, 10, 2);
	structCloud(93, 11, 1);
	structCloud(96, 10, 2);
	structCloud(114, 10, 1);
	structCloud(123, 11, 1);
	structCloud(126, 10, 2);
	structCloud(141, 11, 1);
	structCloud(144, 10, 2);
	structCloud(162, 10, 1);
	structCloud(171, 11, 1);
	structCloud(174, 10, 2);
	structCloud(189, 11, 1);
	structCloud(192, 10, 2);
	structCloud(210, 10, 1);
	structCloud(219, 11, 1);

	structCastleSmall(0, 2);

	structTree(11, 2, 1, false);
	structTree(13, 2, 1, true);
	structTree(21, 2, 1, true);
	structTree(23, 2, 1, false);
	structTree(59, 2, 1, false);
	structTree(61, 2, 1, true);
	structTree(107, 2, 1, false);
	structTree(117, 2, 1, true);
	structTree(119, 2, 1, false);
	structTree(120, 2, 1, false);
	structTree(167, 2, 1, false);
	structTree(213, 2, 1, true);
	structTree(215, 2, 1, false);
	structTree(216, 2, 1, false);
	
	structGND2(195, 2, 1, 2);
	structGND2(198, 3, 1, 1);
	structGND2(200, 5, 1, 1);
	structGND2(202, 7, 1, 1);
	structGND2(204, 9, 2, 1);

	structGND2(95, 2, 1, 3);
	structGND2(109, 2, 2, 4);
	structGND2(71, 2, 4, false);

	structBrick(115, 5, 8, 1);
	structBrick(115, 9, 8, 1);
	lMap[116][9]->setSpawnMushroom(true);
	structBrick(60, 5, 8, 1);
	structBrick(60, 9, 8, 1);
	lMap[66][9]->setSpawnMushroom(true);

	structFence(208, 2, 2);
	structFence(87, 2, 1);
	structFence(62, 2, 4);
	structFence(14, 2, 4);
	structFence(111, 2, 3);

	structPipe(53, 2, 3);
	structPipe(126, 2, 3);
	structPipe(168, 2, 2);

	structCastleWall(24, 2, 8, 6);
	structCastleWall(37, 2, 14, 6);
	structCastleWall(79, 2, 6, 6);
	structCastleWall(88, 2, 6, 6);
	structCastleWall(97, 2, 10, 6);
	structCastleWall(132, 2, 34, 6);
	structCastleWall(172, 2, 20, 6);
	
	structBrick(190, 5, 1, 1);
	lMap[190][5]->setNumOfUse(10);

	structCastleBig(220, 2);
	structCastleWall(229, 2, 10, 6);

	structGND2(214, 2, 1, 1);
	structEnd(214, 3, 9);
}

void Map::loadLVL_8_4() {
	clearMap();

	this->iMapWidth = 480;
	this->iMapHeight = 25;
	this->iLevelType = 3;
	this->iMapTime = 400;

	// ---------- LOAD LISTS ----------

	createMap();

	loadMinionsLVL_8_4();
	loadPipeEventsLVL_8_4();

	structGND(0, 0, 369, 2);
	structGND(0, 12, 369, 1);

	structGND(0, 0, 6, 5);
	structGND(0, 5, 5, 1);
	structGND(0, 6, 4, 1);
	structGND(0, 7, 3, 1);

	structLava(6, 0, 5, 2);

	structGND(11, 0, 55, 2);

	structLava(66, 0, 9, 2);

	structGND(62, 2, 4, 3);

	structGND(75, 0, 21, 5);
	structGND(76, 8, 4, 1);

	structPipe(19, 0, 3);
	structPipe(51, 0, 3);
	structPipe(81, 0, 6);
	structPipe(90, 0, 7);
	structPipe(126, 0, 3);
	structPipe(133, 0, 4);
	structPipe(143, 0, 3);
	structPipe(153, 0, 4);
	structPipe(215, 0, 3);

	structLava(155, 0, 3, 2);

	struckBlockQ2(161, 5, 1);

	structGND(163, 5, 2, 1);
	structPipe(163, 6, 2);

	vPlatform.push_back(new Platform(4, 3, 68*32 - 16, 71*32 + 16, CCFG::GAME_HEIGHT - 16 - 32, CCFG::GAME_HEIGHT - 16 - 32, 68*32 - 16, (float)CCFG::GAME_HEIGHT - 16 - 30, true));

	for(int i = 0; i < 10; i++) {
		lMap[110][2 + i]->setBlockID(169);
	}

	for(int i = 0; i < 10; i++) {
		lMap[191][2 + i]->setBlockID(170);
	}

	structGND(218, 2, 22, 3);
	structGND(244, 2, 16, 3);

	structLava(240, 0, 4, 2);

	for(int i = 0; i < 10; i++) {
		lMap[274][2 + i]->setBlockID(171);
	}

	structPipe(302, 0, 3);

	structPipe(224, 0, 6);
	structPipe(232, 0, 7);
	structPipe(248, 0, 6);

	structPipe(309, 0, 3);

	structLava(320, 0, 5, 2);

	structGND(325, 2, 6, 3);
	structGND(325, 10, 6, 2);

	structBridge(331, 4, 13);
	structLava(331, 0, 13, 2);

	structGND(344, 2, 3, 4);
	structGND(345, 9, 2, 3);

	this->iLevelType = 2;

	structWater(392, 2, 67, 11);
	structPipeHorizontal(458, 6, 1);

	this->iLevelType = 7;

	structGND(385, 0, 7, 13);
	structGND(392, 0, 79, 2);
	structGND(396, 2, 12, 3);
	structGND(401, 5, 7, 1);

	structGND(396, 10, 12, 3);
	structGND(401, 9, 7, 1);

	structGND(408, 12, 63, 1);

	structGND(429, 2, 3, 3);
	structGND(429, 9, 3, 3);

	structGND(457, 2, 14, 4);
	structGND(459, 6, 12, 3);
	structGND(457, 9, 14, 3);

	structPipe(393, 0, 3);

	this->iLevelType = 3;
}

/* ******************************************** */

// ----- POS 0 = TOP, 1 = BOT
bool Map::blockUse(int nX, int nY, int iBlockID, int POS) {
	if(POS == 0) {
		switch(iBlockID) {
			case 8: case 55: // ----- BlockQ
				if(lMap[nX][nY]->getSpawnMushroom()) {
					if(lMap[nX][nY]->getPowerUP()) {
						if(oPlayer->getPowerLVL() == 0) {
							lMinion[getListID(32 * nX)].push_back(new Mushroom(32 * nX, CCFG::GAME_HEIGHT - 16 - 32 * nY, true, nX, nY));
						} else {
							lMinion[getListID(32 * nX)].push_back(new Flower(32 * nX, CCFG::GAME_HEIGHT - 16 - 32 * nY, nX, nY));
						}
					} else {
						lMinion[getListID(32 * nX)].push_back(new Mushroom(32 * nX, CCFG::GAME_HEIGHT - 16 - 32 * nY, false, nX, nY));
					}
					CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cMUSHROOMAPPER);
				} else {
					lCoin.push_back(new Coin(nX * 32 + 7, CCFG::GAME_HEIGHT - nY * 32 - 48));
					oPlayer->setScore(oPlayer->getScore() + 200);
					CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cCOIN);
					oPlayer->setCoins(oPlayer->getCoins() + 1);
				}

				if(lMap[nX][nY]->getNumOfUse() > 1) {
					lMap[nX][nY]->setNumOfUse(lMap[nX][nY]->getNumOfUse() - 1);
				} else {
					lMap[nX][nY]->setNumOfUse(0);
					lMap[nX][nY]->setBlockID(iLevelType == 0 || iLevelType == 4 ? 9 : iLevelType == 1 ? 56 : 56);
				}
				
				lMap[nX][nY]->startBlockAnimation();
				checkCollisionOnTopOfTheBlock(nX, nY);
				break;
			case 13: case 28: case 81: // ----- Brick
				if(lMap[nX][nY]->getSpawnStar()) {
					lMap[nX][nY]->setBlockID(iLevelType == 0 || iLevelType == 4 ? 9 : iLevelType == 1 ? 56 : 80);
					lMinion[getListID(32 * nX)].push_back(new Star(32 * nX, CCFG::GAME_HEIGHT - 16 - 32 * nY, nX, nY));
					lMap[nX][nY]->startBlockAnimation();
					CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cMUSHROOMAPPER);
				} else if(lMap[nX][nY]->getSpawnMushroom()) {
					lMap[nX][nY]->setBlockID(iLevelType == 0 || iLevelType == 4 ? 9 : iLevelType == 1 ? 56 : 80);
					if(lMap[nX][nY]->getPowerUP()) {
						if(oPlayer->getPowerLVL() == 0) {
							lMinion[getListID(32 * nX)].push_back(new Mushroom(32 * nX, CCFG::GAME_HEIGHT - 16 - 32 * nY, true, nX, nY));
						} else {
							lMinion[getListID(32 * nX)].push_back(new Flower(32 * nX, CCFG::GAME_HEIGHT - 16 - 32 * nY, nX, nY));
						}
					} else {
						lMinion[getListID(32 * nX)].push_back(new Mushroom(32 * nX, CCFG::GAME_HEIGHT - 16 - 32 * nY, false, nX, nY));
					}
					lMap[nX][nY]->startBlockAnimation();
					CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cMUSHROOMAPPER);
				} else if(lMap[nX][nY]->getNumOfUse() > 0) {
					lCoin.push_back(new Coin(nX * 32 + 7, CCFG::GAME_HEIGHT - nY * 32 - 48));
					oPlayer->setScore(oPlayer->getScore() + 200);
					oPlayer->setCoins(oPlayer->getCoins() + 1);

					lMap[nX][nY]->setNumOfUse(lMap[nX][nY]->getNumOfUse() - 1);
					if(lMap[nX][nY]->getNumOfUse() == 0) {
						lMap[nX][nY]->setBlockID(iLevelType == 0 || iLevelType == 4 ? 9 : iLevelType == 1 ? 56 : 80);
					}

					CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cCOIN);

					lMap[nX][nY]->startBlockAnimation();
				} else {
					if(oPlayer->getPowerLVL() > 0) {
						lMap[nX][nY]->setBlockID(0);
						lBlockDebris.push_back(new BlockDebris(nX * 32, CCFG::GAME_HEIGHT - 48 - nY * 32));
						CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cBLOCKBREAK);
					} else {
						lMap[nX][nY]->startBlockAnimation();
						CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cBLOCKHIT);
					}
				}

				checkCollisionOnTopOfTheBlock(nX, nY);
				break;
			case 24: // ----- BlockQ2
				if(lMap[nX][nY]->getSpawnMushroom()) {
					if(lMap[nX][nY]->getPowerUP()) {
						if(oPlayer->getPowerLVL() == 0) {
							lMinion[getListID(32 * nX)].push_back(new Mushroom(32 * nX, CCFG::GAME_HEIGHT - 16 - 32 * nY, true, nX, nY));
						} else {
							lMinion[getListID(32 * nX)].push_back(new Flower(32 * nX, CCFG::GAME_HEIGHT - 16 - 32 * nY, nX, nY));
						}
					} else {
						lMinion[getListID(32 * nX)].push_back(new Mushroom(32 * nX, CCFG::GAME_HEIGHT - 16 - 32 * nY, false, nX, nY));
					}
					CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cMUSHROOMAPPER);
				} else {
					lCoin.push_back(new Coin(nX * 32 + 7, CCFG::GAME_HEIGHT - nY * 32 - 48));
					oPlayer->setCoins(oPlayer->getCoins() + 1);
					oPlayer->setScore(oPlayer->getScore() + 200);
					CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cCOIN);

					lMap[nX][nY]->startBlockAnimation();
				}

				lMap[nX][nY]->setBlockID(iLevelType == 0 || iLevelType == 4 ? 9 : iLevelType == 1 ? 56 : 80);
				checkCollisionOnTopOfTheBlock(nX, nY);
				break;
			case 128: case 129:
				spawnVine(nX, nY, iBlockID);
				lMap[nX][nY]->setBlockID(iBlockID == 128 ? 9 : 56);
				lMap[nX][nY]->startBlockAnimation();
				break;
			default:
				break;
		}
	} else if(POS == 1) {
		switch(iBlockID) {
			case 21: case 23: case 31: case 33: case 98: case 100: case 113: case 115: case 137: case 139: case 177: case 179: // Pipe
				pipeUse();
				break;
			case 40: case 41: case 123: case 124: case 182: // End
				EndUse();
				break;
			case 82:
				EndBoss();
				break;
			default:
				break;
		}
	}
	
	switch(iBlockID) {
		case 29: case 71: case 72: case 73:// COIN
			lMap[nX][nY]->setBlockID(iLevelType == 2 ? 94 : 0);
			oPlayer->addCoin();
			CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cCOIN);
			return false;
			break;
		case 36: case 38: case 60: case 62: case 103: case 105: case 118: case 120: // Pipe
			pipeUse();
			break;
		case 127: // BONUS END
			EndBonus();
			break;
		case 169:
			TPUse();
			break;
		case 170:
			TPUse2();
			break;
		case 171:
			TPUse3();
			break;
		default:
			break;
	}

	return true;
}

void Map::pipeUse() {
	for(unsigned int i = 0; i < lPipe.size(); i++) {
		lPipe[i]->checkUse();
	}
}

void Map::EndUse() {
	inEvent = true;

	oEvent->resetData();
	oPlayer->resetJump();
	oPlayer->stopMove();

	oEvent->newUnderWater = false;

	CCFG::getMusic()->StopMusic();
	CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cLEVELEND);

	oEvent->eventTypeID = oEvent->eEnd;

	if(oPlayer->getYPos() < CCFG::GAME_HEIGHT - 16 - 10*32) {
		oFlag->iPoints = 5000;
	} else if(oPlayer->getYPos() < CCFG::GAME_HEIGHT - 16 - 8*32) {
		oFlag->iPoints = 2000;
	} else if(oPlayer->getYPos() < CCFG::GAME_HEIGHT - 16 - 6*32) {
		oFlag->iPoints = 500;
	} else if(oPlayer->getYPos() < CCFG::GAME_HEIGHT - 16 - 4*32) {
		oFlag->iPoints = 200;
	} else {
		oFlag->iPoints = 100;
	}

	oEvent->vOLDDir.push_back(oEvent->eRIGHTEND);
	oEvent->vOLDLength.push_back(oPlayer->getHitBoxX());

	oEvent->vOLDDir.push_back(oEvent->eENDBOT1);
	oEvent->vOLDLength.push_back((CCFG::GAME_HEIGHT - 16 - 32 * 2) - oPlayer->getYPos() - oPlayer->getHitBoxY() - 2);

	oEvent->vOLDDir.push_back(oEvent->eENDBOT2);
	oEvent->vOLDLength.push_back((CCFG::GAME_HEIGHT - 16 - 32 * 2) - oPlayer->getYPos() - oPlayer->getHitBoxY() - 2);

	oEvent->vOLDDir.push_back(oEvent->eRIGHTEND);
	oEvent->vOLDLength.push_back(oPlayer->getHitBoxX());

	oEvent->vOLDDir.push_back(oEvent->eBOTRIGHTEND);
	oEvent->vOLDLength.push_back(32);

	oEvent->vOLDDir.push_back(oEvent->eRIGHTEND);
	oEvent->vOLDLength.push_back(132);

	oEvent->iSpeed = 3;

	switch(currentLevelID) {
		case 0: {
			oEvent->newLevelType = 100;

			oEvent->iDelay = 1500;
			oEvent->newCurrentLevel = 1;
			oEvent->inEvent = true;

			oEvent->newMapXPos = -210*32;
			oEvent->newPlayerXPos = 64;
			oEvent->newPlayerYPos = CCFG::GAME_HEIGHT - 48 - oPlayer->getHitBoxY();
			oEvent->newMoveMap = false;

			oPlayer->setMoveDirection(true);

			oEvent->reDrawX.push_back(204);
			oEvent->reDrawY.push_back(2);
			oEvent->reDrawX.push_back(204);
			oEvent->reDrawY.push_back(3);
			oEvent->reDrawX.push_back(205);
			oEvent->reDrawY.push_back(2);
			oEvent->reDrawX.push_back(205);
			oEvent->reDrawY.push_back(3);
			break;
		}
		case 1: {
			oEvent->newLevelType = 0;

			oEvent->iDelay = 1500;
			oEvent->newCurrentLevel = 2;
			oEvent->inEvent = false;

			oEvent->newMapXPos = 0;
			oEvent->newPlayerXPos = 84;
			oEvent->newPlayerYPos = CCFG::GAME_HEIGHT - 48 - oPlayer->getHitBoxY();
			oEvent->newMoveMap = true;

			oPlayer->setMoveDirection(true);

			oEvent->reDrawX.push_back(309);
			oEvent->reDrawY.push_back(2);
			oEvent->reDrawX.push_back(309);
			oEvent->reDrawY.push_back(3);
			oEvent->reDrawX.push_back(310);
			oEvent->reDrawY.push_back(2);
			oEvent->reDrawX.push_back(310);
			oEvent->reDrawY.push_back(3);
			break;
		}
		case 2: {
			oEvent->newLevelType = 3;

			oEvent->iDelay = 1500;
			oEvent->newCurrentLevel = 3;
			oEvent->inEvent = false;

			oEvent->newMapXPos = 0;
			oEvent->newPlayerXPos = 84;
			oEvent->newPlayerYPos = 150;
			oEvent->newMoveMap = true;

			oPlayer->setMoveDirection(true);

			oEvent->vOLDDir.push_back(oEvent->eRIGHT);
			oEvent->vOLDLength.push_back(32);

			oFlag->castleFlagExtraXPos = 32;

			oEvent->reDrawX.push_back(159);
			oEvent->reDrawY.push_back(2);
			oEvent->reDrawX.push_back(159);
			oEvent->reDrawY.push_back(3);
			oEvent->reDrawX.push_back(160);
			oEvent->reDrawY.push_back(2);
			oEvent->reDrawX.push_back(160);
			oEvent->reDrawY.push_back(3);
			break;
		}
		case 4: {
			oEvent->newLevelType = 100;

			oEvent->iDelay = 1500;
			oEvent->newCurrentLevel = 5;
			oEvent->inEvent = true;

			oEvent->newMapXPos = -220*32;
			oEvent->newPlayerXPos = 64;
			oEvent->newPlayerYPos = CCFG::GAME_HEIGHT - 48 - oPlayer->getHitBoxY();
			oEvent->newMoveMap = false;

			oPlayer->setMoveDirection(true);

			oEvent->reDrawX.push_back(206);
			oEvent->reDrawY.push_back(2);
			oEvent->reDrawX.push_back(206);
			oEvent->reDrawY.push_back(3);
			oEvent->reDrawX.push_back(207);
			oEvent->reDrawY.push_back(2);
			oEvent->reDrawX.push_back(207);
			oEvent->reDrawY.push_back(3);
			break;
		}
		case 5: {
			oEvent->newLevelType = 0;

			oEvent->iDelay = 1500;
			oEvent->newCurrentLevel = 6;
			oEvent->inEvent = false;

			oEvent->newMapXPos = 0;
			oEvent->newPlayerXPos = 84;
			oEvent->newPlayerYPos = CCFG::GAME_HEIGHT - 48 - oPlayer->getHitBoxY();
			oEvent->newMoveMap = true;

			oPlayer->setMoveDirection(true);

			oEvent->vOLDDir.push_back(oEvent->eRIGHT);
			oEvent->vOLDLength.push_back(32);

			oFlag->castleFlagExtraXPos = 32;

			oEvent->reDrawX.push_back(299);
			oEvent->reDrawY.push_back(2);
			oEvent->reDrawX.push_back(299);
			oEvent->reDrawY.push_back(3);
			oEvent->reDrawX.push_back(300);
			oEvent->reDrawY.push_back(2);
			oEvent->reDrawX.push_back(300);
			oEvent->reDrawY.push_back(3);
			break;
		}
		case 6:
			oEvent->newLevelType = 3;

			oEvent->iDelay = 1500;
			oEvent->newCurrentLevel = currentLevelID + 1;
			oEvent->inEvent = false;

			oEvent->newMapXPos = 0;
			oEvent->newPlayerXPos = 84;
			oEvent->newPlayerYPos = 150;
			oEvent->newMoveMap = true;

			oPlayer->setMoveDirection(true);

			oEvent->vOLDDir.push_back(oEvent->eRIGHT);
			oEvent->vOLDLength.push_back(32);

			oFlag->castleFlagExtraXPos = 32;

			oEvent->reDrawX.push_back(232);
			oEvent->reDrawY.push_back(2);
			oEvent->reDrawX.push_back(232);
			oEvent->reDrawY.push_back(3);
			oEvent->reDrawX.push_back(234);
			oEvent->reDrawY.push_back(2);
			oEvent->reDrawX.push_back(234);
			oEvent->reDrawY.push_back(3);
			break;
		case 8:
			oEvent->newLevelType = 4;

			oEvent->iDelay = 1500;
			oEvent->newCurrentLevel = currentLevelID + 1;
			oEvent->inEvent = false;

			oEvent->newMapXPos = 0;
			oEvent->newPlayerXPos = 84;
			oEvent->newPlayerYPos = CCFG::GAME_HEIGHT - 48 - oPlayer->getHitBoxY();
			oEvent->newMoveMap = true;

			oPlayer->setMoveDirection(true);

			oEvent->reDrawX.push_back(206);
			oEvent->reDrawY.push_back(2);
			oEvent->reDrawX.push_back(206);
			oEvent->reDrawY.push_back(3);
			oEvent->reDrawX.push_back(207);
			oEvent->reDrawY.push_back(2);
			oEvent->reDrawX.push_back(207);
			oEvent->reDrawY.push_back(3);
			break;
		case 9:
			oEvent->newLevelType = 4;

			oEvent->iDelay = 1500;
			oEvent->newCurrentLevel = currentLevelID + 1;
			oEvent->inEvent = false;

			oEvent->newMapXPos = 0;
			oEvent->newPlayerXPos = 84;
			oEvent->newPlayerYPos = CCFG::GAME_HEIGHT - 48 - oPlayer->getHitBoxY();
			oEvent->newMoveMap = true;

			oPlayer->setMoveDirection(true);

			oEvent->reDrawX.push_back(215);
			oEvent->reDrawY.push_back(2);
			oEvent->reDrawX.push_back(215);
			oEvent->reDrawY.push_back(3);
			oEvent->reDrawX.push_back(216);
			oEvent->reDrawY.push_back(2);
			oEvent->reDrawX.push_back(216);
			oEvent->reDrawY.push_back(3);
			break;
		case 10:
			oEvent->newLevelType = 3;

			oEvent->iDelay = 1500;
			oEvent->newCurrentLevel = currentLevelID + 1;
			oEvent->inEvent = false;

			oEvent->newMapXPos = 0;
			oEvent->newPlayerXPos = 84;
			oEvent->newPlayerYPos = 150;
			oEvent->newMoveMap = true;

			oEvent->vOLDDir.push_back(oEvent->eRIGHT);
			oEvent->vOLDLength.push_back(32);

			oFlag->castleFlagExtraXPos = 32;

			oPlayer->setMoveDirection(true);

			oEvent->reDrawX.push_back(158);
			oEvent->reDrawY.push_back(2);
			oEvent->reDrawX.push_back(158);
			oEvent->reDrawY.push_back(3);
			oEvent->reDrawX.push_back(159);
			oEvent->reDrawY.push_back(2);
			oEvent->reDrawX.push_back(159);
			oEvent->reDrawY.push_back(3);
			break;
		case 12: {
			oEvent->newLevelType = 100;

			oEvent->iDelay = 1500;
			oEvent->newCurrentLevel = currentLevelID + 1;
			oEvent->inEvent = true;

			oEvent->newMapXPos = -240*32;
			oEvent->newPlayerXPos = 64;
			oEvent->newPlayerYPos = CCFG::GAME_HEIGHT - 48 - oPlayer->getHitBoxY();
			oEvent->newMoveMap = false;

			oPlayer->setMoveDirection(true);

			oEvent->reDrawX.push_back(231);
			oEvent->reDrawY.push_back(2);
			oEvent->reDrawX.push_back(231);
			oEvent->reDrawY.push_back(3);
			oEvent->reDrawX.push_back(232);
			oEvent->reDrawY.push_back(2);
			oEvent->reDrawX.push_back(232);
			oEvent->reDrawY.push_back(3);
			break;
		case 13:
			oEvent->newLevelType = 0;

			oEvent->iDelay = 1500;
			oEvent->newCurrentLevel = currentLevelID + 1;
			oEvent->inEvent = false;

			oEvent->newMapXPos = 0;
			oEvent->newPlayerXPos = 84;
			oEvent->newPlayerYPos = CCFG::GAME_HEIGHT - 48 - oPlayer->getHitBoxY();
			oEvent->newMoveMap = true;

			oPlayer->setMoveDirection(true);

			oEvent->reDrawX.push_back(419);
			oEvent->reDrawY.push_back(2);
			oEvent->reDrawX.push_back(419);
			oEvent->reDrawY.push_back(3);
			oEvent->reDrawX.push_back(420);
			oEvent->reDrawY.push_back(2);
			oEvent->reDrawX.push_back(420);
			oEvent->reDrawY.push_back(3);
			break;
		case 14:
			oEvent->newLevelType = 3;

			oEvent->iDelay = 1500;
			oEvent->newCurrentLevel = currentLevelID + 1;
			oEvent->inEvent = false;

			oEvent->newMapXPos = 0;
			oEvent->newPlayerXPos = 84;
			oEvent->newPlayerYPos = 150;
			oEvent->newMoveMap = true;

			oEvent->vOLDDir.push_back(oEvent->eRIGHT);
			oEvent->vOLDLength.push_back(32);

			oFlag->castleFlagExtraXPos = 32;

			oPlayer->setMoveDirection(true);

			oEvent->reDrawX.push_back(154);
			oEvent->reDrawY.push_back(2);
			oEvent->reDrawX.push_back(154);
			oEvent->reDrawY.push_back(3);
			oEvent->reDrawX.push_back(155);
			oEvent->reDrawY.push_back(2);
			oEvent->reDrawX.push_back(155);
			oEvent->reDrawY.push_back(3);
			break;
		}
		case 16: {
			oEvent->newLevelType = 0;

			oEvent->iDelay = 1500;
			oEvent->newCurrentLevel = currentLevelID + 1;
			oEvent->inEvent = false;

			oEvent->newMapXPos = -80*32;
			oEvent->newPlayerXPos = 84;
			oEvent->newPlayerYPos = CCFG::GAME_HEIGHT - 48 - oPlayer->getHitBoxY();
			oEvent->newMoveMap = true;

			oPlayer->setMoveDirection(true);

			oEvent->reDrawX.push_back(205);
			oEvent->reDrawY.push_back(2);
			oEvent->reDrawX.push_back(205);
			oEvent->reDrawY.push_back(3);
			oEvent->reDrawX.push_back(206);
			oEvent->reDrawY.push_back(2);
			oEvent->reDrawX.push_back(206);
			oEvent->reDrawY.push_back(3);
			break;
		}
		case 17: {
			oEvent->newLevelType = 0;

			oEvent->iDelay = 1500;
			oEvent->newCurrentLevel = currentLevelID + 1;
			oEvent->inEvent = false;

			oEvent->newMapXPos = 0;
			oEvent->newPlayerXPos = 84;
			oEvent->newPlayerYPos = CCFG::GAME_HEIGHT - 48 - oPlayer->getHitBoxY();
			oEvent->newMoveMap = true;

			oPlayer->setMoveDirection(true);

			oEvent->reDrawX.push_back(286);
			oEvent->reDrawY.push_back(2);
			oEvent->reDrawX.push_back(286);
			oEvent->reDrawY.push_back(3);
			oEvent->reDrawX.push_back(287);
			oEvent->reDrawY.push_back(2);
			oEvent->reDrawX.push_back(287);
			oEvent->reDrawY.push_back(3);
			break;
		}
		case 18: {
			oEvent->newLevelType = 3;

			oEvent->iDelay = 1500;
			oEvent->newCurrentLevel = currentLevelID + 1;
			oEvent->inEvent = false;

			oEvent->newMapXPos = 0;
			oEvent->newPlayerXPos = 84;
			oEvent->newPlayerYPos = 150;
			oEvent->newMoveMap = true;

			oEvent->vOLDDir.push_back(oEvent->eRIGHT);
			oEvent->vOLDLength.push_back(32);

			oFlag->castleFlagExtraXPos = 32;

			oPlayer->setMoveDirection(true);

			oEvent->reDrawX.push_back(159);
			oEvent->reDrawY.push_back(2);
			oEvent->reDrawX.push_back(159);
			oEvent->reDrawY.push_back(3);
			oEvent->reDrawX.push_back(160);
			oEvent->reDrawY.push_back(2);
			oEvent->reDrawX.push_back(160);
			oEvent->reDrawY.push_back(3);
			break;
		}
		case 20: {
			oEvent->newLevelType = 4;

			oEvent->iDelay = 1500;
			oEvent->newCurrentLevel = currentLevelID + 1;
			oEvent->inEvent = false;

			oEvent->newMapXPos = -85*32;
			oEvent->newPlayerXPos = 84;
			oEvent->newPlayerYPos = CCFG::GAME_HEIGHT - 48 - oPlayer->getHitBoxY();
			oEvent->newMoveMap = true;

			oEvent->vOLDDir.push_back(oEvent->eRIGHT);
			oEvent->vOLDLength.push_back(96);

			oFlag->castleFlagExtraXPos = 96;

			oPlayer->setMoveDirection(true);

			oEvent->reDrawX.push_back(194);
			oEvent->reDrawY.push_back(2);
			oEvent->reDrawX.push_back(194);
			oEvent->reDrawY.push_back(3);
			oEvent->reDrawX.push_back(195);
			oEvent->reDrawY.push_back(2);
			oEvent->reDrawX.push_back(195);
			oEvent->reDrawY.push_back(3);
			break;
		}
		case 21: {
			oEvent->newLevelType = 4;

			oEvent->iDelay = 1500;
			oEvent->newCurrentLevel = currentLevelID + 1;
			oEvent->inEvent = false;

			oEvent->newMapXPos = 0;
			oEvent->newPlayerXPos = 84;
			oEvent->newPlayerYPos = CCFG::GAME_HEIGHT - 48 - oPlayer->getHitBoxY();
			oEvent->newMoveMap = true;

			oEvent->vOLDDir.push_back(oEvent->eRIGHT);
			oEvent->vOLDLength.push_back(32);

			oFlag->castleFlagExtraXPos = 32;

			oPlayer->setMoveDirection(true);

			oEvent->reDrawX.push_back(307);
			oEvent->reDrawY.push_back(2);
			oEvent->reDrawX.push_back(307);
			oEvent->reDrawY.push_back(3);
			oEvent->reDrawX.push_back(308);
			oEvent->reDrawY.push_back(2);
			oEvent->reDrawX.push_back(308);
			oEvent->reDrawY.push_back(3);
			break;
		}
		case 22: {
			oEvent->newLevelType = 3;

			oEvent->iDelay = 1500;
			oEvent->newCurrentLevel = currentLevelID + 1;
			oEvent->inEvent = false;

			oEvent->newMapXPos = 0;
			oEvent->newPlayerXPos = 84;
			oEvent->newPlayerYPos = 150;
			oEvent->newMoveMap = true;

			oEvent->vOLDDir.push_back(oEvent->eRIGHT);
			oEvent->vOLDLength.push_back(32);

			oFlag->castleFlagExtraXPos = 32;

			oPlayer->setMoveDirection(true);

			oEvent->reDrawX.push_back(174);
			oEvent->reDrawY.push_back(2);
			oEvent->reDrawX.push_back(174);
			oEvent->reDrawY.push_back(3);
			oEvent->reDrawX.push_back(175);
			oEvent->reDrawY.push_back(2);
			oEvent->reDrawX.push_back(175);
			oEvent->reDrawY.push_back(3);
			break;
		}
		case 24: {
			oEvent->newLevelType = 100;

			oEvent->iDelay = 1500;
			oEvent->newCurrentLevel = currentLevelID + 1;
			oEvent->inEvent = true;

			oEvent->newMapXPos = -220*32;
			oEvent->newPlayerXPos = 64;
			oEvent->newPlayerYPos = CCFG::GAME_HEIGHT - 48 - oPlayer->getHitBoxY();
			oEvent->newMoveMap = false;

			oPlayer->setMoveDirection(true);

			oEvent->reDrawX.push_back(185);
			oEvent->reDrawY.push_back(2);
			oEvent->reDrawX.push_back(185);
			oEvent->reDrawY.push_back(3);
			oEvent->reDrawX.push_back(186);
			oEvent->reDrawY.push_back(2);
			oEvent->reDrawX.push_back(186);
			oEvent->reDrawY.push_back(3);
			break;
		}
		case 25: {
			oEvent->newLevelType = 0;

			oEvent->iDelay = 1500;
			oEvent->newCurrentLevel = currentLevelID + 1;
			oEvent->inEvent = false;

			oEvent->newMapXPos = 0;
			oEvent->newPlayerXPos = 84;
			oEvent->newPlayerYPos = CCFG::GAME_HEIGHT - 48 - oPlayer->getHitBoxY();
			oEvent->newMoveMap = true;

			oPlayer->setMoveDirection(true);

			oEvent->vOLDDir.push_back(oEvent->eRIGHT);
			oEvent->vOLDLength.push_back(32);

			oFlag->castleFlagExtraXPos = 32;

			oEvent->reDrawX.push_back(299);
			oEvent->reDrawY.push_back(2);
			oEvent->reDrawX.push_back(299);
			oEvent->reDrawY.push_back(3);
			oEvent->reDrawX.push_back(300);
			oEvent->reDrawY.push_back(2);
			oEvent->reDrawX.push_back(300);
			oEvent->reDrawY.push_back(3);
			break;
		}
		case 26:
			oEvent->newLevelType = 3;

			oEvent->iDelay = 1500;
			oEvent->newCurrentLevel = currentLevelID + 1;
			oEvent->inEvent = false;

			oEvent->newMapXPos = 0;
			oEvent->newPlayerXPos = 84;
			oEvent->newPlayerYPos = 150;
			oEvent->newMoveMap = true;

			oPlayer->setMoveDirection(true);

			oEvent->vOLDDir.push_back(oEvent->eRIGHT);
			oEvent->vOLDLength.push_back(32);

			oFlag->castleFlagExtraXPos = 32;

			oEvent->reDrawX.push_back(232);
			oEvent->reDrawY.push_back(2);
			oEvent->reDrawX.push_back(232);
			oEvent->reDrawY.push_back(3);
			oEvent->reDrawX.push_back(233);
			oEvent->reDrawY.push_back(2);
			oEvent->reDrawX.push_back(233);
			oEvent->reDrawY.push_back(3);
			break;
		case 28: {
			oEvent->newLevelType = 0;

			oEvent->iDelay = 1500;
			oEvent->newCurrentLevel = currentLevelID + 1;
			oEvent->inEvent = false;

			oEvent->newMapXPos = 0;
			oEvent->newPlayerXPos = 84;
			oEvent->newPlayerYPos = CCFG::GAME_HEIGHT - 48 - oPlayer->getHitBoxY();
			oEvent->newMoveMap = true;

			oPlayer->setMoveDirection(true);

			oEvent->reDrawX.push_back(382);
			oEvent->reDrawY.push_back(2);
			oEvent->reDrawX.push_back(382);
			oEvent->reDrawY.push_back(3);
			oEvent->reDrawX.push_back(383);
			oEvent->reDrawY.push_back(2);
			oEvent->reDrawX.push_back(383);
			oEvent->reDrawY.push_back(3);
			break;
		}
		case 29: {
			oEvent->newLevelType = 0;

			oEvent->iDelay = 1500;
			oEvent->newCurrentLevel = currentLevelID + 1;
			oEvent->inEvent = false;

			oEvent->newMapXPos = 0;
			oEvent->newPlayerXPos = 84;
			oEvent->newPlayerYPos = CCFG::GAME_HEIGHT - 48 - oPlayer->getHitBoxY();
			oEvent->newMoveMap = true;

			oPlayer->setMoveDirection(true);

			oEvent->reDrawX.push_back(222);
			oEvent->reDrawY.push_back(2);
			oEvent->reDrawX.push_back(222);
			oEvent->reDrawY.push_back(3);
			oEvent->reDrawX.push_back(223);
			oEvent->reDrawY.push_back(2);
			oEvent->reDrawX.push_back(223);
			oEvent->reDrawY.push_back(3);
			break;
		}
		case 30: {
			oEvent->newLevelType = 3;

			oEvent->iDelay = 1500;
			oEvent->newCurrentLevel = currentLevelID + 1;
			oEvent->inEvent = false;

			oEvent->newMapXPos = 0;
			oEvent->newPlayerXPos = 84;
			oEvent->newPlayerYPos = 150;
			oEvent->newMoveMap = true;

			oPlayer->setMoveDirection(true);

			oEvent->vOLDDir.push_back(oEvent->eRIGHT);
			oEvent->vOLDLength.push_back(128);

			oFlag->castleFlagExtraXPos = 128;

			oEvent->reDrawX.push_back(224);
			oEvent->reDrawY.push_back(2);
			oEvent->reDrawX.push_back(224);
			oEvent->reDrawY.push_back(3);
			oEvent->reDrawX.push_back(225);
			oEvent->reDrawY.push_back(2);
			oEvent->reDrawX.push_back(225);
			oEvent->reDrawY.push_back(3);
			break;
		}
	}

	oEvent->vOLDDir.push_back(oEvent->eENDPOINTS);
	oEvent->vOLDLength.push_back(iMapTime);

	oEvent->vOLDDir.push_back(oEvent->eNOTHING);
	oEvent->vOLDLength.push_back(128);
}

void Map::EndBoss() {
	inEvent = true;

	oEvent->resetData();
	oPlayer->resetJump();
	oPlayer->stopMove();

	oEvent->endGame = false;

	switch(currentLevelID) {
		case 31:
			oEvent->endGame = true;
			break;
		default:
			CCFG::getMusic()->StopMusic();
			CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cCASTLEEND);
			break;
	}

	oEvent->eventTypeID = oEvent->eBossEnd;

	oEvent->iSpeed = 3;

	oEvent->newLevelType = 0;
	oEvent->newCurrentLevel = currentLevelID + 1;
	oEvent->newMoveMap = true;
	oEvent->iDelay = 500;
	oEvent->inEvent = false;

	oEvent->newMapXPos = 0;
	oEvent->newPlayerXPos = 64;
	oEvent->newPlayerYPos = CCFG::GAME_HEIGHT - 48 - oPlayer->getHitBoxY();
	oEvent->newMoveMap = true;

	switch(currentLevelID) {
		case 7:
			oEvent->newLevelType = 4;
			break;
		case 19:
			oEvent->newLevelType = 4;
			break;
	}

	bool addOne = false;

	if(lMap[getBlockIDX((int)(oPlayer->getXPos() + oPlayer->getHitBoxX()/2 - fXPos))][6]->getBlockID() == 82) {
		oEvent->vOLDDir.push_back(oEvent->eBOT);
		oEvent->vOLDLength.push_back(CCFG::GAME_HEIGHT - 16 - 5*32 - (oPlayer->getYPos() + oPlayer->getHitBoxY()));
	} else {
		oEvent->vOLDDir.push_back(oEvent->eBOTRIGHTEND);
		oEvent->vOLDLength.push_back(CCFG::GAME_HEIGHT - 16 - 5*32 - (oPlayer->getYPos() + oPlayer->getHitBoxY()));

		oEvent->vOLDDir.push_back(oEvent->eRIGHT);
		oEvent->vOLDLength.push_back(32 - CCFG::GAME_HEIGHT - 16 - 5*32 - (oPlayer->getYPos() + oPlayer->getHitBoxY()));
		addOne = true;
	}

	oEvent->vOLDDir.push_back(oEvent->eBOSSEND1);
	oEvent->vOLDLength.push_back(1);
	oEvent->vOLDDir.push_back(oEvent->eNOTHING);
	oEvent->vOLDLength.push_back(10);

	oEvent->vOLDDir.push_back(oEvent->eBOSSEND2);
	oEvent->vOLDLength.push_back(1);
	oEvent->vOLDDir.push_back(oEvent->eNOTHING);
	oEvent->vOLDLength.push_back(3);

	for(int i = 0; i < 6; i++) {
		oEvent->vOLDDir.push_back(oEvent->eBOSSEND3);
		oEvent->vOLDLength.push_back(2 + i);
		oEvent->vOLDDir.push_back(oEvent->eNOTHING);
		oEvent->vOLDLength.push_back(3);
	}
	
	oEvent->vOLDDir.push_back(oEvent->eBOSSEND4);
		oEvent->vOLDLength.push_back(1);

	for(int i = 6; i < 12; i++) {
		oEvent->vOLDDir.push_back(oEvent->eBOSSEND3);
		oEvent->vOLDLength.push_back(2 + i);
		oEvent->vOLDDir.push_back(oEvent->eNOTHING);
		oEvent->vOLDLength.push_back(3);
	}

	oEvent->vOLDDir.push_back(oEvent->eNOTHING);
	oEvent->vOLDLength.push_back(90);

	if(currentLevelID == 31) {
		CCFG::getMusic()->StopMusic();
		CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cPRINCESSMUSIC);
	}

	oEvent->vOLDDir.push_back(oEvent->eBOTRIGHTBOSS);
	oEvent->vOLDLength.push_back(8*32);

	switch(currentLevelID) {
		case 31:
			oEvent->vOLDDir.push_back(oEvent->eENDGAMEBOSSTEXT1);
			oEvent->vOLDLength.push_back(getBlockIDX((int)(oPlayer->getXPos() + oPlayer->getHitBoxX()/2 - fXPos))*32 + 7*32 + (addOne ? 32 : 0));
		break;
		default:
			oEvent->vOLDDir.push_back(oEvent->eBOSSTEXT1);
			oEvent->vOLDLength.push_back(getBlockIDX((int)(oPlayer->getXPos() + oPlayer->getHitBoxX()/2 - fXPos))*32 + 7*32 + (addOne ? 32 : 0));
		break;
	}

	oEvent->vOLDDir.push_back(oEvent->eRIGHT);
	oEvent->vOLDLength.push_back(2*32 + 16);

	oEvent->vOLDDir.push_back(oEvent->eMARIOSPRITE1);
	oEvent->vOLDLength.push_back(1);

	oEvent->vOLDDir.push_back(oEvent->eNOTHING);
	oEvent->vOLDLength.push_back(90);

	switch(currentLevelID) {
		case 31:
			oEvent->vOLDDir.push_back(oEvent->eENDGAMEBOSSTEXT2);
			oEvent->vOLDLength.push_back(getBlockIDX((int)(oPlayer->getXPos() + oPlayer->getHitBoxX()/2 - fXPos))*32 + 5*32 + (addOne ? 32 : 0) + 28);
			break;
		default:
			oEvent->vOLDDir.push_back(oEvent->eBOSSTEXT2);
			oEvent->vOLDLength.push_back(getBlockIDX((int)(oPlayer->getXPos() + oPlayer->getHitBoxX()/2 - fXPos))*32 + 5*32 + (addOne ? 32 : 0));
			break;
	}

	oEvent->vOLDDir.push_back(oEvent->eNOTHING);
	oEvent->vOLDLength.push_back(300 + (currentLevelID == 31 ? 100 : 0));

	switch(currentLevelID) {
		case 31:
			oEvent->vOLDDir.push_back(oEvent->eNOTHING);
			oEvent->vOLDLength.push_back(90);
		break;
	}
}

void Map::EndBonus() {
	inEvent = true;

	oEvent->resetData();
	oPlayer->resetJump();
	oPlayer->stopMove();

	oEvent->eventTypeID = oEvent->eNormal;

	oEvent->iSpeed = 3;

	oEvent->newLevelType = iLevelType;
	oEvent->newCurrentLevel = currentLevelID;
	oEvent->newMoveMap = true;
	oEvent->iDelay = 0;
	oEvent->inEvent = false;

	oEvent->newMoveMap = true;

	switch(currentLevelID) {
		case 4: {
			oEvent->newMapXPos = -158*32 + 16;
			oEvent->newPlayerXPos = 128;
			oEvent->newPlayerYPos = -oPlayer->getHitBoxY();
			
			break;
		}
		case 8: {
			oEvent->newMapXPos = -158*32 + 16;
			oEvent->newPlayerXPos = 128;
			oEvent->newPlayerYPos = -oPlayer->getHitBoxY();
			break;
		}
		case 17: {
			oEvent->newMapXPos = -207*32 + 16;
			oEvent->newPlayerXPos = 128;
			oEvent->newPlayerYPos = -oPlayer->getHitBoxY();
			break;
		}
		case 21: {
			oEvent->newMapXPos = -243*32 + 16;
			oEvent->newPlayerXPos = 128;
			oEvent->newPlayerYPos = -oPlayer->getHitBoxY();
			break;
		}
	}

	oEvent->vOLDDir.push_back(oEvent->eNOTHING);
	oEvent->vOLDLength.push_back(1);
}

void Map::playerDeath(bool animation, bool instantDeath) {
	if((oPlayer->getPowerLVL() == 0 && !oPlayer->getUnkillAble()) || instantDeath) {
		inEvent = true;

		oEvent->resetData();
		oPlayer->resetJump();
		oPlayer->stopMove();

		oEvent->iDelay = 150;
		oEvent->newCurrentLevel = currentLevelID;

		oEvent->newMoveMap = bMoveMap;

		oEvent->eventTypeID = oEvent->eNormal;

		oPlayer->resetPowerLVL();

		if(animation) {
			oEvent->iSpeed = 4;
			oEvent->newLevelType = iLevelType;

			oPlayer->setYPos(oPlayer->getYPos() + 4.0f);

			oEvent->vOLDDir.push_back(oEvent->eDEATHNOTHING);
			oEvent->vOLDLength.push_back(30);

			oEvent->vOLDDir.push_back(oEvent->eDEATHTOP);
			oEvent->vOLDLength.push_back(64);

			oEvent->vOLDDir.push_back(oEvent->eDEATHBOT);
			oEvent->vOLDLength.push_back(CCFG::GAME_HEIGHT - oPlayer->getYPos() + 128);
		} else {
			oEvent->iSpeed = 4;
			oEvent->newLevelType = iLevelType;

			oEvent->vOLDDir.push_back(oEvent->eDEATHTOP);
			oEvent->vOLDLength.push_back(1);
		}

		oEvent->vOLDDir.push_back(oEvent->eNOTHING);
		oEvent->vOLDLength.push_back(64);

		if(oPlayer->getNumOfLives() > 1) {
			oEvent->vOLDDir.push_back(oEvent->eLOADINGMENU);
			oEvent->vOLDLength.push_back(90);

			oPlayer->setNumOfLives(oPlayer->getNumOfLives() - 1);

			CCFG::getMusic()->StopMusic();
			CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cDEATH);
		} else {
			oEvent->vOLDDir.push_back(oEvent->eGAMEOVER);
			oEvent->vOLDLength.push_back(90);

			oPlayer->setNumOfLives(oPlayer->getNumOfLives() - 1);

			CCFG::getMusic()->StopMusic();
			CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cDEATH);
		}
	} else if(!oPlayer->getUnkillAble()) {
		oPlayer->setPowerLVL(oPlayer->getPowerLVL() - 1);
	}
}

void Map::startLevelAnimation() {
	oEvent->newUnderWater = false;

	switch(currentLevelID) {
		case 0:

			break;
		case 1:
			oEvent->resetData();
			oPlayer->resetJump();
			oPlayer->stopMove();

			oEvent->iSpeed = 2;
			oEvent->newLevelType = 1;

			oEvent->iDelay = 150;
			oEvent->newCurrentLevel = 1;

			oEvent->newMapXPos = 0;
			oEvent->newPlayerXPos = 96;
			oEvent->newPlayerYPos = 64;
			oEvent->newMoveMap = true;

			oEvent->vOLDDir.push_back(oEvent->eRIGHT);
			oEvent->vOLDLength.push_back(7 * 32 + 4);

			oEvent->vOLDDir.push_back(oEvent->ePLAYPIPERIGHT);
			oEvent->vOLDLength.push_back(1);

			oEvent->vOLDDir.push_back(oEvent->eRIGHT);
			oEvent->vOLDLength.push_back(1 * 32 - 2);

			oEvent->vOLDDir.push_back(oEvent->eNOTHING);
			oEvent->vOLDLength.push_back(75);

			oEvent->reDrawX.push_back(220);
			oEvent->reDrawY.push_back(2);
			oEvent->reDrawX.push_back(220);
			oEvent->reDrawY.push_back(3);
			break;
		case 5:
			oEvent->resetData();
			oPlayer->resetJump();
			oPlayer->stopMove();

			oEvent->iSpeed = 2;
			oEvent->newLevelType = 2;

			oEvent->iDelay = 150;
			oEvent->newCurrentLevel = 5;

			oEvent->newMapXPos = 0;
			oEvent->newPlayerXPos = 96;
			oEvent->newPlayerYPos = 64;
			oEvent->newMoveMap = true;
			oEvent->newUnderWater = true;

			oEvent->vOLDDir.push_back(oEvent->eRIGHT);
			oEvent->vOLDLength.push_back(7 * 32 + 4);

			oEvent->vOLDDir.push_back(oEvent->ePLAYPIPERIGHT);
			oEvent->vOLDLength.push_back(1);

			oEvent->vOLDDir.push_back(oEvent->eRIGHT);
			oEvent->vOLDLength.push_back(1 * 32 - 2);

			oEvent->vOLDDir.push_back(oEvent->eNOTHING);
			oEvent->vOLDLength.push_back(75);

			oEvent->reDrawX.push_back(230);
			oEvent->reDrawY.push_back(2);
			oEvent->reDrawX.push_back(230);
			oEvent->reDrawY.push_back(3);
			break;
		case 13:
			oEvent->resetData();
			oPlayer->resetJump();
			oPlayer->stopMove();

			oEvent->iSpeed = 2;
			oEvent->newLevelType = 1;

			oEvent->iDelay = 150;
			oEvent->newCurrentLevel = currentLevelID;

			oEvent->newMapXPos = 0;
			oEvent->newPlayerXPos = 96;
			oEvent->newPlayerYPos = 64;
			oEvent->newMoveMap = true;

			oEvent->vOLDDir.push_back(oEvent->eRIGHT);
			oEvent->vOLDLength.push_back(7 * 32 + 4);

			oEvent->vOLDDir.push_back(oEvent->ePLAYPIPERIGHT);
			oEvent->vOLDLength.push_back(1);

			oEvent->vOLDDir.push_back(oEvent->eRIGHT);
			oEvent->vOLDLength.push_back(1 * 32 - 2);

			oEvent->vOLDDir.push_back(oEvent->eNOTHING);
			oEvent->vOLDLength.push_back(75);

			oEvent->reDrawX.push_back(250);
			oEvent->reDrawY.push_back(2);
			oEvent->reDrawX.push_back(250);
			oEvent->reDrawY.push_back(3);
			break;
		case 25:
			oEvent->resetData();
			oPlayer->resetJump();
			oPlayer->stopMove();

			oEvent->iSpeed = 2;
			oEvent->newLevelType = 2;

			oEvent->iDelay = 150;
			oEvent->newCurrentLevel = 25;

			oEvent->newMapXPos = 0;
			oEvent->newPlayerXPos = 96;
			oEvent->newPlayerYPos = 64;
			oEvent->newMoveMap = true;
			oEvent->newUnderWater = true;

			oEvent->vOLDDir.push_back(oEvent->eRIGHT);
			oEvent->vOLDLength.push_back(7 * 32 + 4);

			oEvent->vOLDDir.push_back(oEvent->ePLAYPIPERIGHT);
			oEvent->vOLDLength.push_back(1);

			oEvent->vOLDDir.push_back(oEvent->eRIGHT);
			oEvent->vOLDLength.push_back(1 * 32 - 2);

			oEvent->vOLDDir.push_back(oEvent->eNOTHING);
			oEvent->vOLDLength.push_back(75);

			oEvent->reDrawX.push_back(230);
			oEvent->reDrawY.push_back(2);
			oEvent->reDrawX.push_back(230);
			oEvent->reDrawY.push_back(3);
			break;
		case 26:
			oEvent->resetData();
			oPlayer->resetJump();
			oPlayer->stopMove();

			oEvent->iSpeed = 2;
			oEvent->newLevelType = 2;

			oEvent->iDelay = 150;
			oEvent->newCurrentLevel = 26;

			oEvent->newMapXPos = 0;
			oEvent->newPlayerXPos = 96;
			oEvent->newPlayerYPos = 64;
			oEvent->newMoveMap = true;
			oEvent->newUnderWater = true;

			oEvent->vOLDDir.push_back(oEvent->eRIGHT);
			oEvent->vOLDLength.push_back(7 * 32 + 4);

			oEvent->vOLDDir.push_back(oEvent->ePLAYPIPERIGHT);
			oEvent->vOLDLength.push_back(1);

			oEvent->vOLDDir.push_back(oEvent->eRIGHT);
			oEvent->vOLDLength.push_back(1 * 32 - 2);

			oEvent->vOLDDir.push_back(oEvent->eNOTHING);
			oEvent->vOLDLength.push_back(75);

			oEvent->reDrawX.push_back(230);
			oEvent->reDrawY.push_back(2);
			oEvent->reDrawX.push_back(230);
			oEvent->reDrawY.push_back(3);
			break;
	}
}

void Map::spawnVine(int nX, int nY, int iBlockID) {
	if(iLevelType == 0 || iLevelType == 4) {
		addVine(nX, nY, 0, 34);
	} else {
		addVine(nX, nY, 0, 36);
	}
}

void Map::TPUse() {
	switch(currentLevelID) {
		case 27:
			if(bTP) {
				if(!(-fXPos > 90*32)) {
					fXPos -= 64*32;
					bTP = false;
				}
			} else {
				if(!(-fXPos < 90*32)) {
					fXPos += 64*32;
				}
			}
			break;
		case 31:
			fXPos += 75*32;
			break;
	}
}

void Map::TPUse2() {
	switch(currentLevelID) {
		case 27:
			fXPos += 64*32;
			bTP = false;
			break;
		case 31:
			fXPos += 12*32;
			break;
	}
}

void Map::TPUse3() {
	switch(currentLevelID) {
		case 27:
			bTP = true;
			break;
		case 31:
			fXPos += 79*32;
			break;
	}
}


/* ******************************************** */

void Map::structBush(int X, int Y, int iSize) {
	// ----- LEFT & RIGHT
	for(int i = 0; i < iSize; i++) {
		lMap[X + i][Y + i]->setBlockID(5);
		lMap[X + iSize + 1 + i][Y + iSize - 1 - i]->setBlockID(6);
	}
	
	// ----- CENTER LEFT & RIGHT
	for(int i = 0, k = 1; i < iSize - 1; i++) {
		for(int j = 0; j < k; j++) {
			lMap[X + 1 + i][Y + j]->setBlockID((i%2 == 0 ? 3 : 4));
			lMap[X + iSize * 2 - 1 - i][Y + j]->setBlockID((i%2 == 0 ? 3 : 4));
		}
		++k;
	}

	// ----- CENTER
	for(int i = 0; i < iSize; i++) {
		lMap[X + iSize][Y + i]->setBlockID((i%2 == 0 && iSize != 1 ? 4 : 3));
	}

	// ----- TOP
	lMap[X + iSize][Y + iSize]->setBlockID(7);
}

void Map::structGrass(int X, int Y, int iSize) {
	lMap[X][Y]->setBlockID(10);
	for(int i = 0; i < iSize; i++) {
		lMap[X + 1 + i][Y]->setBlockID(11);
	}
	lMap[X + iSize + 1][Y]->setBlockID(12);
}

void Map::structCloud(int X, int Y, int iSize) {
	// ----- LEFT
	lMap[X][Y]->setBlockID(iLevelType == 3 ? 148 : 14);
	lMap[X][Y + 1]->setBlockID(15);
	
	for(int i = 0; i < iSize; i++) {
		lMap[X + 1 + i][Y]->setBlockID(iLevelType == 3 ? 149 : 16);
		lMap[X + 1 + i][Y + 1]->setBlockID(iLevelType == 3 ? 150 : 17);
	}

	lMap[X + iSize + 1][Y]->setBlockID(18);
	lMap[X + iSize + 1][Y + 1]->setBlockID(19);
}

void Map::structGND(int X, int Y, int iWidth, int iHeight) {
	for(int i = 0; i < iWidth; i++) {
		for(int j = 0; j < iHeight; j++) {
			lMap[X + i][Y + j]->setBlockID(iLevelType == 0 || iLevelType == 4 ? 1 : iLevelType == 1 ? 26 : iLevelType == 2 ? 92 : iLevelType == 6 ? 166 : iLevelType == 7 ? 181 : 75);
		}
	}
}

void Map::structBonus(int X, int Y, int iWidth) {
	for(int i = 0; i < iWidth; i++) {
		lMap[X + i][Y]->setBlockID(125);
	}
}

void Map::structBonusEnd(int X) {
	for(int i = 0; i < 20; i++) {
		lMap[X + i][0]->setBlockID(127);
	}
}

void Map::structUW1(int X, int Y, int iWidth, int iHeight) {
	for(int i = 0; i < iWidth; i++) {
		for(int j = 0; j < iHeight; j++) {
			lMap[X + i][Y + j]->setBlockID(93);
		}
	}
}

// ----- true = LEFT, false = RIGHT -----
void Map::structGND2(int X, int Y, int iSize, bool bDir) {
	if(bDir) {
		for(int i = 0, k = 1; i < iSize; i++) {
			for(int j = 0; j < k; j++) {
				lMap[X + i][Y + j]->setBlockID(iLevelType == 0 || iLevelType == 4 ? 25 : iLevelType == 3 ? 167 : 27);
			}
			++k;
		}
	} else {
		for(int i = 0, k = 1; i < iSize; i++) {
			for(int j = 0; j < k; j++) {
				lMap[X + iSize - 1 - i][Y + j]->setBlockID(iLevelType == 0 || iLevelType == 4 ? 25 : iLevelType == 3 ? 167 : 27);
			}
			++k;
		}
	}
}

void Map::structGND2(int X, int Y, int iWidth, int iHeight) {
	for(int i = 0; i < iWidth; i++) {
		for(int j = 0; j < iHeight; j++) {
			lMap[X + i][Y + j]->setBlockID(iLevelType == 0 || iLevelType == 4 ? 25 : iLevelType == 3 ? 167 : 27);
		}
	}
}

void Map::structPipe(int X, int Y, int iHeight) {
	for(int i = 0; i < iHeight; i++) {
		lMap[X][Y + i]->setBlockID(iLevelType == 0 ? 20 : iLevelType == 2 ? 97 : iLevelType == 4 ? 112 : iLevelType == 5 ? 136 : iLevelType == 3 ? 176 : iLevelType == 7 ? 172 : 30);
		lMap[X + 1][Y + i]->setBlockID(iLevelType == 0 ? 22 : iLevelType == 2 ? 99 : iLevelType == 4 ? 114 : iLevelType == 5 ? 138 : iLevelType == 3 ? 178 : iLevelType == 7 ? 174 : 32);
	}

	lMap[X][Y + iHeight]->setBlockID(iLevelType == 0 ? 21 : iLevelType == 2 ? 98 : iLevelType == 4 ? 113 : iLevelType == 5 ? 137 : iLevelType == 3 ? 177 : iLevelType == 7 ? 173 : 31);
	lMap[X + 1][Y + iHeight]->setBlockID(iLevelType == 0 ? 23 : iLevelType == 2 ? 100 : iLevelType == 4 ? 115 : iLevelType == 5 ? 139 : iLevelType == 3 ? 179 : iLevelType == 7 ? 175 : 33);
}

void Map::structPipeVertical(int X, int Y, int iHeight) {
	for(int i = 0; i < iHeight + 1; i++) {
		lMap[X][Y + i]->setBlockID(iLevelType == 0 ? 20 : iLevelType == 2 ? 97 : iLevelType == 4 ? 112 : 30);
		lMap[X + 1][Y + i]->setBlockID(iLevelType == 0 ? 22 : iLevelType == 2 ? 99 : iLevelType == 4 ? 114 : 32);
	}
}

void Map::structPipeHorizontal(int X, int Y, int iWidth) {
	lMap[X][Y]->setBlockID(iLevelType == 0 ? 62 : iLevelType == 2 ? 105 : iLevelType == 4 ? 120 : 38);
	lMap[X][Y + 1]->setBlockID(iLevelType == 0 ? 60 : iLevelType == 2 ? 103 : iLevelType == 4 ? 118 : 36);

	for(int i = 0 ; i < iWidth; i++) {
		lMap[X + 1 + i][Y]->setBlockID(iLevelType == 0 ? 61 : iLevelType == 2 ? 104 : iLevelType == 4 ? 119 : 37);
		lMap[X + 1 + i][Y + 1]->setBlockID(iLevelType == 0 ? 59 : iLevelType == 2 ? 102 : iLevelType == 4 ? 117 : 35);
	}
	
	lMap[X + 1 + iWidth][Y]->setBlockID(iLevelType == 0 ? 58 : iLevelType == 2 ? 101 : iLevelType == 4 ? 116 : 34);
	lMap[X + 1 + iWidth][Y + 1]->setBlockID(iLevelType == 0 ? 63 : iLevelType == 2 ? 106 : iLevelType == 4 ? 121 : 39);
}

void Map::structBrick(int X, int Y, int iWidth, int iHeight) {
	for(int i = 0; i < iWidth; i++) {
		for(int j = 0; j < iHeight; j++) {
			lMap[X + i][Y + j]->setBlockID(iLevelType == 0 || iLevelType == 4 ? 13 : iLevelType == 3 ? 81 : 28);
		}
	}
}

void Map::struckBlockQ(int X, int Y, int iWidth) {
	for(int i = 0; i < iWidth; i++) {
		lMap[X + i][Y]->setBlockID(iLevelType == 0 || iLevelType == 4 ? 8 : 55);
	}
}

void Map::struckBlockQ2(int X, int Y, int iWidth) {
	for(int i = 0; i < iWidth; i++) {
		lMap[X + i][Y]->setBlockID(24);
	}
}

void Map::structCoins(int X, int Y, int iWidth, int iHeight) {
	for(int i = 0; i < iWidth; i++) {
		for(int j = 0; j < iHeight; j++) {
			lMap[X + i][Y + j]->setBlockID(iLevelType == 0 || iLevelType == 4 ? 71 : iLevelType == 1 ? 29 : iLevelType == 2 ? 73 : 29);
		}
	}
}

void Map::structEnd(int X, int Y, int iHeight) {
	for(int i = 0; i < iHeight; i++) {
		lMap[X][Y + i]->setBlockID(iLevelType == 4 ? 123 : 40);
	}

	oFlag = new Flag(X*32 - 16, Y + iHeight + 72);

	lMap[X][Y + iHeight]->setBlockID(iLevelType == 4 ? 124 : 41);

	for(int i = Y + iHeight + 1; i < Y + iHeight + 4; i++) {
		lMap[X][i]->setBlockID(182);
	}
}

void Map::structCastleSmall(int X, int Y) {
	for(int i = 0; i < 2; i++){
		lMap[X][Y + i]->setBlockID(iLevelType == 3 ? 155 : 43);
		lMap[X + 1][Y + i]->setBlockID(iLevelType == 3 ? 155 : 43);
		lMap[X + 3][Y + i]->setBlockID(iLevelType == 3 ? 155 : 43);
		lMap[X + 4][Y + i]->setBlockID(iLevelType == 3 ? 155 : 43);

		lMap[X + 2][Y + i]->setBlockID(iLevelType == 3 ? 159 : 47);
	}

	lMap[X + 2][Y + 1]->setBlockID(iLevelType == 3 ? 158 : 46);

	for(int i = 0; i < 5; i++) {
		lMap[X + i][Y + 2]->setBlockID(i == 0 || i == 4 ? iLevelType == 3 ? 157 : 45 : iLevelType == 3 ? 156 : 44);
	}

	lMap[X + 1][Y + 3]->setBlockID(iLevelType == 3 ? 160 : 48);
	lMap[X + 2][Y + 3]->setBlockID(iLevelType == 3 ? 155 : 43);
	lMap[X + 3][Y + 3]->setBlockID(iLevelType == 3 ? 161 : 49);

	for(int i = 0; i < 3; i++) {
		lMap[X + i + 1][Y + 4]->setBlockID(iLevelType == 3 ? 157 : 45);
	}
}

void Map::structCastleBig(int X, int Y) {
	for(int i = 0; i < 2; i++) {
		for(int j = 0; j < 5; j++) {
			setBlockID(X + i, Y + j, iLevelType == 3 ? 155 : 43);
			setBlockID(X + i + 7, Y + j, iLevelType == 3 ? 155 : 43);
		}
	}

	for(int i = 0; i < 3; i++) {
		setBlockID(X + 2 + i*2, Y, iLevelType == 3 ? 159 : 47);
		setBlockID(X + 2 + i*2, Y + 1, iLevelType == 3 ? 158 : 46);
	}

	for(int i = 0; i < 9; i++) {
		setBlockID(X + i, Y + 2, iLevelType == 3 ? 155 : 43);
	}

	for(int i = 0; i < 9; i++) {
		if(i < 2 || i > 6) {
			setBlockID(X + i, Y + 5, iLevelType == 3 ? 157 : 45);
		} else {
			setBlockID(X + i, Y + 5, iLevelType == 3 ? 156 : 44);
		}
	}

	
	for(int i = 0; i < 2; i++) {
		setBlockID(X + 3 + i*2, Y, iLevelType == 3 ? 155 : 43);
		setBlockID(X + 3 + i*2, Y + 1, iLevelType == 3 ? 155 : 43);
	}
	
	for(int i = 0; i < 2; i++) {
		setBlockID(X + 3 + i*2, Y + 3, iLevelType == 3 ? 159 : 47);
		setBlockID(X + 3 + i*2, Y + 4, iLevelType == 3 ? 158 : 46);
	}

	for(int i = 0; i < 3; i++) {
		setBlockID(X + 2 + i*2, Y + 3, iLevelType == 3 ? 155 : 43);
		setBlockID(X + 2 + i*2, Y + 4, iLevelType == 3 ? 155 : 43);
	}

	for(int i = 0; i < 2; i++) {
		setBlockID(X + 2, Y + 6 + i, iLevelType == 3 ? 155 : 43);
		setBlockID(X + 3, Y + 6 + i, iLevelType == 3 ? 155 : 43);
		setBlockID(X + 5, Y + 6 + i, iLevelType == 3 ? 155 : 43);
		setBlockID(X + 6, Y + 6 + i, iLevelType == 3 ? 155 : 43);
	}

	setBlockID(X + 4, Y + 6, iLevelType == 3 ? 159 : 47);
	setBlockID(X + 4, Y + 7, iLevelType == 3 ? 158 : 46);

	for(int i = 0; i < 3; i++) {
		setBlockID(X + 3 + i, Y + 8, iLevelType == 3 ? 156 : 44);
	}

	setBlockID(X + 2, Y + 8, iLevelType == 3 ? 157 : 45);
	setBlockID(X + 6, Y + 8, iLevelType == 3 ? 157 : 45);

	setBlockID(X + 2, Y + 8, iLevelType == 3 ? 157 : 45);

	setBlockID(X + 3, Y + 9, iLevelType == 3 ? 160 : 48);
	setBlockID(X + 4, Y + 9, iLevelType == 3 ? 155 : 43);
	setBlockID(X + 5, Y + 9, iLevelType == 3 ? 161 : 49);

	for(int i = 0; i < 3; i++) {
		setBlockID(X + 3 + i, Y + 10, iLevelType == 3 ? 157 : 45);
	}
}

void Map::structCastleWall(int X, int Y, int iWidth, int iHeight) {
	for(int i = 0; i < iWidth; i++) {
		for(int j = 0; j  < iHeight - 1; j++) {
			lMap[X + i][Y + j]->setBlockID(iLevelType == 3 ? 155 : 43);
		}
	}

	for(int i = 0; i < iWidth; i++) {
		lMap[X + i][Y + iHeight - 1]->setBlockID(iLevelType == 3 ? 157 : 45);
	}
}

void Map::structT(int X, int Y, int iWidth, int iHeight) {
	for(int i = 0; i < iHeight - 1; i++) {
		for(int j = 1; j < iWidth - 1; j++) {
			lMap[X + j][Y + i]->setBlockID(iLevelType == 3 ? 154 : 70);
		}
	}

	for(int i = 1; i < iWidth - 1; i++) {
		lMap[X + i][Y + iHeight - 1]->setBlockID(iLevelType == 3 ? 152 : 68);
	}

	lMap[X][Y + iHeight - 1]->setBlockID(iLevelType == 3 ? 151 : 67);
	lMap[X + iWidth - 1][Y + iHeight - 1]->setBlockID(iLevelType == 3 ? 153 : 69);
}

void Map::structTMush(int X, int Y, int iWidth, int iHeight) {
	for(int i = 0; i < iHeight - 2; i++) {
		lMap[X + iWidth/2][Y + i]->setBlockID(144);
	}

	lMap[X + iWidth/2][Y + iHeight - 2]->setBlockID(143);

	for(int i = 1; i < iWidth - 1; i++) {
		lMap[X + i][Y + iHeight - 1]->setBlockID(141);
	}

	lMap[X][Y + iHeight - 1]->setBlockID(140);
	lMap[X + iWidth - 1][Y + iHeight - 1]->setBlockID(142);
}

void Map::structWater(int X, int Y, int iWidth, int iHeight) {
	for(int i = 0; i < iWidth; i++) {
		for(int j = 0; j < iHeight - 1; j++) {
			lMap[X + i][Y + j]->setBlockID(iLevelType == 2 ? 94 : 110);
		}
		lMap[X + i][Y + iHeight - 1]->setBlockID(iLevelType == 2 ? 95 : 111);
	}
}

void Map::structLava(int X, int Y, int iWidth, int iHeight) {
	for(int i = 0; i < iWidth; i++) {
		for(int j = 0; j < iHeight - 1; j++) {
			lMap[X + i][Y + j]->setBlockID(77);
		}
		lMap[X + i][Y + iHeight - 1]->setBlockID(78);
	}
}

void Map::structBridge(int X, int Y, int iWidth) {
	for(int i = 0; i < iWidth; i++) {
		lMap[X + i][Y]->setBlockID(76);
	}

	lMap[X + iWidth - 1][Y + 1]->setBlockID(79);

	lMap[X + iWidth][6]->setBlockID(82);
	lMap[X + iWidth + 1][6]->setBlockID(83);
	lMap[X + iWidth + 1][7]->setBlockID(83);
	lMap[X + iWidth + 1][8]->setBlockID(83);
}

void Map::structBridge2(int X, int Y, int iWidth) {
	for(int i = 0; i < iWidth; i++) {
		lMap[X + i][Y]->setBlockID(107);
		lMap[X + i][Y + 1]->setBlockID(iLevelType == 4 ? 122 : 108);
	}
}

void Map::structTree(int X, int Y, int iHeight, bool BIG) {
	for(int i = 0; i < iHeight; i++) {
		lMap[X][Y + i]->setBlockID(91);
	}

	if(BIG) {
		lMap[X][Y + iHeight]->setBlockID(iLevelType == 4 ? 88 : 85);
		lMap[X][Y + iHeight + 1]->setBlockID(iLevelType == 4 ? 89 : 86);
	} else {
		lMap[X][Y + iHeight]->setBlockID(iLevelType == 4 ? 87 : 84);
	}
}

void Map::structFence(int X, int Y, int iWidth) {
	for(int i = 0; i < iWidth; i++) {
		lMap[X + i][Y]->setBlockID(90);
	}
}

void Map::structPlatformLine(int X) {
	for(int i = 0; i < iMapHeight; i++) {
		lMap[X][i]->setBlockID(109);
	}
}

void Map::structSeeSaw(int X, int Y, int iWidth) {
	lMap[X][Y]->setBlockID(iLevelType == 3 ? 162 : 132);
	lMap[X + iWidth - 1][Y]->setBlockID(iLevelType == 3 ? 163 : 133);

	for(int i = 1; i < iWidth - 1; i++) {
		lMap[X + i][Y]->setBlockID(iLevelType == 3 ? 164 : 134);
	}
}

void Map::structBulletBill(int X, int Y, int iHieght) {
	lMap[X][Y + iHieght + 1]->setBlockID(145);
	lMap[X][Y + iHieght]->setBlockID(146);

	for(int i = 0; i < iHieght; i++) {
		lMap[X][Y + i]->setBlockID(147);
	}

	addBulletBillSpawner(X, Y + iHieght + 1, 0);
}

/* ******************************************** */

void Map::setBlockID(int X, int Y, int iBlockID) {
	if(X >= 0 && X < iMapWidth) {
		lMap[X][Y]->setBlockID(iBlockID);
	}
}

/* ******************************************** */

Player* Map::getPlayer() {
	return oPlayer;
}

Platform* Map::getPlatform(int iID) {
	return vPlatform[iID];
}

float Map::getXPos() {
	return fXPos;
}

void Map::setXPos(float iXPos) {
	this->fXPos = iXPos;
}

float Map::getYPos() {
	return fYPos;
}

void Map::setYPos(float iYPos) {
	this->fYPos = iYPos;
}

int Map::getLevelType() {
	return iLevelType;
}

void Map::setLevelType(int iLevelType) {
	this->iLevelType = iLevelType;
}

int Map::getCurrentLevelID() {
	return currentLevelID;
}

void Map::setCurrentLevelID(int currentLevelID) {
	if(this->currentLevelID != currentLevelID) {
		this->currentLevelID = currentLevelID;
		oEvent->resetRedraw();
		loadLVL();
		iSpawnPointID = 0;
	}

	this->currentLevelID = currentLevelID;
}

bool Map::getUnderWater() {
	return underWater;
}

void Map::setUnderWater(bool underWater) {
	this->underWater = underWater;
}

void Map::setSpawnPointID(int iSpawnPointID) {
	this->iSpawnPointID = iSpawnPointID;
}

int Map::getMapTime() {
	return iMapTime;
}

void Map::setMapTime(int iMapTime) {
	this->iMapTime = iMapTime;
}

int Map::getMapWidth() {
	return iMapWidth;
}

bool Map::getMoveMap() {
	return bMoveMap;
}

void Map::setMoveMap(bool bMoveMap) {
	this->bMoveMap = bMoveMap;
}

bool Map::getDrawLines() {
	return drawLines;
}

void Map::setDrawLines(bool drawLines) {
	this->drawLines = drawLines;
}

/* ******************************************** */

Event* Map::getEvent() {
	return oEvent;
}

bool Map::getInEvent() {
	return inEvent;
}

void Map::setInEvent(bool inEvent) {
	this->inEvent = inEvent;
}

/* ******************************************** */

Block* Map::getBlock(int iID) {
	return vBlock[iID];
}

Block* Map::getMinionBlock(int iID) {
	return vMinion[iID];
}

MapLevel* Map::getMapBlock(int iX, int iY) {
	return lMap[iX][iY];
}

Flag* Map::getFlag() {
	return oFlag;
}