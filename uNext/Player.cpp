#include "Player.h"
#include "Core.h"
#include "stdlib.h"
#include "time.h"

/* ******************************************** */

Player::Player(SDL_Renderer* rR, float fXPos, float fYPos) {
	this->fXPos = fXPos;
	this->fYPos = fYPos;
	this->iNumOfLives = 3;

	this->iSpriteID = 1;

	this->iScore = this->iCoins = 0;
	this->iFrameID = 0, this->iComboPoints = 1;

	this->nextBubbleTime = 0;
	this->nextFallFrameID = 0;

	this->powerLVL = 0;
	this->inLevelAnimation = false;
	this->inLevelAnimationType = false;

	this->unKillAble = false;
	this->starEffect = false;
	this->unKillAbleTimeFrameID = 0;

	this->inLevelDownAnimation = false;
	this->inLevelDownAnimationFrameID = 0;

	this->moveDirection = true;
	this->currentMaxMove = 4;
	this->moveSpeed = 0;
	this->bMove = false;
	this->changeMoveDirection = false;
	this->bSquat = false;

	this->onPlatformID = -1;

	this->springJump = false;

	this->iTimePassed = SDL_GetTicks();

	this->jumpState = 0;
	this->startJumpSpeed = 7.65f;
	this->currentFallingSpeed = 2.7f;

	this->iMoveAnimationTime = 0;

	this->nextFireBallFrameID = 8;

	// ----- LOAD SPRITE
	std::vector<std::string> tempS;
	std::vector<unsigned int> tempI;

	srand((unsigned)time(NULL));

	// ----- 0
	tempS.push_back("mario/mario_death");
	tempI.push_back(0);
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();

	// ----- 1
	tempS.push_back("mario/mario");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
	// ----- 2
	tempS.push_back("mario/mario_move0");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
	// ----- 3
	tempS.push_back("mario/mario_move1");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
	// ----- 4
	tempS.push_back("mario/mario_move2");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
	// ----- 5
	tempS.push_back("mario/mario_jump");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
	// ----- 6
	tempS.push_back("mario/mario_st");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
	// ----- 7
	tempS.push_back("mario/mario"); // SQUAT
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
	// ----- 8
	tempS.push_back("mario/mario_underwater0");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
	// ----- 9
	tempS.push_back("mario/mario_underwater1");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
	// ----- 10
	tempS.push_back("mario/mario_end");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
	// ----- 11
	tempS.push_back("mario/mario_end1");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();

	// ---------- BIG
	// ----- 12
	tempS.push_back("mario/mario1");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
	// ----- 13
	tempS.push_back("mario/mario1_move0");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
	// ----- 14
	tempS.push_back("mario/mario1_move1");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
	// ----- 15
	tempS.push_back("mario/mario1_move2");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
	// ----- 16
	tempS.push_back("mario/mario1_jump");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
	// ----- 17
	tempS.push_back("mario/mario1_st");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
	// ----- 18
	tempS.push_back("mario/mario1_squat"); // SQUAT
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
	// ----- 19
	tempS.push_back("mario/mario1_underwater0");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
	// ----- 20
	tempS.push_back("mario/mario1_underwater1");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
	// ----- 21
	tempS.push_back("mario/mario1_end");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
	// ----- 22
	tempS.push_back("mario/mario1_end1");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();

	// ----- 23
	tempS.push_back("mario/mario2");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
	// ----- 24
	tempS.push_back("mario/mario2_move0");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
	// ----- 25
	tempS.push_back("mario/mario2_move1");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
	// ----- 26
	tempS.push_back("mario/mario2_move2");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
	// ----- 27
	tempS.push_back("mario/mario2_jump");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
	// ----- 28
	tempS.push_back("mario/mario2_st");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
	// ----- 29
	tempS.push_back("mario/mario2_squat"); // SQUAT
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
	// ----- 30
	tempS.push_back("mario/mario2_underwater0");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
	// ----- 31
	tempS.push_back("mario/mario2_underwater1");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
	// ----- 32
	tempS.push_back("mario/mario2_end");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
	// ----- 33
	tempS.push_back("mario/mario2_end1");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();

	// ----- 34
	tempS.push_back("mario/mario2s");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
	// ----- 35
	tempS.push_back("mario/mario2s_move0");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
	// ----- 36
	tempS.push_back("mario/mario2s_move1");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
	// ----- 37
	tempS.push_back("mario/mario2s_move2");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
	// ----- 38
	tempS.push_back("mario/mario2s_jump");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
	// ----- 39
	tempS.push_back("mario/mario2s_st");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
	// ----- 40
	tempS.push_back("mario/mario2s_squat"); // SQUAT
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
	// ----- 41
	tempS.push_back("mario/mario2s_underwater0");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
	// ----- 42
	tempS.push_back("mario/mario2s_underwater1");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
	// ----- 43
	tempS.push_back("mario/mario2s_end");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
	// ----- 44
	tempS.push_back("mario/mario2s_end1");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();

	// ----- 45
	tempS.push_back("mario/mario_s0");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
	// ----- 46
	tempS.push_back("mario/mario_s0_move0");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
	// ----- 47
	tempS.push_back("mario/mario_s0_move1");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
	// ----- 48
	tempS.push_back("mario/mario_s0_move2");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
	// ----- 49
	tempS.push_back("mario/mario_s0_jump");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
	// ----- 50
	tempS.push_back("mario/mario_s0_st");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
	// ----- 51
	tempS.push_back("mario/mario_s0"); // SQUAT
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
	// ----- 52
	tempS.push_back("mario/mario_s0_underwater0");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
	// ----- 53
	tempS.push_back("mario/mario_s0_underwater1");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
	// ----- 54
	tempS.push_back("mario/mario_s0_end");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
	// ----- 55
	tempS.push_back("mario/mario_s0_end1");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();

	// ----- 56
	tempS.push_back("mario/mario_s1");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
	// ----- 57
	tempS.push_back("mario/mario_s1_move0");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
	// ----- 58
	tempS.push_back("mario/mario_s1_move1");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
	// ----- 59
	tempS.push_back("mario/mario_s1_move2");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
	// ----- 60
	tempS.push_back("mario/mario_s1_jump");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
	// ----- 61
	tempS.push_back("mario/mario_s1_st");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
	// ----- 62
	tempS.push_back("mario/mario_s1"); // SQUAT
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
	// ----- 63
	tempS.push_back("mario/mario_s1_underwater0");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
	// ----- 64
	tempS.push_back("mario/mario_s1_underwater1");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
	// ----- 65
	tempS.push_back("mario/mario_s1_end");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
	// ----- 66
	tempS.push_back("mario/mario_s1_end1");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();

	// ----- LOAD SPRITE

	// ----- 67
	tempS.push_back("mario/mario_lvlup");
	sMario.push_back(new Sprite(rR,  tempS, tempI, true));
	tempS.clear();
}

Player::~Player(void) {
	for(std::vector<Sprite*>::iterator i = sMario.begin(); i != sMario.end(); i++) {
		delete (*i);
	}

	delete tMarioLVLUP;
}

/* ******************************************** */

void Player::Update() {
	playerPhysics();
	movePlayer();

	if(iFrameID > 0) {
		--iFrameID;
	} else if(iComboPoints > 1) {
		--iComboPoints;
	}

	if(powerLVL == 2) {
		if(nextFireBallFrameID > 0) {
			--nextFireBallFrameID;
		}
	}

	if(inLevelDownAnimation) {
		if(inLevelDownAnimationFrameID > 0) {
			--inLevelDownAnimationFrameID;
		} else {
			unKillAble = false;
		}
	}
}

void Player::playerPhysics() {
	if(!CCore::getMap()->getUnderWater()) {
		if (jumpState == 1) {
			updateYPos(-(int)currentJumpSpeed);
			currentJumpDistance += (int)currentJumpSpeed;

			currentJumpSpeed *= (currentJumpDistance / jumpDistance > 0.75f ? 0.972f : 0.986f);
		
			if (currentJumpSpeed < 2.5f) {
				currentJumpSpeed = 2.5f;
			}

			if(!CCFG::keySpace && currentJumpDistance > 64 && !springJump) {
				jumpDistance = 16;
				currentJumpDistance = 0;
				currentJumpSpeed = 2.5f;
			}

			if (jumpDistance <= currentJumpDistance) {
				jumpState = 2;
			}
		} else {
			if(onPlatformID == -1) {
				onPlatformID = CCore::getMap()->checkCollisionWithPlatform((int)fXPos, (int)fYPos, getHitBoxX(), getHitBoxY());
				if(onPlatformID >= 0) {
					if (CCore::getMap()->checkCollisionLB((int)(fXPos - CCore::getMap()->getXPos() + 2), (int)fYPos + 2, getHitBoxY(), true) || CCore::getMap()->checkCollisionRB((int)(fXPos - CCore::getMap()->getXPos() - 2), (int)fYPos + 2, getHitBoxX(), getHitBoxY(), true)) {
						onPlatformID = -1;
						resetJump();
					} else {
						fYPos = (float)CCore::getMap()->getPlatform(onPlatformID)->getYPos() - getHitBoxY();
						resetJump();
						CCore::getMap()->getPlatform(onPlatformID)->turnON();
					}
				}
			} else {
				onPlatformID = CCore::getMap()->checkCollisionWithPlatform((int)fXPos, (int)fYPos, getHitBoxX(), getHitBoxY());
			}
		
			if(onPlatformID >= 0) {
				if (CCore::getMap()->checkCollisionLB((int)(fXPos - CCore::getMap()->getXPos() + 2), (int)fYPos + 2, getHitBoxY(), true) || CCore::getMap()->checkCollisionRB((int)(fXPos - CCore::getMap()->getXPos() - 2), (int)fYPos + 2, getHitBoxX(), getHitBoxY(), true)) {
					onPlatformID = -1;
					resetJump();
				} else {
					fYPos += CCore::getMap()->getPlatform(onPlatformID)->getMoveY();
					CCore::getMap()->getPlatform(onPlatformID)->moveY();
					//if(moveSpeed == 0)
					CCore::getMap()->setXPos(CCore::getMap()->getXPos() - CCore::getMap()->getPlatform(onPlatformID)->getMoveX());
			
					jumpState = 0;
				}
			}
			else if (!CCore::getMap()->checkCollisionLB((int)(fXPos - CCore::getMap()->getXPos() + 2), (int)fYPos + 2, getHitBoxY(), true) &&
				!CCore::getMap()->checkCollisionRB((int)(fXPos - CCore::getMap()->getXPos() - 2), (int)fYPos + 2, getHitBoxX(), getHitBoxY(), true)) {

				if(nextFallFrameID > 0) {
					--nextFallFrameID;
				} else {
					currentFallingSpeed *= 1.05f;

					if (currentFallingSpeed > startJumpSpeed) {
						currentFallingSpeed = startJumpSpeed;
					}

					updateYPos((int)currentFallingSpeed);
				}
				

				jumpState = 2;

				setMarioSpriteID(5);
			} else if(jumpState == 2) {
				resetJump();
			} else {
				checkCollisionBot(0, 0);
			}
		}
	} else {
		if(nextBubbleTime + 685 < SDL_GetTicks()) {
			CCore::getMap()->addBubble((int)(fXPos - CCore::getMap()->getXPos() + (moveDirection ? getHitBoxX() - rand()%8 : rand()%8)), (int)fYPos + 4);
			nextBubbleTime = SDL_GetTicks() + rand()%715;
		}

		if (jumpState == 1) {
			if(fYPos > CCFG::GAME_HEIGHT - 12*32 - 16) {
				updateYPos(-2);
				currentJumpDistance += 2;

				swimingAnimation();

				if (jumpDistance <= currentJumpDistance) {
					jumpState = 2;
					currentJumpDistance = 0;
					nextFallFrameID = 4;
				}
			} else {
				jumpState = 2;
				nextFallFrameID = 14;
				currentJumpDistance = 0;
			}
		} else {
			if (!CCore::getMap()->checkCollisionLB((int)(fXPos - CCore::getMap()->getXPos() + 2), (int)fYPos + 2, getHitBoxY(), true) &&
				!CCore::getMap()->checkCollisionRB((int)(fXPos - CCore::getMap()->getXPos() - 2), (int)fYPos + 2, getHitBoxX(), getHitBoxY(), true)) {

				if(nextFallFrameID > 0) {
					--nextFallFrameID;
				} else {
					if(currentJumpDistance < 32) {
						updateYPos(1);
						++currentJumpDistance;
					} else {
						updateYPos(2);
					}
				}

				jumpState = 2;

				swimingAnimation();
			} else if(jumpState == 2) {
				resetJump();
			}
		}
	}
}

void Player::movePlayer() {
	if (bMove && !changeMoveDirection && (!bSquat || powerLVL == 0)) {
		if (moveSpeed > currentMaxMove) {
			--moveSpeed;
		}
		else if (SDL_GetTicks() - (100 + 35 * moveSpeed) >= iTimePassed && moveSpeed < currentMaxMove) {
			++moveSpeed;
			iTimePassed = SDL_GetTicks();
		}
		else if (moveSpeed == 0) {
			moveSpeed = 1;
		}
	} else {
		if (SDL_GetTicks() - (50 + 15 * (currentMaxMove - moveSpeed) * (bSquat && powerLVL > 0 ? 6 : 1)) > iTimePassed && moveSpeed != 0) {
			--moveSpeed;
			iTimePassed = SDL_GetTicks();
			if (jumpState == 0 && !CCore::getMap()->getUnderWater()) setMarioSpriteID(6);
		}
		
		if (changeMoveDirection && moveSpeed <= 1) {
			moveDirection = newMoveDirection;
			changeMoveDirection = false;
			bMove = true;
		}
	}
	
	if (moveSpeed > 0) {
		if (moveDirection) {
			updateXPos(moveSpeed);
		}
		else {
			updateXPos(-moveSpeed);
		}

		// ----- SPRITE ANIMATION
		if(CCore::getMap()->getUnderWater()) {
			swimingAnimation();
		} else if (!changeMoveDirection && jumpState == 0 && bMove) {
			moveAnimation();
		}
		// ----- SPRITE ANIMATION
	}
	else if (jumpState == 0) {
		setMarioSpriteID(1);
		updateXPos(0);
	} else {
		updateXPos(0);
	}

	if(bSquat && !CCore::getMap()->getUnderWater() && powerLVL > 0) {
		setMarioSpriteID(7);
	}
}

/* ******************************************** */

void Player::powerUPAnimation() {
	if(inLevelDownAnimation) {
		if(inLevelAnimationFrameID%15 < 5) {
			iSpriteID = 12;
			if(inLevelAnimationFrameID != 0 && inLevelAnimationFrameID%15 == 0) {
				fYPos += 16;
				fXPos -= 4;
			}
		} else if(inLevelAnimationFrameID%15 < 10) {
			iSpriteID = 67;
			if(inLevelAnimationFrameID%15 == 5) {
				fYPos += 16;
				fXPos += 1;
			}
		} else {
			iSpriteID = 1;
			if(inLevelAnimationFrameID%15 == 10) {
				fYPos -= 32;
				fXPos += 3;
			}
		}

		++inLevelAnimationFrameID;
		if(inLevelAnimationFrameID > 59) {
			inLevelAnimation = false;
			fYPos += 32;
			if(jumpState != 0) {
				setMarioSpriteID(5);
			}
		}
	} else if(powerLVL == 1) {
		if(inLevelAnimationFrameID%15 < 5) {
			iSpriteID = 1;
			if(inLevelAnimationFrameID != 0 && inLevelAnimationFrameID%15 == 0) {
				fYPos += 32;
				fXPos += 4;
			}
		} else if(inLevelAnimationFrameID%15 < 10) {
			iSpriteID = 67;
			if(inLevelAnimationFrameID%15 == 5) {
				fYPos -= 16;
				fXPos -= 3;
			}
		} else {
			iSpriteID = 12;
			if(inLevelAnimationFrameID%15 == 10) {
				fYPos -= 16;
				fXPos -= 1;
			}
		}

		++inLevelAnimationFrameID;
		if(inLevelAnimationFrameID > 59) {
			inLevelAnimation = false;
			if(jumpState != 0) {
				setMarioSpriteID(5);
			}
		}
	} else if(powerLVL == 2) {
		if(inLevelAnimationFrameID%10 < 5) {
			iSpriteID = iSpriteID%11 + 22;
		} else {
			iSpriteID = iSpriteID%11 + 33;
		}

		++inLevelAnimationFrameID;
		if(inLevelAnimationFrameID > 59) {
			inLevelAnimation = false;
			if(jumpState != 0) {
				setMarioSpriteID(5);
			}
			iSpriteID = iSpriteID%11 + 22;
		}
	} else {
		inLevelAnimation = false;
	}
}

void Player::moveAnimation() {
	if(SDL_GetTicks() - 65 + moveSpeed * 4 > iMoveAnimationTime) {
		iMoveAnimationTime = SDL_GetTicks();
		if (iSpriteID >= 4 + 11 * powerLVL) {
			setMarioSpriteID(2);
		}
		else {		
			++iSpriteID;
		}
	}
}

void Player::swimingAnimation() {
	if(SDL_GetTicks() - 105 > iMoveAnimationTime) {
		iMoveAnimationTime = SDL_GetTicks();
		if(iSpriteID % 11 == 8) {
			setMarioSpriteID(9);
		} else {
			setMarioSpriteID(8);
		}
	}
}

void Player::startMove() {
	iMoveAnimationTime = SDL_GetTicks();
	iTimePassed = SDL_GetTicks();
	moveSpeed = 1;
	bMove = true;
	if(CCore::getMap()->getUnderWater()) {
		setMarioSpriteID(8);
	}
}

void Player::resetMove() {
	--moveSpeed;
	bMove = false;
}

void Player::stopMove() {
	moveSpeed = 0;
	bMove = false;
	changeMoveDirection = false;
	bSquat = false;
	setMarioSpriteID(1);
}

void Player::startRun() {
	currentMaxMove = maxMove + (CCore::getMap()->getUnderWater() ? 0 : 2);

	createFireBall();
}

void Player::resetRun() {
	currentMaxMove = maxMove;
}

void Player::createFireBall() {
	if(powerLVL == 2) {
		if(nextFireBallFrameID <= 0) {
			CCore::getMap()->addPlayerFireBall((int)(fXPos - CCore::getMap()->getXPos() + (moveDirection ? getHitBoxX() : -32)), (int)(fYPos + getHitBoxY()/2), !moveDirection);
			nextFireBallFrameID = 16;
			CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cFIREBALL);
		}
	}
}

/* ******************************************** */

void Player::jump() {
	if(CCore::getMap()->getUnderWater()) {
		startJump(1);
		nextBubbleTime -= 65;
	} else if(jumpState == 0) {
		startJump(4);
	}
}

void Player::startJump(int iH) {
	currentJumpSpeed = startJumpSpeed;
	jumpDistance = 32 * iH + 24.0f;
	currentJumpDistance = 0;

	if(!CCore::getMap()->getUnderWater()) {
		setMarioSpriteID(5);
	} else {
		if(jumpState == 0) {
			iMoveAnimationTime = SDL_GetTicks();
			setMarioSpriteID(8);
			swimingAnimation();
		}
		CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cSWIM);
	}

	if(iH > 1) {
		if(powerLVL == 0) {
			CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cJUMP);
		} else {
			CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cJUMPBIG);
		}
	}

	jumpState = 1;
}

void Player::resetJump() {
	jumpState = 0;
	jumpDistance = 0;
	currentJumpDistance = 0;
	currentFallingSpeed = 2.7f;
	nextFallFrameID = 0;
	springJump = false;
}

/* ******************************************** */

void Player::updateXPos(int iN) {
	checkCollisionBot(iN, 0);
	checkCollisionCenter(iN, 0);
	if (iN > 0) {
		if (!CCore::getMap()->checkCollisionRB((int)(fXPos - CCore::getMap()->getXPos() + iN), (int)fYPos - 2, getHitBoxX(), getHitBoxY(), true) && !CCore::getMap()->checkCollisionRT((int)(fXPos - CCore::getMap()->getXPos() + iN), (int)fYPos + 2, getHitBoxX(), true) &&
			(powerLVL == 0 ? true : (!CCore::getMap()->checkCollisionRC((int)(fXPos - CCore::getMap()->getXPos() + iN), (int)fYPos, getHitBoxX(), getHitBoxY() / 2, true))))
		{
			if (fXPos >= 416 && CCore::getMap()->getMoveMap()) {
				CCore::getMap()->moveMap(-iN, 0);
			}
			else {
				fXPos += iN;
			}
		}
		else {
			updateXPos(iN - 1);
			if (moveSpeed > 1 && jumpState == 0) --moveSpeed;
		}
	} else if (iN < 0) {
		if (!CCore::getMap()->checkCollisionLB((int)(fXPos - CCore::getMap()->getXPos() + iN), (int)fYPos - 2, getHitBoxY(), true) && !CCore::getMap()->checkCollisionLT((int)(fXPos - CCore::getMap()->getXPos() + iN), (int)fYPos + 2, true) &&
				(powerLVL == 0 ? true : (!CCore::getMap()->checkCollisionLC((int)(fXPos - CCore::getMap()->getXPos() + iN), (int)fYPos, getHitBoxY() / 2, true))))
			{
			if (fXPos <= 192 && CCore::getMap()->getXPos() && CCore::getMap()->getMoveMap() && CCFG::canMoveBackward) {

				CCore::getMap()->moveMap(-iN, 0);
			}
			else if(fXPos - CCore::getMap()->getXPos() + iN >= 0 && fXPos >= 0) {
				fXPos += iN;
			} else if(CCFG::canMoveBackward && fXPos >= 0) {
				updateXPos(iN + 1);
			}
		}
		else {
			updateXPos(iN + 1);
			if (moveSpeed > 1 && jumpState == 0) --moveSpeed;
		}
	}
}

void Player::updateYPos(int iN) {
	bool bLEFT, bRIGHT;
	
	if (iN > 0) {
		bLEFT  = CCore::getMap()->checkCollisionLB((int)(fXPos - CCore::getMap()->getXPos() + 2), (int)fYPos + iN, getHitBoxY(), true);
		bRIGHT = CCore::getMap()->checkCollisionRB((int)(fXPos - CCore::getMap()->getXPos() - 2), (int)fYPos + iN, getHitBoxX(), getHitBoxY(), true);

		if (!bLEFT && !bRIGHT) {
			fYPos += iN;
		} else {
			if (jumpState == 2) {
				jumpState = 0;
			}
			updateYPos(iN - 1);
		}
	} else if(iN < 0) {
		bLEFT  = CCore::getMap()->checkCollisionLT((int)(fXPos - CCore::getMap()->getXPos() + 2), (int)fYPos + iN, false);
		bRIGHT = CCore::getMap()->checkCollisionRT((int)(fXPos - CCore::getMap()->getXPos() - 2), (int)fYPos + iN, getHitBoxX(), false);

		if(CCore::getMap()->checkCollisionWithPlatform((int)fXPos, (int)fYPos, 0, 0) >= 0 || CCore::getMap()->checkCollisionWithPlatform((int)fXPos, (int)fYPos, getHitBoxX(), 0) >= 0) {
			jumpState = 2;
		}
		else if (!bLEFT && !bRIGHT) {
			fYPos += iN;
		} else  {
			if (jumpState == 1) {
				if (!bLEFT && bRIGHT) {
					Vector2* vRT = getBlockRT(fXPos - CCore::getMap()->getXPos(), fYPos + iN);

					if(!CCore::getMap()->getBlock(CCore::getMap()->getMapBlock(vRT->getX(), vRT->getY())->getBlockID())->getVisible()) {
						if(CCore::getMap()->blockUse(vRT->getX(), vRT->getY(), CCore::getMap()->getMapBlock(vRT->getX(), vRT->getY())->getBlockID(), 0)) {
							jumpState = 2;
						} else {
							fYPos += iN;
						}
					} else if((int)(fXPos + getHitBoxX() - CCore::getMap()->getXPos()) % 32 <= 8) {
						updateXPos((int)-((int)(fXPos + getHitBoxX() - CCore::getMap()->getXPos()) % 32));
					} else if(CCore::getMap()->getBlock(CCore::getMap()->getMapBlock(vRT->getX(), vRT->getY())->getBlockID())->getUse()) {
						if(CCore::getMap()->blockUse(vRT->getX(), vRT->getY(), CCore::getMap()->getMapBlock(vRT->getX(), vRT->getY())->getBlockID(), 0)) {
							jumpState = 2;
						} else {
							fYPos += iN;
						}
					} else {
						jumpState = 2;
					}

					delete vRT;
				} else if (bLEFT && !bRIGHT) {
					Vector2* vLT = getBlockLT(fXPos - CCore::getMap()->getXPos(), fYPos + iN);
					if(!CCore::getMap()->getBlock(CCore::getMap()->getMapBlock(vLT->getX(), vLT->getY())->getBlockID())->getVisible()) {
						if(CCore::getMap()->blockUse(vLT->getX(), vLT->getY(), CCore::getMap()->getMapBlock(vLT->getX(), vLT->getY())->getBlockID(), 0)) {
							jumpState = 2;
						} else {
							fYPos += iN;
						}
					} else if ((int)(fXPos - CCore::getMap()->getXPos()) % 32 >= 24) {
						updateXPos((int)(32 - (int)(fXPos - CCore::getMap()->getXPos()) % 32));
					} else if(CCore::getMap()->getBlock(CCore::getMap()->getMapBlock(vLT->getX(), vLT->getY())->getBlockID())->getUse()) {
						if(CCore::getMap()->blockUse(vLT->getX(), vLT->getY(), CCore::getMap()->getMapBlock(vLT->getX(), vLT->getY())->getBlockID(), 0)) {
							jumpState = 2;
						} else {
							fYPos += iN;
						}
					} else {
						jumpState = 2;
					}

					delete vLT;
				} else {
					if ((int)(fXPos + getHitBoxX() - CCore::getMap()->getXPos()) % 32 > 32 - (int)(fXPos - CCore::getMap()->getXPos()) % 32) {
						Vector2* vRT = getBlockRT(fXPos - CCore::getMap()->getXPos(), fYPos + iN);

						if (CCore::getMap()->getBlock(CCore::getMap()->getMapBlock(vRT->getX(), vRT->getY())->getBlockID())->getUse()) {
							if(CCore::getMap()->blockUse(vRT->getX(), vRT->getY(), CCore::getMap()->getMapBlock(vRT->getX(), vRT->getY())->getBlockID(), 0)) {
								jumpState = 2;
							}
						} else {
							jumpState = 2;
						}

						delete vRT;
					} else {
						Vector2* vLT = getBlockLT(fXPos - CCore::getMap()->getXPos(), fYPos + iN);

						if (CCore::getMap()->getBlock(CCore::getMap()->getMapBlock(vLT->getX(), vLT->getY())->getBlockID())->getUse()) {
							if(CCore::getMap()->blockUse(vLT->getX(), vLT->getY(), CCore::getMap()->getMapBlock(vLT->getX(), vLT->getY())->getBlockID(), 0)) {
								jumpState = 2;
							}
						} else {
							jumpState = 2;
						}

						delete vLT;
					}
				}
			}

			updateYPos(iN + 1);
		}
	}

	if((int)fYPos % 2 == 1) {
		fYPos += 1;
	}

	if(!CCore::getMap()->getInEvent() && fYPos - getHitBoxY() > CCFG::GAME_HEIGHT) {
		CCore::getMap()->playerDeath(false, true);
		fYPos = -80;
	}
}

/* ******************************************** */

bool Player::checkCollisionBot(int nX, int nY) {
	Vector2* vLT = getBlockLB(fXPos - CCore::getMap()->getXPos() + nX, fYPos + nY);

	if (CCore::getMap()->getBlock(CCore::getMap()->getMapBlock(vLT->getX(), vLT->getY())->getBlockID())->getUse()) {
		CCore::getMap()->blockUse(vLT->getX(), vLT->getY(), CCore::getMap()->getMapBlock(vLT->getX(), vLT->getY())->getBlockID(), 1);
	}

	delete vLT;

	vLT = getBlockRB(fXPos - CCore::getMap()->getXPos() + nX, fYPos + nY);

	if (CCore::getMap()->getBlock(CCore::getMap()->getMapBlock(vLT->getX(), vLT->getY())->getBlockID())->getUse()) {
		CCore::getMap()->blockUse(vLT->getX(), vLT->getY(), CCore::getMap()->getMapBlock(vLT->getX(), vLT->getY())->getBlockID(), 1);
	}

	delete vLT;
	return true;
}

bool Player::checkCollisionCenter(int nX, int nY) {
	if(powerLVL == 0) {
		Vector2* vLT = getBlockLC(fXPos - CCore::getMap()->getXPos() + nX, fYPos + nY);

		if (CCore::getMap()->getBlock(CCore::getMap()->getMapBlock(vLT->getX(), vLT->getY())->getBlockID())->getUse()) {
			CCore::getMap()->blockUse(vLT->getX(), vLT->getY(), CCore::getMap()->getMapBlock(vLT->getX(), vLT->getY())->getBlockID(), 2);
		}

		delete vLT;

		vLT = getBlockRC(fXPos - CCore::getMap()->getXPos() + nX, fYPos + nY);

		if (CCore::getMap()->getBlock(CCore::getMap()->getMapBlock(vLT->getX(), vLT->getY())->getBlockID())->getUse()) {
			CCore::getMap()->blockUse(vLT->getX(), vLT->getY(), CCore::getMap()->getMapBlock(vLT->getX(), vLT->getY())->getBlockID(), 2);
		}

		delete vLT;
	} else {
		Vector2* vLT = getBlockLC(fXPos - CCore::getMap()->getXPos() + nX, fYPos + nY + (powerLVL > 0 ? 16 : 0));

		if (CCore::getMap()->getBlock(CCore::getMap()->getMapBlock(vLT->getX(), vLT->getY())->getBlockID())->getUse()) {
			CCore::getMap()->blockUse(vLT->getX(), vLT->getY(), CCore::getMap()->getMapBlock(vLT->getX(), vLT->getY())->getBlockID(), 2);
		}

		delete vLT;

		vLT = getBlockRC(fXPos - CCore::getMap()->getXPos() + nX, fYPos + nY + (powerLVL > 0 ? 16 : 0));

		if (CCore::getMap()->getBlock(CCore::getMap()->getMapBlock(vLT->getX(), vLT->getY())->getBlockID())->getUse()) {
			CCore::getMap()->blockUse(vLT->getX(), vLT->getY(), CCore::getMap()->getMapBlock(vLT->getX(), vLT->getY())->getBlockID(), 2);
		}

		delete vLT;

		vLT = getBlockLC(fXPos - CCore::getMap()->getXPos() + nX, fYPos + nY - (powerLVL > 0 ? 16 : 0));

		if (CCore::getMap()->getBlock(CCore::getMap()->getMapBlock(vLT->getX(), vLT->getY())->getBlockID())->getUse()) {
			CCore::getMap()->blockUse(vLT->getX(), vLT->getY(), CCore::getMap()->getMapBlock(vLT->getX(), vLT->getY())->getBlockID(), 2);
		}

		delete vLT;

		vLT = getBlockRC(fXPos - CCore::getMap()->getXPos() + nX, fYPos + nY - (powerLVL > 0 ? 16 : 0));

		if (CCore::getMap()->getBlock(CCore::getMap()->getMapBlock(vLT->getX(), vLT->getY())->getBlockID())->getUse()) {
			CCore::getMap()->blockUse(vLT->getX(), vLT->getY(), CCore::getMap()->getMapBlock(vLT->getX(), vLT->getY())->getBlockID(), 2);
		}

		delete vLT;
	}

	return true;
}

/* ******************************************** */

Vector2* Player::getBlockLB(float nX, float nY) {
	return CCore::getMap()->getBlockID((int)nX + 1, (int)nY + getHitBoxY() + 2);
}

Vector2* Player::getBlockRB(float nX, float nY) {
	return CCore::getMap()->getBlockID((int)nX + getHitBoxX() - 1, (int)nY + getHitBoxY() + 2);
}

Vector2* Player::getBlockLC(float nX, float nY) {
	return CCore::getMap()->getBlockID((int)nX - 1, (int)nY + getHitBoxY()/2);
}

Vector2* Player::getBlockRC(float nX, float nY) {
	return CCore::getMap()->getBlockID((int)nX + getHitBoxX() + 1, (int)nY + getHitBoxY()/2);
}

Vector2* Player::getBlockLT(float nX, float nY) {
	return CCore::getMap()->getBlockID((int)nX + 1, (int)nY);
}

Vector2* Player::getBlockRT(float nX, float nY) {
	return CCore::getMap()->getBlockID((int)nX + getHitBoxX() - 1, (int)nY);
}

/* ******************************************** */

void Player::Draw(SDL_Renderer* rR) {
	if(!inLevelDownAnimation || CCore::getMap()->getInEvent()) {
		sMario[getMarioSpriteID()]->getTexture()->Draw(rR, (int)fXPos, (int)fYPos + (CCore::getMap()->getInEvent() ? 0 : 2), !moveDirection);
	} else {
		if(inLevelDownAnimationFrameID%15 < (inLevelDownAnimationFrameID > 120 ? 7 : inLevelDownAnimationFrameID > 90 ? 9 : inLevelDownAnimationFrameID > 60 ? 11 : inLevelDownAnimationFrameID > 30 ? 13 : 14)) {
			sMario[getMarioSpriteID()]->getTexture()->Draw(rR, (int)fXPos, (int)fYPos + (CCore::getMap()->getInEvent() ? 0 : 2), !moveDirection);
		}
	}
}

/* ******************************************** */

int Player::getMarioSpriteID() {
	if(starEffect && !inLevelAnimation && CCFG::getMM()->getViewID() == CCFG::getMM()->eGame) {
		if(unKillAbleTimeFrameID <= 0) {
			starEffect = unKillAble = false;
		}

		if(unKillAbleTimeFrameID == 35) {
			CCFG::getMusic()->changeMusic(true, true);
		}

		++unKillAbleFrameID;

		--unKillAbleTimeFrameID;

		if(unKillAbleTimeFrameID < 90) {
			if(unKillAbleFrameID < 5) {
				return powerLVL < 1 ? iSpriteID + 44 : powerLVL == 2 ? iSpriteID : iSpriteID + 11;
			} else if(unKillAbleFrameID < 10) {
				return powerLVL < 1 ? iSpriteID + 55 : powerLVL == 2 ? iSpriteID + 11 : iSpriteID + 22;
			} else {
				unKillAbleFrameID = 0;
			}
		} else {
			if(unKillAbleFrameID < 20) {
				return powerLVL < 1 ? iSpriteID + 44 : powerLVL == 2 ? iSpriteID : iSpriteID + 11;
			} else if(unKillAbleFrameID < 40) {
				return powerLVL < 1 ? iSpriteID + 55 : powerLVL == 2 ? iSpriteID + 11 : iSpriteID + 22;
			} else {
				unKillAbleFrameID = 0;
			}
		}
	}

	return iSpriteID;
}

void Player::setMarioSpriteID(int iID) {
	this->iSpriteID = iID + 11 * powerLVL;
}

int Player::getHitBoxX() {
	return powerLVL == 0 ? iSmallX : iBigX;
}

int Player::getHitBoxY() {
	return powerLVL == 0 ? iSmallY : bSquat ? 44 : iBigY;
}

/* ******************************************** */

void Player::addCoin() {
	++iCoins;
	iScore += 100;
}

/* ******************************************** */

void Player::setMoveDirection(bool moveDirection) {
	this->moveDirection = moveDirection;
}

bool Player::getChangeMoveDirection() {
	return changeMoveDirection;
}

void Player::setChangeMoveDirection() {
	this->changeMoveDirection = true;
	this->newMoveDirection = !moveDirection;
}

/* ******************************************** */

bool Player::getInLevelAnimation() {
	return inLevelAnimation;
}

void Player::setInLevelAnimation(bool inLevelAnimation) {
	this->inLevelAnimation = inLevelAnimation;
}

int Player::getXPos() {
	return (int)fXPos;
}

void Player::setXPos(float fXPos) {
	this->fXPos = fXPos;
}

int Player::getYPos() {
	return (int)fYPos;
}

int Player::getPowerLVL() {
	return powerLVL;
}

void Player::setPowerLVL(int powerLVL) {
	if(powerLVL <= 2) {
		if(this->powerLVL < powerLVL) {
			CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cMUSHROOMMEAT);
			setScore(getScore() + 1000);
			CCore::getMap()->addPoints((int)(fXPos - CCore::getMap()->getXPos() + getHitBoxX() / 2), (int)fYPos + 16, "1000", 8, 16);
			inLevelAnimation = true;
			inLevelAnimationFrameID = 0;
			inLevelDownAnimationFrameID = 0;
			inLevelDownAnimation = false;
			this->powerLVL = powerLVL;
		} else if(this->powerLVL > 0) {
			CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cSHRINK);
			inLevelDownAnimation = true;
			inLevelDownAnimationFrameID = 180;
			inLevelAnimation = true;
			inLevelAnimationFrameID = 0;
			this->powerLVL = 0;
			unKillAble = true;
		}
	} else {
		++iNumOfLives;
		CCore::getMap()->addPoints((int)(fXPos - CCore::getMap()->getXPos() + getHitBoxX() / 2), (int)fYPos + 16, "1UP", 10, 14);
		CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cONEUP);
	}
}

void Player::resetPowerLVL() {
	this->powerLVL = 0;
	this->iSpriteID = 1;
}

int Player::getNumOfLives() {
	return iNumOfLives;
}

bool Player::getStarEffect() {
	return starEffect;
}

void Player::setStarEffect(bool starEffect) {
	if(starEffect && this->starEffect != starEffect) {
		CCFG::getMusic()->PlayMusic(CCFG::getMusic()->mSTAR);
	}
	this->starEffect = starEffect;
	this->unKillAble = starEffect;

	this->unKillAbleFrameID = 0;

	this->unKillAbleTimeFrameID = 550;
}

bool Player::getUnkillAble() {
	return unKillAble;
}

void Player::setNumOfLives(int iNumOfLives) {
	this->iNumOfLives = iNumOfLives;
}

int Player::getMoveSpeed() {
	return moveSpeed;
}

int Player::getJumpState() {
	return jumpState;
}

bool Player::getMove() {
	return bMove;
}

bool Player::getMoveDirection() {
	return moveDirection;
}

void Player::setNextFallFrameID(int nextFallFrameID) {
	this->nextFallFrameID = nextFallFrameID;
}

void Player::setCurrentJumpSpeed(float currentJumpSpeed) {
	this->currentJumpSpeed = currentJumpSpeed;
}

void Player::setMoveSpeed(int moveSpeed) {
	this->moveSpeed = moveSpeed;
}

void Player::setYPos(float fYPos) {
	this->fYPos = fYPos;
}

bool Player::getSquat() {
	return bSquat;
}

void Player::setSquat(bool bSquat) {
	if(bSquat && this->bSquat != bSquat) {
		if(powerLVL > 0) {
			fYPos += 20;
		}
		this->bSquat = bSquat;
	} else if(this->bSquat != bSquat && !CCore::getMap()->getUnderWater()) {
		if(powerLVL > 0) {
			fYPos -= 20;
		}
		this->bSquat = bSquat;
	}
}

void Player::setSpringJump(bool springJump) {
	this->springJump = springJump;
}

unsigned int Player::getScore() {
	return iScore;
}

void Player::setScore(unsigned int iScore) {
	this->iScore = iScore;
}

void Player::addComboPoints() {
	++iComboPoints;
	iFrameID = 40;
}

int Player::getComboPoints() {
	return iComboPoints;
}

unsigned int Player::getCoins() {
	return iCoins;
}

void Player::setCoins(unsigned int iCoins) {
	this->iCoins = iCoins;
}

Sprite* Player::getMarioSprite() {
	return sMario[1 + 11 * powerLVL];
}

CIMG* Player::getMarioLVLUP() {
	return tMarioLVLUP;
}