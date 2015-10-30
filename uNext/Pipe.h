#pragma once

#ifndef PIPE_H
#define PIPE_H

class Pipe
{
private:
	// ----- 0 = VERTICAL, 1 = HORIZONTAL -> VERTICAL, 2 = VERTICAL -> VERTICAL
	int iType;

	// ----- X, Y LEFT Block Position
	int iLX, iLY;
	// ----- X, Y RIGHT Block Position
	int iRX, iRY;

	int newPlayerPosX, newPlayerPosY;

	// ----- MAP LVL ID
	int newCurrentLevel;
	int newLevelType;
	bool newMoveMap;
	bool newUnderWater;

	int iDelay;

	int iSpeed;
public:
	Pipe(int iType, int iLX, int iLY, int iRX, int iRY, int newPlayerPosX, int newPlayerPosY, int newCurrentLevel, int newLevelType, bool newMoveMap, int iDelay, int iSpeed, bool underWater);
	~Pipe(void);

	void checkUse();
	void setEvent();
};

#endif