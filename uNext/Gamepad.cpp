#include "Gamepad.h"


/* ******************************************** */


GameController:: GameController(void) {
	Controllers[MAX_CONTROLLERS] = nullptr;
	for (int JoystickIndex = 0; JoystickIndex < MaxJoysticks; ++JoystickIndex) {
		if (!SDL_IsGameController(JoystickIndex)) { continue; }
		if (ControllerIndex >= MAX_CONTROLLERS) { break; }
		Controllers[ControllerIndex] = SDL_GameControllerOpen(JoystickIndex);
		ControllerIndex++;
	}
}

GameController::~GameController(void) {

}
/* ******************************************** */

bool GameController:: GetButton(int ControllerNum, std::string Button) {
	int CN = ControllerNum - 1;
	if (Button == "A") { return SDL_GameControllerGetButton(Controllers[CN], SDL_CONTROLLER_BUTTON_A); }
	if (Button == "B") { return SDL_GameControllerGetButton(Controllers[CN], SDL_CONTROLLER_BUTTON_B); }
	if (Button == "X") { return SDL_GameControllerGetButton(Controllers[CN], SDL_CONTROLLER_BUTTON_X); }
	if (Button == "Y") { return SDL_GameControllerGetButton(Controllers[CN], SDL_CONTROLLER_BUTTON_Y); }

	if (Button == "Up") { return SDL_GameControllerGetButton(Controllers[CN], SDL_CONTROLLER_BUTTON_DPAD_UP); }
	if (Button == "Down") { return SDL_GameControllerGetButton(Controllers[CN], SDL_CONTROLLER_BUTTON_DPAD_DOWN); }
	if (Button == "Left") { return SDL_GameControllerGetButton(Controllers[CN], SDL_CONTROLLER_BUTTON_DPAD_LEFT); }
	if (Button == "Right") { return SDL_GameControllerGetButton(Controllers[CN], SDL_CONTROLLER_BUTTON_DPAD_RIGHT); }

	if (Button == "Start") { return SDL_GameControllerGetButton(Controllers[CN], SDL_CONTROLLER_BUTTON_START); }
	if (Button == "Select") { return SDL_GameControllerGetButton(Controllers[CN], SDL_CONTROLLER_BUTTON_BACK); }
	else { return false; }
}

/* ******************************************** */

int GameController::Add(int  JoystickIndex) {
	if (!SDL_IsGameController(JoystickIndex))
		return 1;
	else {
		Controllers[JoystickIndex] = SDL_GameControllerOpen(JoystickIndex);
		ControllerIndex++;
		return 0;
	}
}


void GameController::Remove(SDL_JoystickID ControllerID) {
	for (int i = 0; i < MaxJoysticks; i++) {
		if (Controllers[i] == SDL_GameControllerFromInstanceID(ControllerID)) {
			SDL_GameControllerClose(Controllers[i]);
			Controllers[i] = nullptr;
			ControllerIndex--;
			break;
		}
	}
}