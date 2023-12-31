/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute
 * it and/or use it and/or modify it under the terms of the
 * "Clarified Artistic License"
 */
#ifndef EnemyAircraft_Tank_h
#define EnemyAircraft_Tank_h

#include "EnemyAircraft.h"

class ActiveAmmo;
class Global;

//====================================================================
class EnemyAircraft_Tank : public EnemyAircraft
{
public:
	EnemyAircraft_Tank(EnemyType et, float p[3], float randFact = 1.0);
	virtual ~EnemyAircraft_Tank();

	void	update();
	void	init();
	void	init(float *p, float randFact = 1.0);

	virtual const char* NameString()
	{
		return "EnemyAircraft_Tank";
	}

friend class EnemyFleet;
friend class ScreenItemAdd;

protected:
	void	move();
};

#endif //EnemyAircraft_Tank_h
