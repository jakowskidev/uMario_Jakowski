#pragma once

#ifndef MAP_H
#define MAP_H

#include "header.h"
#include "IMG.h"
#include "Block.h"
#include "MapLevel.h"
#include "Player.h"
#include "PlayerFireBall.h"
#include "Vector2.h"
#include "LevelText.h"
#include "Goombas.h"
#include "Koppa.h"
#include "Beetle.h"
#include "Mushroom.h"
#include "Flag.h"
#include "Toad.h"
#include "Bowser.h"
#include "Plant.h"
#include "Hammer.h"
#include "HammerBro.h"
#include "Star.h"
#include "Cheep.h"
#include "CheepSpawner.h"
#include "Squid.h"
#include "Flower.h"
#include "Lakito.h"
#include "Spikey.h"
#include "FireBall.h"
#include "Fire.h"
#include "Pipe.h"
#include "Spring.h"
#include "BulletBill.h"
#include "BulletBillSpawner.h"
#include "Vine.h"
#include "UpFire.h"
#include "Platform.h"
#include "Event.h"
#include "Coin.h"
#include "Points.h"
#include "BlockDebris.h"
#include "Bubble.h"
#include <vector>

/* ******************************************** */

class Map
{
private:
	float fXPos, fYPos;

	std::vector<Block*> vBlock;
	int iBlockSize; // Size of vBlock

	std::vector<Block*> vMinion;
	int iMinionSize; // Size of vBlock

	std::vector<std::vector<MapLevel*>> lMap;
	int iMapWidth, iMapHeight;

	std::vector<BlockDebris*> lBlockDebris;

	std::vector<Platform*> vPlatform;

	std::vector<LevelText*> vLevelText;

	std::vector<Bubble*> lBubble;

	int currentLevelID;
	int iLevelType; // 0, 1, 2;
	bool underWater;

	int iSpawnPointID;

	bool bMoveMap;

	int iFrameID;
	int iMapTime;
	
	bool inEvent;
	Event* oEvent;

	// ----- PLAYER -----
	Player* oPlayer;

	// ----- MINIONS -----
	std::vector<std::vector<Minion*>> lMinion;
	int iMinionListSize;

	int getListID(int nXPos);

	int getNumOfMinions(); // ----- Ilosc minionow w grze.
	// ----- MINIONS -----

	// ----- POINTS & COIN -----

	std::vector<Coin*> lCoin;
	std::vector<Points*> lPoints;

	// ----- POINTS & COIN -----

	// ----- PIPEEVENTS -----

	std::vector<Pipe*> lPipe;

	// ----- ENDEVENTS

	Flag* oFlag;

	bool drawLines;

	// ---------- Methods ----------

	int getStartBlock();
	int getEndBlock();

	// ----- Load -----
	void loadGameData(SDL_Renderer* rR);

	void createMap();

	void checkSpawnPoint();
	int getNumOfSpawnPoints();
	int getSpawnPointXPos(int iID);
	int getSpawnPointYPos(int iID);

	void loadLVL_1_1();
	void loadLVL_1_2();
	void loadLVL_1_3();
	void loadLVL_1_4();
	void loadLVL_2_1();
	void loadLVL_2_2();
	void loadLVL_2_3();
	void loadLVL_2_4();
	void loadLVL_3_1();
	void loadLVL_3_2();
	void loadLVL_3_3();
	void loadLVL_3_4();
	void loadLVL_4_1();
	void loadLVL_4_2();
	void loadLVL_4_3();
	void loadLVL_4_4();
	void loadLVL_5_1();
	void loadLVL_5_2();
	void loadLVL_5_3();
	void loadLVL_5_4();
	void loadLVL_6_1();
	void loadLVL_6_2();
	void loadLVL_6_3();
	void loadLVL_6_4();
	void loadLVL_7_1();
	void loadLVL_7_2();
	void loadLVL_7_3();
	void loadLVL_7_4();
	void loadLVL_8_1();
	void loadLVL_8_2();
	void loadLVL_8_3();
	void loadLVL_8_4();

	void loadMinionsLVL_1_1();
	void loadMinionsLVL_1_2();
	void loadMinionsLVL_1_3();
	void loadMinionsLVL_1_4();
	void loadMinionsLVL_2_1();
	void loadMinionsLVL_2_2();
	void loadMinionsLVL_2_3();
	void loadMinionsLVL_2_4();
	void loadMinionsLVL_3_1();
	void loadMinionsLVL_3_2();
	void loadMinionsLVL_3_3();
	void loadMinionsLVL_3_4();
	void loadMinionsLVL_4_1();
	void loadMinionsLVL_4_2();
	void loadMinionsLVL_4_3();
	void loadMinionsLVL_4_4();
	void loadMinionsLVL_5_1();
	void loadMinionsLVL_5_2();
	void loadMinionsLVL_5_3();
	void loadMinionsLVL_5_4();
	void loadMinionsLVL_6_1();
	void loadMinionsLVL_6_2();
	void loadMinionsLVL_6_3();
	void loadMinionsLVL_6_4();
	void loadMinionsLVL_7_1();
	void loadMinionsLVL_7_2();
	void loadMinionsLVL_7_3();
	void loadMinionsLVL_7_4();
	void loadMinionsLVL_8_1();
	void loadMinionsLVL_8_2();
	void loadMinionsLVL_8_3();
	void loadMinionsLVL_8_4();

	void clearPipeEvents();
	void loadPipeEventsLVL_1_1();
	void loadPipeEventsLVL_1_2();
	void loadPipeEventsLVL_1_3();
	void loadPipeEventsLVL_2_1();
	void loadPipeEventsLVL_2_2();
	void loadPipeEventsLVL_3_1();
	void loadPipeEventsLVL_4_1();
	void loadPipeEventsLVL_4_2();
	void loadPipeEventsLVL_5_1();
	void loadPipeEventsLVL_5_2();
	void loadPipeEventsLVL_6_2();
	void loadPipeEventsLVL_7_1();
	void loadPipeEventsLVL_7_2();
	void loadPipeEventsLVL_8_1();
	void loadPipeEventsLVL_8_2();
	void loadPipeEventsLVL_8_4();

	void clearLevelText();

	void pipeUse();
	void EndUse();
	void EndBoss();
	void EndBonus();

	void TPUse();
	void TPUse2();
	void TPUse3();

	bool bTP; // -- TP LOOP

	void spawnVine(int nX, int nY, int iBlockID);

	void clearMap();
	void clearMinions();
public:
	Map(void);
	Map(SDL_Renderer* rR);
	~Map(void);

	void Update();

	void UpdatePlayer();
	void UpdateMinions();
	void UpdateMinionsCollisions();
	void UpdateBlocks();
	void UpdateMinionBlokcs();

	void Draw(SDL_Renderer* rR);
	void DrawMap(SDL_Renderer* rR);
	void DrawMinions(SDL_Renderer* rR);
	void DrawGameLayout(SDL_Renderer* rR);
	void DrawLines(SDL_Renderer* rR);

	void moveMap(int iX, int iY);
	void setSpawnPoint();

	bool blockUse(int nX, int nY, int iBlockID, int POS);

	void addPoints(int X, int Y, std::string sText, int iW, int iH);
	void addGoombas(int iX, int iY, bool moveDirection);
	void addKoppa(int iX, int iY, int minionState, bool moveDirection);
	void addBeetle(int X, int Y, bool moveDirection);
	void addPlant(int iX, int iY);
	void addToad(int X, int Y, bool peach);
	void addSquid(int X, int Y);
	void addCheep(int X, int Y, int minionType, int moveSpeed, bool moveDirection = false);
	void addCheepSpawner(int X, int XEnd);
	void addHammerBro(int X, int Y);
	void addHammer(int X, int Y, bool moveDirection);
	void addLakito(int X, int Y, int iMaxXPos);
	void addSpikey(int X, int Y);

	void addPlayerFireBall(int X, int Y, bool moveDirection);

	void addUpFire(int X, int iYEnd);
	void addSpring(int X, int Y);

	void addBowser(int X, int Y, bool spawnHammer = false);

	void addFire(float fX, float fY, int toYPos);
	void addFireBall(int X, int Y, int iWidth, int iSliceID, bool LEFT);

	void addVine(int X, int Y, int minionState, int iBlockID);

	void addText(int X, int Y, std::string sText);

	void addBubble(int X, int Y);
	void addBulletBillSpawner(int X, int Y, int minionState);
	void addBulletBill(int X, int Y, bool moveDirection, int minionState);

	void lockMinions();
	void clearPlatforms();
	void clearBubbles();

	// ********** COLLISION
	Vector2* getBlockID(int nX, int nY);
	int getBlockIDX(int nX);
	int getBlockIDY(int nY);

	// ----- LEFT
	bool checkCollisionLB(int nX, int nY, int nHitBoxY, bool checkVisible);
	bool checkCollisionLT(int nX, int nY, bool checkVisible);
	// ----- CENTER
	bool checkCollisionLC(int nX, int nY, int nHitBoxY, bool checkVisible);
	bool checkCollisionRC(int nX, int nY, int nHitBoxX, int nHitBoxY, bool checkVisible);
	// ----- RIGHT
	bool checkCollisionRB(int nX, int nY, int nHitBoxX, int nHitBoxY, bool checkVisible);
	bool checkCollisionRT(int nX, int nY, int nHitBoxX, bool checkVisible);

	int checkCollisionWithPlatform(int nX, int nY, int iHitBoxX, int iHitBoxY);

	bool checkCollision(Vector2* nV, bool checkVisible);

	void checkCollisionOnTopOfTheBlock(int nX, int nY);
	// ********** COLLISION

	void playerDeath(bool animation, bool instantDeath);

	// ----- LOAD
	void resetGameData();
	void loadLVL();
	void setBackgroundColor(SDL_Renderer* rR);
	std::string getLevelName();

	void startLevelAnimation();

	void structBush(int X, int Y, int iSze);
	void structGrass(int X, int Y, int iSize);
	void structCloud(int X, int Y, int iSize);
	void structGND(int X, int Y, int iWidth, int iHeight);
	void structGND2(int X, int Y, int iSize, bool bDir); // true = LEFT, false = RIGHT
	void structGND2(int X, int Y, int iWidth, int iHeight);
	void structUW1(int X, int Y, int iWidth, int iHeight);

	void structBonus(int X, int Y, int iWidth);
	void structBonusEnd(int X);

	void structPipe(int X, int Y, int iHeight);
	void structPipeVertical(int X, int Y, int iHeight);
	void structPipeHorizontal(int X, int Y, int iWidth);

	void structCoins(int X, int Y, int iWidth, int iHeight);

	void structBrick(int X, int Y, int iWidth, int iHeight);
	void struckBlockQ(int X, int Y, int iWidth);
	void struckBlockQ2(int X, int Y, int iWidth);

	void structEnd(int X, int Y, int iHeight);

	void structCastleSmall(int X, int Y);
	void structCastleBig(int X, int Y);
	void structCastleWall(int X, int Y, int iWidth, int iHeight);

	void structT(int X, int Y, int iWidth, int iHeight);
	void structTMush(int X, int Y, int iWidth, int iHeight);

	void structWater(int X, int Y, int iWidth, int iHeight);
	void structLava(int X, int Y, int iWidth, int iHeight);
	void structBridge(int X, int Y, int iWidth);

	void structTree(int X, int Y, int iHeigth, bool BIG);
	void structFence(int X, int Y, int iWidth);
	void structBridge2(int X, int Y, int iWidth);

	void structSeeSaw(int X, int Y, int iWidth);
	void structPlatformLine(int X);

	void structBulletBill(int X, int Y, int iHieght);

	void setBlockID(int X, int Y, int iBlockID);

	// ----- get & set -----
	float getXPos();
	void setXPos(float iYPos);
	float getYPos();
	void setYPos(float iYPos);

	int getLevelType();
	void setLevelType(int iLevelType);

	int getCurrentLevelID();
	void setCurrentLevelID(int currentLevelID);

	bool getUnderWater();
	void setUnderWater(bool underWater);

	int getMapTime();
	void setMapTime(int iMapTime);

	bool getDrawLines();
	void setDrawLines(bool drawLines);

	void setSpawnPointID(int iSpawnPointID);

	int getMapWidth();

	Block* getBlock(int iID);
	Block* getMinionBlock(int iID);
	MapLevel* getMapBlock(int iX, int iY);

	Player* getPlayer();
	Platform* getPlatform(int iID);

	Flag* getFlag();

	bool getMoveMap();
	void setMoveMap(bool bMoveMap);

	Event* getEvent();
	bool getInEvent();
	void setInEvent(bool inEvent);
};

#endif