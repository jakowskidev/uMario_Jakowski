#include "header.h"
#include "Core.h"
#include "IMG.h"
#include "CFG.h"
#include "Text.h"
#include "SDL_mixer.h"

/* ******************************************** */
const Uint8 * keystate;
Map* CCore::oMap = new Map();
bool CCore::mouseLeftPressed = false;
bool CCore::mouseRightPressed = false;
int CCore::mouseX = 0;
int CCore::mouseY = 0;
bool CCore::quitGame = false;

bool CCore::movePressed = false;
bool CCore::keyMenuPressed = false;
bool CCore::keyS = false;
bool CCore::keyW = false;
bool CCore::keyA = false;
bool CCore::keyD = false;
bool CCore::keyShift = false;
bool CCore::keyAPressed = false;
bool CCore::keyDPressed = false;


CCore::CCore(void) {
	this->quitGame = false;
	this->iFPS = 0;
	this->iNumOfFPS = 0;
	this->lFPSTime = 0;

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER);
	
	window = SDL_CreateWindow("uMario - www.LukaszJakowski.pl", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, CCFG::GAME_WIDTH, CCFG::GAME_HEIGHT, SDL_WINDOW_SHOWN);
	if(window == NULL) {
		quitGame = true;
	}

	rR = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_RenderSetLogicalSize(rR, CCFG::GAME_WIDTH, CCFG::GAME_HEIGHT);
	this ->Fullscreen = false;
	
	// ----- ICO
	std::string fileName = "files/images/ico.bmp";
	SDL_Surface* loadedSurface = SDL_LoadBMP(fileName.c_str());
	SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 255, 0, 255));

	SDL_SetWindowIcon(window, loadedSurface);
	SDL_FreeSurface(loadedSurface);

	mainEvent = new SDL_Event();
	// ----- ICO
	
	
	oController = GameController();

	if (SDL_NumJoysticks() == 0) {
		this->ControllerEnabled = false;
	} else {
		this->ControllerEnabled = true;
	}
		
	oMap = new Map(rR);
	CCFG::getMM()->setActiveOption(rR);
	CCFG::getSMBLOGO()->setIMG("super_mario_bros", rR);

	CCFG::getMusic()->PlayMusic();

	this->keyMenuPressed = this->movePressed = this->keyS = this->keyW = this->keyA = this->keyD = this->keyShift = false;

	this->keyAPressed = this->keyDPressed = this->firstDir = false;

	this->mouseX = this->mouseY = 0;

	CCFG::keyIDA = SDLK_a;
	CCFG::keyIDS = SDLK_s;
	CCFG::keyIDD = SDLK_d;
	CCFG::keyIDSpace = SDLK_SPACE;
	CCFG::keyIDShift = SDLK_LSHIFT;
}

CCore::~CCore(void) {
	delete oMap;
	delete mainEvent;
	SDL_DestroyRenderer(rR);
	SDL_DestroyWindow(window);
}

/* ******************************************** */

void CCore::mainLoop() {
	lFPSTime = SDL_GetTicks();
	
	while(!quitGame && mainEvent->type != SDL_QUIT) {
		
		frameTime = SDL_GetTicks();
		SDL_PollEvent(mainEvent);
		SDL_RenderClear(rR);

		CCFG::getMM()->setBackgroundColor(rR);
		SDL_RenderFillRect(rR, NULL);

		ManageController();
		Input();
		MouseInput();

		SetFS();
		

		Update();
		Draw();
		
		
		CCFG::getText()->Draw(rR, "FPS:" + std::to_string(iNumOfFPS), CCFG::GAME_WIDTH - CCFG::getText()->getTextWidth("FPS:" + std::to_string(iNumOfFPS), 8) - 8, 5, 8);

		if(SDL_GetTicks() - 1000 >= lFPSTime) {
			lFPSTime = SDL_GetTicks();
			iNumOfFPS = iFPS;
			iFPS = 0;
		}

		++iFPS;
		

		SDL_RenderPresent(rR);

		
		if(SDL_GetTicks() - frameTime < MIN_FRAME_TIME) {
			SDL_Delay(MIN_FRAME_TIME - (SDL_GetTicks () - frameTime));
		}
	}
}

void CCore::ManageController()
{
	if (mainEvent->type == SDL_CONTROLLERDEVICEADDED)
		oController.Add(mainEvent->cdevice.which);
		this->ControllerEnabled = true;
	if (mainEvent->type == SDL_CONTROLLERDEVICEREMOVED)
		oController.Remove(mainEvent->cdevice.which);
		if (SDL_NumJoysticks() == 0)
			this->ControllerEnabled = false;
}

void CCore::Input() {
	switch(CCFG::getMM()->getViewID()) {
		case 2: case 7:
			if(!oMap->getInEvent()) {
				InputPlayer();
			} else {
				resetMove();
			}
			break;
		default:
			InputMenu();
			break;
	}
}

void CCore::InputMenu() {
	if (mainEvent->type == SDL_CONTROLLERBUTTONDOWN) {
		switch (mainEvent->cbutton.button) {
			case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
				if (!keyMenuPressed) {
					CCFG::getMM()->keyPressed(2);
					keyMenuPressed = true;
				}
				break;
			case SDL_CONTROLLER_BUTTON_DPAD_UP:
				if (!keyMenuPressed) {
					CCFG::getMM()->keyPressed(0);
					keyMenuPressed = true;
				}
				break;
			case SDL_CONTROLLER_BUTTON_START: case SDL_CONTROLLER_BUTTON_A:
				if (!keyMenuPressed) {
					CCFG::getMM()->enter();
					keyMenuPressed = true;
				}
				break;
			case SDL_CONTROLLER_BUTTON_BACK: case SDL_CONTROLLER_BUTTON_B:
				if (!keyMenuPressed) {
					CCFG::getMM()->escape();
					keyMenuPressed = true;
				}
				break;
			case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
				if (!keyMenuPressed) {
					CCFG::getMM()->keyPressed(3);
					keyMenuPressed = true;
				}
				break;
			case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
				if (!keyMenuPressed) {
					CCFG::getMM()->keyPressed(1);
					keyMenuPressed = true;
				}
				break;
		}
	}
	else if (mainEvent->type == SDL_CONTROLLERBUTTONUP) {
		switch (mainEvent->cbutton.button) {
		case SDL_CONTROLLER_BUTTON_DPAD_DOWN: case SDL_CONTROLLER_BUTTON_DPAD_UP: case SDL_CONTROLLER_BUTTON_START: case SDL_CONTROLLER_BUTTON_A: case SDL_CONTROLLER_BUTTON_BACK: case SDL_CONTROLLER_BUTTON_B: case SDL_CONTROLLER_BUTTON_DPAD_LEFT: case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
			keyMenuPressed = false;
			break;
		default:
			break;
		}
	}

	if(mainEvent->type == SDL_KEYDOWN) {
		CCFG::getMM()->setKey(mainEvent->key.keysym.sym);

		switch(mainEvent->key.keysym.sym) {
			case SDLK_s: case SDLK_DOWN:
				if(!keyMenuPressed) {
					CCFG::getMM()->keyPressed(2);
					keyMenuPressed = true;
				}
				break;
			case SDLK_w: case SDLK_UP:
				if(!keyMenuPressed) {
					CCFG::getMM()->keyPressed(0);
					keyMenuPressed = true;
				}
				break;
			case SDLK_KP_ENTER: case SDLK_RETURN:
				if(!keyMenuPressed) {
					CCFG::getMM()->enter();
					keyMenuPressed = true;
				}
				break;
			case SDLK_ESCAPE:
				if(!keyMenuPressed) {
					CCFG::getMM()->escape();
					keyMenuPressed = true;
				}
				break;
			case SDLK_LEFT: case SDLK_d:
				if(!keyMenuPressed) {
					CCFG::getMM()->keyPressed(3);
					keyMenuPressed = true;
				}
				break;
			case SDLK_RIGHT: case SDLK_a:
				if(!keyMenuPressed) {
					CCFG::getMM()->keyPressed(1);
					keyMenuPressed = true;
				}
				break;
			case SDLK_F12:
				if (!Fullscreen) {
					Fullscreen = true;
				}
				else {
					Fullscreen = false;
				}
				break;
		}
	}

	if(mainEvent->type == SDL_KEYUP) {
		switch(mainEvent->key.keysym.sym) {
			case SDLK_s: case SDLK_DOWN: case SDLK_w: case SDLK_UP: case SDLK_KP_ENTER: case SDLK_RETURN: case SDLK_ESCAPE: case SDLK_a: case SDLK_RIGHT: case SDLK_LEFT: case SDLK_d:
				keyMenuPressed = false;
				break;
			default:
				break;
		}
	}
}

void CCore::InputPlayer() {

	if (this->ControllerEnabled) {
		if (oController.GetButton(1, "Right") == 1) {
			keyDPressed = true;
			if (!keyAPressed) {
				firstDir = true;
			}
		} else{
			if (firstDir) {
				firstDir = false;
			}
			keyDPressed = false;}

		if (oController.GetButton(1, "Down") == 1) {
			if (!keyS) {
				keyS = true;
				if (!oMap->getUnderWater() && !oMap->getPlayer()->getInLevelAnimation()) oMap->getPlayer()->setSquat(true);
			}
		} else {
			oMap->getPlayer()->setSquat(false);
			keyS = false;}
 
		if (oController.GetButton(1, "Left") == 1) {
			keyAPressed = true;
			if (!keyDPressed) {
				firstDir = false;
			}
		} else{
			if (!firstDir) {
				firstDir = true;
			}
			keyAPressed = false;}

		if (oController.GetButton(1, "A") == 1) {
			oMap->getPlayer()->jump();
			if (!CCFG::keySpace) {
				CCFG::keySpace = true;
			}
		} else{ CCFG::keySpace = false; }

		if (oController.GetButton(1, "X") == 1) {
			if (!keyShift) {
				oMap->getPlayer()->startRun();
				keyShift = true;
			}
		} else{
			if (keyShift) {
				oMap->getPlayer()->resetRun();
				keyShift = false;
			}
		}
	}

	if(mainEvent->type == SDL_WINDOWEVENT) {
		switch(mainEvent->window.event) {
			case SDL_WINDOWEVENT_FOCUS_LOST:
				CCFG::getMM()->resetActiveOptionID(CCFG::getMM()->ePasue);
				CCFG::getMM()->setViewID(CCFG::getMM()->ePasue);
				CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cPASUE);
				CCFG::getMusic()->PauseMusic();
				break;
		}
	}

	
	if (mainEvent->type == SDL_CONTROLLERBUTTONDOWN) {
		switch (mainEvent->cbutton.button) {
				
			case SDL_CONTROLLER_BUTTON_START:
				if (!keyMenuPressed) {
					CCFG::getMM()->enter();
					keyMenuPressed = true;
				}
			case SDL_CONTROLLER_BUTTON_BACK:
				if (!keyMenuPressed && CCFG::getMM()->getViewID() == CCFG::getMM()->eGame) {
					CCFG::getMM()->resetActiveOptionID(CCFG::getMM()->ePasue);
					CCFG::getMM()->setViewID(CCFG::getMM()->ePasue);
					CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cPASUE);
					CCFG::getMusic()->PauseMusic();
					keyMenuPressed = true;
				}
				break;
		}
	}

	if (mainEvent->type == SDL_KEYDOWN) {
		if (mainEvent->key.keysym.sym == CCFG::keyIDD) {
			keyDPressed = true;
			if (!keyAPressed) {
				firstDir = true;
			}
		}

		if (mainEvent->key.keysym.sym == CCFG::keyIDS) {
			if (!keyS) {
				keyS = true;
				if (!oMap->getUnderWater() && !oMap->getPlayer()->getInLevelAnimation()) oMap->getPlayer()->setSquat(true);
			}
		}

		if (mainEvent->key.keysym.sym == CCFG::keyIDA) {
			keyAPressed = true;
			if (!keyDPressed) {
				firstDir = false;
			}
		}

		if (mainEvent->key.keysym.sym == CCFG::keyIDSpace) {
			if (!CCFG::keySpace) {
				oMap->getPlayer()->jump();
				CCFG::keySpace = true;
			}
		}

		if (mainEvent->key.keysym.sym == CCFG::keyIDShift) {
			if (!keyShift) {
				oMap->getPlayer()->startRun();
				keyShift = true;
			}
		}

		switch (mainEvent->key.keysym.sym) {
		case SDLK_KP_ENTER: case SDLK_RETURN:
			if (!keyMenuPressed) {
				CCFG::getMM()->enter();
				keyMenuPressed = true;
			}
		case SDLK_ESCAPE:
			if (!keyMenuPressed && CCFG::getMM()->getViewID() == CCFG::getMM()->eGame) {
				CCFG::getMM()->resetActiveOptionID(CCFG::getMM()->ePasue);
				CCFG::getMM()->setViewID(CCFG::getMM()->ePasue);
				CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cPASUE);
				CCFG::getMusic()->PauseMusic();
				keyMenuPressed = true;
			}
			break;
		}
	}

	 if(mainEvent->type == SDL_KEYUP) {
		if(mainEvent->key.keysym.sym == CCFG::keyIDD) {
				if(firstDir) {
					firstDir = false;
				}

				keyDPressed = false;
			}

			if(mainEvent->key.keysym.sym == CCFG::keyIDS) {
				oMap->getPlayer()->setSquat(false);
				keyS = false;
			}
		
			if(mainEvent->key.keysym.sym == CCFG::keyIDA) {
				if(!firstDir) {
					firstDir = true;
				}

				keyAPressed = false;
			}
		
			if(mainEvent->key.keysym.sym == CCFG::keyIDSpace) {
				CCFG::keySpace = false;
			}
		
			if(mainEvent->key.keysym.sym == CCFG::keyIDShift) {
				if(keyShift) {
					oMap->getPlayer()->resetRun();
					keyShift = false;
				}
			}
		switch(mainEvent->key.keysym.sym) {
			case SDLK_KP_ENTER: case SDLK_RETURN: case SDLK_ESCAPE:
				keyMenuPressed = false;
				break;
		}
	}
	 if (mainEvent->type = SDL_CONTROLLERBUTTONUP) {
		 switch (mainEvent->cbutton.button) {
		 case SDL_CONTROLLER_BUTTON_START: case SDL_CONTROLLER_BUTTON_BACK:
			 keyMenuPressed = false;
			 break;
		 }
	 }


	if(keyAPressed) {
		if(!oMap->getPlayer()->getMove() && firstDir == false && !oMap->getPlayer()->getChangeMoveDirection() && !oMap->getPlayer()->getSquat()) {
			oMap->getPlayer()->startMove();
			oMap->getPlayer()->setMoveDirection(false);
		} else if(!keyDPressed && oMap->getPlayer()->getMoveSpeed() > 0 && firstDir != oMap->getPlayer()->getMoveDirection()) {
			oMap->getPlayer()->setChangeMoveDirection();
		}
	}

	if(keyDPressed) {
		if(!oMap->getPlayer()->getMove() && firstDir == true && !oMap->getPlayer()->getChangeMoveDirection() && !oMap->getPlayer()->getSquat()) {
			oMap->getPlayer()->startMove();
			oMap->getPlayer()->setMoveDirection(true);
		} else if(!keyAPressed && oMap->getPlayer()->getMoveSpeed() > 0 && firstDir != oMap->getPlayer()->getMoveDirection()) {
			oMap->getPlayer()->setChangeMoveDirection();
		}
	}

	if(oMap->getPlayer()->getMove() && !keyAPressed && !keyDPressed) {
		oMap->getPlayer()->resetMove();
	}
}

void CCore::MouseInput() {
	switch(mainEvent->type) {
		case SDL_MOUSEBUTTONDOWN: {
			switch (mainEvent->button.button) {
				case SDL_BUTTON_LEFT:
					mouseLeftPressed = true;
					break;
				case SDL_BUTTON_RIGHT:
					mouseRightPressed = true;
					break;
			}
			break;
		}
		case SDL_MOUSEMOTION: {
			
			SDL_GetMouseState(&mouseX, &mouseY);
			//CCFG::getMM()->getConsole()->print("x:" + std::to_string(mouseX));
			//CCFG::getMM()->getConsole()->print("y:" + std::to_string(mouseY));
			break;
		}
		case SDL_MOUSEBUTTONUP: {
			switch (mainEvent->button.button) {
				case SDL_BUTTON_LEFT:
					mouseLeftPressed = false;
					break;
				case SDL_BUTTON_RIGHT:
					mouseRightPressed = false;
					break;
			}
			break;
		}
		case SDL_MOUSEWHEEL:
			if(mainEvent->wheel.timestamp > SDL_GetTicks() - 2) {
				//CCFG::getMM()->getLE()->mouseWheel(mainEvent->wheel.y);
			}
			break;
	}
}


void CCore::SetFS() {
	if (this->Fullscreen == true) {
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
		SDL_RenderSetLogicalSize(rR, CCFG::GAME_WIDTH, CCFG::GAME_HEIGHT);
	}
	else if (this->Fullscreen == false) {
		SDL_SetWindowFullscreen(window, 0);
	}
}

void CCore::resetKeys() {
	movePressed = keyMenuPressed = keyS = keyW = keyA = keyD = CCFG::keySpace = keyShift = keyAPressed = keyDPressed = false;
}

void CCore::Update() {
	CCFG::getMM()->Update();
}


void CCore::Draw() {
	CCFG::getMM()->Draw(rR);
}

/* ******************************************** */

void CCore::resetMove() {
	this->keyAPressed = this->keyDPressed = false;
}

Map* CCore::getMap() {
	return oMap;
}