#pragma once

#ifndef BULLETBILL_H
#define BULLETBILL_H	

#include "Minion.h"

class BulletBll : public Minion
{
private:



public:
	BulletBll(void);
	~BulletBll(void);

	void Update();
	void Draw(SDL_Renderer* rR, CIMG* iIMG);


};

#endif