/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute
 * it and/or use it and/or modify it under the terms of the
 * "Clarified Artistic License"
 */
#ifndef EnemyAmmo_h
#define EnemyAmmo_h

#ifdef HAVE_CONFIG_H
#include <chromium-bsu-config.h>
#endif

#include "compatibility.h"

#if defined(HAVE_APPLE_OPENGL_FRAMEWORK) || defined(HAVE_OPENGL_GL_H)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include "define.h"
#include "Ammo.h"

class Global;
class HeroAircraft;

//====================================================================
class EnemyAmmo
{
public:
	EnemyAmmo();
	~EnemyAmmo();

	void	addAmmo(int type, float pos[3], float vel[3]);
	void	updateAmmo();

	void	checkForHits(HeroAircraft *hero);

	void	drawGL();
	void	clear();

private:
	ActiveAmmo	*getNewAmmo();
	void 		killAmmo(ActiveAmmo *dead);

private:
	ActiveAmmo	*ammoRoot[NUM_ENEMY_AMMO_TYPES];
	ActiveAmmo	*ammoPool;
	float		ammoSize[NUM_ENEMY_AMMO_TYPES][2];
	float		ammoDamage[NUM_ENEMY_AMMO_TYPES];

private:
	Global *game;
};


#endif // EnemyAmmo_h
