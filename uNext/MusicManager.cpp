/************************
 * @author £ukasz Jakowski
 * @since  02.05.2014 16:13
 * 
 ************************/

#include "MusicManager.h"

/* ******************************************** */

MusicManager::MusicManager(void) {
	

	
}

MusicManager::~MusicManager(void) {

}

/* ******************************************** */

void MusicManager::PlayMusic() {
	vMusic.push_back(Mix_LoadMUS("files/sounds/overworld.wav"));
	vMusic.push_back(Mix_LoadMUS("files/sounds/overworld.wav"));
	Mix_VolumeMusic(100);
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
	Mix_PlayMusic(vMusic[0], -1);
}

/* ******************************************** */

Mix_Music* MusicManager::loadMusic(std::string fileName) {
	fileName = "files/sounds/" + fileName + ".wav";
	return Mix_LoadMUS("files/sounds/overworld.wav");
}

/* ******************************************** */