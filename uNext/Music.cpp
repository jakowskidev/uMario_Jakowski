#include "Music.h"
#include "Core.h"
#include "SDL_mixer.h"

/* ******************************************** */

Music::Music(void) {
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

	vMusic.push_back(loadMusic("overworld"));
	vMusic.push_back(loadMusic("overworld-fast"));
	vMusic.push_back(loadMusic("underground"));
	vMusic.push_back(loadMusic("underground-fast"));
	vMusic.push_back(loadMusic("underwater"));
	vMusic.push_back(loadMusic("underwater-fast"));
	vMusic.push_back(loadMusic("castle"));
	vMusic.push_back(loadMusic("castle-fast"));
	vMusic.push_back(loadMusic("lowtime"));
	vMusic.push_back(loadMusic("starmusic"));
	vMusic.push_back(loadMusic("starmusic-fast"));
	vMusic.push_back(loadMusic("scorering"));

	vChunk.push_back(loadChunk("coin"));
	vChunk.push_back(loadChunk("blockbreak"));
	vChunk.push_back(loadChunk("blockhit"));
	vChunk.push_back(loadChunk("boom"));
	vChunk.push_back(loadChunk("bowserfall"));
	vChunk.push_back(loadChunk("bridgebreak"));
	vChunk.push_back(loadChunk("bulletbill"));
	vChunk.push_back(loadChunk("death"));
	vChunk.push_back(loadChunk("fire"));
	vChunk.push_back(loadChunk("fireball"));
	vChunk.push_back(loadChunk("gameover"));
	vChunk.push_back(loadChunk("intermission"));
	vChunk.push_back(loadChunk("jump"));
	vChunk.push_back(loadChunk("jumpbig"));
	vChunk.push_back(loadChunk("levelend"));
	vChunk.push_back(loadChunk("lowtime"));
	vChunk.push_back(loadChunk("mushroomappear"));
	vChunk.push_back(loadChunk("mushroomeat"));
	vChunk.push_back(loadChunk("oneup"));
	vChunk.push_back(loadChunk("pause"));
	vChunk.push_back(loadChunk("shrink"));
	vChunk.push_back(loadChunk("rainboom"));
	vChunk.push_back(loadChunk("shot"));
	vChunk.push_back(loadChunk("shrink"));
	vChunk.push_back(loadChunk("stomp"));
	vChunk.push_back(loadChunk("swim"));
	vChunk.push_back(loadChunk("vine"));
	vChunk.push_back(loadChunk("castleend"));
	vChunk.push_back(loadChunk("princessmusic"));

	setVolume(100);
	this->currentMusic = mNOTHING;
}

Music::~Music(void) {
	for(unsigned int i = 0; i < vMusic.size(); i++) {
		Mix_FreeMusic(vMusic[i]);
	}

	vMusic.clear();

	for(unsigned int i = 0; i < vChunk.size(); i++) {
		Mix_FreeChunk(vChunk[i]);
	}

	vChunk.clear();
}

/* ******************************************** */

void Music::changeMusic(bool musicByLevel, bool forceChange) {
	eMusic eNew = currentMusic;

	if(musicByLevel) {
		if(CCore::getMap()->getInEvent() && CCore::getMap()->getEvent()->inEvent) {
			eNew = mNOTHING;
			PlayChunk(cINTERMISSION);
		} else {
			switch(CCore::getMap()->getLevelType()) {
				case 0: case 4:
					eNew = CCore::getMap()->getMapTime() > 90 ? mOVERWORLD : mOVERWORLDFAST;
					break;
				case 1:
					eNew = CCore::getMap()->getMapTime() > 90 ? mUNDERWORLD : mUNDERWORLDFAST;
					break;
				case 2:
					eNew = CCore::getMap()->getMapTime() > 90 ? mUNDERWATER : mUNDERWATERFAST;
					break;
				case 3:
					eNew = CCore::getMap()->getMapTime() > 90 ? mCASTLE : mCASTLEFAST;
					break;
				case 100:
					eNew = mNOTHING;
					PlayChunk(cINTERMISSION);
					CCore::getMap()->setLevelType(0);
					break;
				default:
					eNew = mNOTHING;
					break;
			}
		}
	}

	if(currentMusic != eNew || forceChange) {
		StopMusic();
		currentMusic = eNew;
		PlayMusic();
	}
}

void Music::PlayMusic() {
	if(currentMusic != mNOTHING) {
		Mix_PlayMusic(vMusic[currentMusic - 1], -1);
		musicStopped = false;
	} else {
		StopMusic();
	}
}

void Music::PlayMusic(eMusic musicID) {
	if(musicID != mNOTHING) {
		Mix_PlayMusic(vMusic[musicID - 1], -1);
		musicStopped = false;
		currentMusic = musicID;
	} else {
		StopMusic();
		currentMusic = mNOTHING;
	}
}

void Music::StopMusic() {
	if(!musicStopped) {
		Mix_HaltMusic();
		musicStopped = true;
	}
}

void Music::PauseMusic() {
	if(Mix_PausedMusic() == 1) {
		Mix_ResumeMusic();
		musicStopped = false;
	} else {
		Mix_PauseMusic();
		musicStopped = true;
	}
}

/* ******************************************** */

void Music::PlayChunk(eChunk chunkID) {
	Mix_VolumeChunk(vChunk[chunkID], iVolume);
	Mix_PlayChannel(-1, vChunk[chunkID], 0);
}

/* ******************************************** */

Mix_Music* Music::loadMusic(std::string fileName) {
	fileName = "files/sounds/" + fileName + ".wav";
	return Mix_LoadMUS(fileName.c_str());
}

Mix_Chunk* Music::loadChunk(std::string fileName) {
	fileName = "files/sounds/" + fileName + ".wav";
	return Mix_LoadWAV(fileName.c_str());
}

int Music::getVolume() {
	return iVolume;
}

void Music::setVolume(int iVolume) {
	this->iVolume = iVolume;
	Mix_VolumeMusic(iVolume);
}
