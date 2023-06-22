/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute
 * it and/or use it and/or modify it under the terms of the
 * "Clarified Artistic License"
 */

#ifdef HAVE_CONFIG_H
#include <chromium-bsu-config.h>
#endif

#include "gettext.h"

#include "EnemyAircraft.h"

#include <cstdio>
#include <cstdlib>
#include <cmath>

#include "Config.h"

#include "define.h"
#include "Ammo.h"
#include "Global.h"
#include "EnemyAmmo.h"
#include "HeroAircraft.h"
#include "ScreenItemAdd.h"

#include "EnemyAircraft_Straight.h"
#include "EnemyAircraft_Omni.h"
#include "EnemyAircraft_RayGun.h"
#include "EnemyAircraft_Tank.h"
#include "EnemyAircraft_Gnat.h"
#include "EnemyAircraft_Boss00.h"
#include "EnemyAircraft_Boss01.h"

int	EnemyAircraft::allocated = 0;

//=================================================================
EnemyAircraft::EnemyAircraft(EnemyType et, float p[3], float randFact)
	: ScreenItem(ScreenItem::ItemEnemy)
{
	type = et;

	game = Global::getInstance();

	Config *config = Config::instance();

	EnemyAircraft::init(p, randFact);

	float xBound = config->screenBoundX()-2.0;
	if(pos[0] < -xBound)
		pos[0] = -xBound;
	if(pos[0] > xBound)
		pos[0] = xBound;

	allocated++;
}

//----------------------------------------------------------
EnemyAircraft::~EnemyAircraft()
{
	allocated--;
}

//----------------------------------------------------------
// this is only here to get rid of the IRIX compiler warning...
void EnemyAircraft::init()
{
	ScreenItem::init();
}

//----------------------------------------------------------
void EnemyAircraft::init(float *p, float randFact)
{
	ScreenItem::init();
	over = 0;	//-- if this points to another aircraft, this aircraft will be inserted after that
				//   aircraft in the fleet list

	pos[0] = p[0];
	pos[1] = p[1];
	pos[2] = p[2];

	shootInterval = 1;
	shootSwap	= 0;
	randMoveX	= randFact*FRAND;
	lastMoveX	= 0.0;
	lastMoveY	= 0.0;
	preFire		= 0.0;
	target = game->hero;

	next = 0;
	back = 0;

	secondaryMove[0] = secondaryMove[1] = 0.0;

	shootVec[0] =  0.0;
	shootVec[1] = -0.2;
	shootVec[2] =  0.0;

	vel[0] = 0.0;
	vel[1] = 0.0;
	vel[2] = 0.0;
}


//----------------------------------------------------------
void EnemyAircraft::printNumAllocated(void)
{
	fprintf(stderr, _("%d EnemyAircraft allocated\n"), allocated);
}

//----------------------------------------------------------
EnemyAircraft* EnemyAircraft::makeNewEnemy(EnemyType et, float p[3], float randFact)
{
	EnemyAircraft *enemy = 0;

	switch(et)
	{
		case EnemyStraight:
			enemy = new EnemyAircraft_Straight(et, p, randFact);
			break;
		case EnemyOmni:
			enemy = new EnemyAircraft_Omni(et, p, randFact);
			break;
		case EnemyRayGun:
			enemy = new EnemyAircraft_RayGun(et, p, randFact);
			break;
		case EnemyTank:
			enemy = new EnemyAircraft_Tank(et, p, randFact);
			break;
		case EnemyGnat:
			enemy = new EnemyAircraft_Gnat(et, p, randFact);
			break;
		case EnemyBoss00:
			enemy = new EnemyAircraft_Boss00(et, p, randFact);
			break;
		case EnemyBoss01:
			enemy = new EnemyAircraft_Boss01(et, p, randFact);
			break;
		case NumEnemyTypes:
		default:
			enemy = 0;
			break;
	}
	return enemy;
}

//
//----------------------------------------------------------
bool EnemyAircraft::checkHit(ActiveAmmo *ammo)
{
	bool retVal = false;
	if( ammo->pos[1] > pos[1]-size[1] )
	{
		if( (ammo->pos[1] < pos[1]+size[1]) &&
			(ammo->pos[0] > pos[0]-size[0]) &&
			(ammo->pos[0] < pos[0]+size[0]) )
		{
			retVal = true;
		}
	}
	return retVal;
}

//----------------------------------------------------------
void EnemyAircraft::calcShootInterval()
{
	shootInterval = 1;
}
