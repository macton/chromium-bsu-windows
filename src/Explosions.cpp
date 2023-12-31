/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute
 * it and/or use it and/or modify it under the terms of the
 * "Clarified Artistic License"
 */
#include "Explosions.h"

#include <cstdlib>

#ifdef HAVE_CONFIG_H
#include <chromium-bsu-config.h>
#endif

#include "compatibility.h"

#if defined(HAVE_APPLE_OPENGL_FRAMEWORK) || defined(HAVE_OPENGL_GL_H)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include "extern.h"
#include "Global.h"
#include "HeroAircraft.h"
#include "Image.h"
#include "Atlas.h"

//====================================================================
Explosions::Explosions()
{
	game = Global::getInstance();

	int		i;
	for(i = 0; i < (int)NumExploTypes; i++)
	{
		exploRoot[i] = new Explo();
		exploSize[i][0] = 0.5;
		exploSize[i][1] = 0.5;
		exploStay[i] = 20.0;
		exploPause[i][0] = 0;
		exploPause[i][1] = 0;
		exploPause[i][2] = 0;
	}
	exploPool = new Explo();

	exploSize[EnemyDestroyed][0] = 1.35;
	exploSize[EnemyDestroyed][1] = 1.35;
	exploStay[EnemyDestroyed]	= 30.0;

	exploSize[EnemyDamage][0]	= 1.0;
	exploSize[EnemyDamage][1]	= 1.0;
	exploStay[EnemyDamage]		= 20.0;
	exploPause[EnemyDamage][1]	= 3;

	exploSize[EnemyAmmo00][0]	= 1.5;
	exploSize[EnemyAmmo00][1]	= 1.5;
	exploStay[EnemyAmmo00]		= 15.0;
	exploPause[EnemyAmmo00][1]	= 1;

	exploSize[EnemyAmmo01][0]	= 0.5;
	exploSize[EnemyAmmo01][1]	= 0.5;
	exploStay[EnemyAmmo01]		= 10.0;
	exploPause[EnemyAmmo01][1]	= 3;

	exploSize[EnemyAmmo02][0]	= 1.7;
	exploSize[EnemyAmmo02][1]	= 1.7;
	exploStay[EnemyAmmo02]		= 10.0;
	exploPause[EnemyAmmo02][1]	= 2;

	exploSize[EnemyAmmo03][0]	= 1.7;
	exploSize[EnemyAmmo03][1]	= 1.7;
	exploStay[EnemyAmmo03]		= 10.0;
	exploPause[EnemyAmmo03][1]	= 2;

	exploSize[EnemyAmmo04][0]	= 2.0;
	exploSize[EnemyAmmo04][1]	= 1.5;
	exploStay[EnemyAmmo04]		= 10.0;
	exploPause[EnemyAmmo04][1]	= 5;

	exploSize[HeroDestroyed][0] = 1.5;
	exploSize[HeroDestroyed][1] = 1.5;
	exploStay[HeroDestroyed]	= 25.0;

	exploSize[HeroDamage][0]	= 1.1;
	exploSize[HeroDamage][1]	= 1.1;
	exploStay[HeroDamage]		= 25.0;
	exploPause[HeroDamage][1]	= 3;

	exploSize[HeroAmmo00][0]	= 0.25;
	exploSize[HeroAmmo00][1]	= 0.25;
	exploStay[HeroAmmo00]		= 10.0;

	exploSize[HeroAmmo01][0]	= 0.5;
	exploSize[HeroAmmo01][1]	= 1.0;
	exploStay[HeroAmmo01]		= 15.0;
	exploPause[HeroAmmo01][1]	= 1;

	exploSize[HeroAmmo02][0]	= 0.9;
	exploSize[HeroAmmo02][1]	= 1.0;
	exploStay[HeroAmmo02]		= 23.0;

	exploSize[HeroShields][0]	= 1.6;
	exploSize[HeroShields][1]	= 1.6;
	exploStay[HeroShields]		= 25.0;
	exploPause[HeroShields][1]	= 5;

	exploSize[PowerBurst][0]	= 1.8;
	exploSize[PowerBurst][1]	= 1.8;
	exploStay[PowerBurst]		= 35.0;

	exploSize[AddLife][0]		= 2.5;
	exploSize[AddLife][1]		= 2.5;
	exploStay[AddLife]			= 25.0;

	exploSize[LoseLife][0]		= 3.5;
	exploSize[LoseLife][1]		= 3.5;
	exploStay[LoseLife]			= 35.0;

	exploSize[ScoreLife][0]		= 3.5;
	exploSize[ScoreLife][1]		= 3.5;
	exploStay[ScoreLife]		= 35.0;

	exploSize[Electric][0]		= 1.7;
	exploSize[Electric][1]		= 0.5;
	exploStay[Electric]			= 43.0;
	exploPause[Electric][1]		= 0;

	exploSize[Glitter][0]		= 0.8;
	exploSize[Glitter][1]		= 1.0;
	exploStay[Glitter]			= 20.0;
	exploPause[Glitter][1]		= 0;

}

Explosions::~Explosions()
{
	Explo *cur;
	Explo *del;

	clear();	//-- move all explosions into the pool

	cur = exploPool->next;
	while(cur)
	{
		del = cur;
		cur = cur->next;
		delete del;
	}

	for(int i = 0; i < (int)NumExploTypes; i++)
		delete exploRoot[i];
	delete exploPool;
}

//----------------------------------------------------------
void	Explosions::clear()
{
	int i;
	Explo *cur;
	Explo *del;

	for(i = 0; i < NumExploTypes; i++)
	{
		cur = exploRoot[i]->next;
		while(cur)
		{
			del = cur;
			cur = cur->next;
			killExplo(del);
		}
		exploRoot[i]->next = 0;
	}
}

//----------------------------------------------------------
Explo *Explosions::getNewExplo()
{
	Explo *retExplo;
	if(exploPool->next)
	{
		retExplo = exploPool->next;
		exploPool->next = retExplo->next;
	}
	else
	{
		retExplo = new Explo();
	}
	return retExplo;
}

//----------------------------------------------------------
void Explosions::killExplo(Explo *dead)
{
	dead->back = 0;
	dead->next = exploPool->next;
	exploPool->next = dead;
}

//----------------------------------------------------------
Explo *Explosions::addExplo(ExploType t, float p[3], int a, float s)
{
	Explo *newExplo	= 0;
	Explo *first	= 0;

	if((exploPause[t][0]) <= 0)
	{
		exploPause[t][2] = 1; //-- set flag to init explo pause count
		newExplo = getNewExplo();
		newExplo->init(p, a, s);
		newExplo->back = exploRoot[t];
		newExplo->next = exploRoot[t]->next;
		first = exploRoot[t]->next;
		if(first)
			first->back = newExplo;
		exploRoot[t]->next = newExplo;
	}
	return newExplo;
}

//----------------------------------------------------------
Explo *Explosions::addElectric(float p[3], float v[3], float clr[4], int age, float size)
{
	Explo *newExplo	= 0;
	Explo *first	= 0;

	exploPause[Electric][2] = 1; //-- set flag to init explo pause count
	newExplo = getNewExplo();
	newExplo->init(p, v, clr, age, size);
	newExplo->back = exploRoot[Electric];
	newExplo->next = exploRoot[Electric]->next;
	first = exploRoot[Electric]->next;
	if(first)
		first->back = newExplo;
	exploRoot[Electric]->next = newExplo;
	return newExplo;
}

//----------------------------------------------------------
Explo *Explosions::addGlitter(float p[3], float v[3], float clr[4], int age, float size)
{
	Explo *newExplo	= 0;
	Explo *first	= 0;

	exploPause[Glitter][2] = 1; //-- set flag to init explo pause count
	newExplo = getNewExplo();
	newExplo->init(p, v, clr, age, size);
	newExplo->back = exploRoot[Glitter];
	newExplo->next = exploRoot[Glitter]->next;
	first = exploRoot[Glitter]->next;
	if(first)
		first->back = newExplo;
	exploRoot[Glitter]->next = newExplo;
	return newExplo;
}

//----------------------------------------------------------
void	Explosions::update()
{
	Explo	*explo;
	Explo	*backExplo;
	Explo	*nextExplo;
	for(int i = 0; i < NumExploTypes; i++)
	{
		if(exploPause[i][0] > 0.0)
			exploPause[i][0] -= game->speedAdj;
		else
			exploPause[i][0] = 0.0;
		if(exploPause[i][2]) //-- if flag was set, init count
		{
			exploPause[i][0] = exploPause[i][1];
			exploPause[i][2] = 0.0;
		}

		explo = exploRoot[i]->next;
		while(explo)
		{
			explo->age++;
			if(explo->age > 0)
			{
				explo->pos[0] += explo->vel[0]*game->speedAdj;
				explo->pos[1] += explo->vel[1]*game->speedAdj;
				explo->pos[2] += explo->vel[2]*game->speedAdj;
			}
			if(explo->age > (exploStay[i]/game->speedAdj))
			{
				backExplo = explo->back;
				nextExplo = explo->next;
				backExplo->next = nextExplo;
				if(nextExplo)
					nextExplo->back = backExplo;
				killExplo(explo);
				explo = nextExplo;
			}
			else
				explo = explo->next;
		}
	}
}

//----------------------------------------------------------
void	Explosions::drawGL()
{

	if(exploRoot[EnemyDestroyed]->next)	drawEnemyDestroyed(EnemyDestroyed);
	if(exploRoot[EnemyDamage]->next)	drawEnemyDamage(EnemyDamage);
	if(exploRoot[EnemyAmmo00]->next)	drawAmmo(EnemyAmmo00);
	if(exploRoot[EnemyAmmo01]->next)	drawAmmo(EnemyAmmo01);
	if(exploRoot[EnemyAmmo02]->next)	drawAmmo(EnemyAmmo02);
	if(exploRoot[EnemyAmmo03]->next)	drawAmmo(EnemyAmmo03);
	if(exploRoot[EnemyAmmo04]->next)	drawAmmo(EnemyAmmo04);

	if(exploRoot[HeroDestroyed]->next)	drawHeroDestroyed(HeroDestroyed);
	if(exploRoot[HeroDamage]->next)		drawHeroDamage(HeroDamage);
	if(exploRoot[HeroAmmo00]->next)		drawAmmo(HeroAmmo00);
	if(exploRoot[HeroAmmo01]->next)		drawAmmo(HeroAmmo01);
	if(exploRoot[HeroAmmo02]->next)		drawAmmo(HeroAmmo02);

	if(exploRoot[HeroShields]->next)	drawShields(HeroShields);
	if(exploRoot[PowerBurst]->next)		drawBurst(PowerBurst);

	if(exploRoot[AddLife]->next)		drawLife(AddLife);
	if(exploRoot[LoseLife]->next)		drawLife(LoseLife);
	if(exploRoot[ScoreLife]->next)		drawLife(ScoreLife);

	if(exploRoot[Electric]->next)		drawElectric(Electric);
	if(exploRoot[Glitter]->next)		drawGlitter(Glitter);
}

void	Explosions::drawEnemyDamage(ExploType type)
{
	float	age;
	float	ex, ey;
	float	exs, eys;
	float* pos;
	float	clr, tmp;
	float	xoff, yoff;
	Explo* thisExplo;

	thisExplo = exploRoot[type]->next;
	while (thisExplo)
	{
		age = thisExplo->age * game->speedAdj;
		if (age >= 0)
		{
			clr = age / exploStay[type];
			ex = thisExplo->size * exploSize[type][0] * clr;
			ey = thisExplo->size * exploSize[type][1] * clr;
			exs = ex * 0.7;
			eys = ey * 0.7;
			tmp = clr * clr;
			clr = 1.2 - clr;
			tmp = 0.5 + clr * 0.5;
			pos = thisExplo->pos;
			xoff = 0.0;
			yoff = -0.3;

			// tex[EnemyDamage] = Image::load(dataLoc("png/enemyExplo.png"));
			glColor4f(tmp, tmp, tmp, clr);
			AtlasDrawSprite(kShipExplode, pos[0] + xoff, pos[1] + yoff, ex, ey);
		}

		thisExplo = thisExplo->next; //ADVANCE
	}
}

void	Explosions::drawEnemyDestroyed(ExploType type)
{
	float	age;
	float	ex, ey;
	float	exs, eys;
	float* pos;
	float	clr, tmp;
	float	xoff, yoff;
	Explo* thisExplo;

	thisExplo = exploRoot[type]->next;
	while (thisExplo)
	{
		age = thisExplo->age * game->speedAdj;
		if (age >= 0)
		{
			clr = age / exploStay[type];
			ex = thisExplo->size * exploSize[type][0] * clr;
			ey = thisExplo->size * exploSize[type][1] * clr;
			exs = ex * 0.7;
			eys = ey * 0.7;
			tmp = clr * clr;
			clr = 1.2 - clr;
			tmp = 0.5 + clr * 0.5;
			pos = thisExplo->pos;
			xoff = 0.0;
			yoff = -0.3;

			// tex[EnemyDestroyed] = Image::load(dataLoc("png/enemyExplo.png"));
			glColor4f(tmp, tmp, tmp, clr);
			AtlasDrawSprite(kShipExplode, pos[0]+xoff, pos[1]+yoff, ex , ey );
			AtlasDrawSprite(kShipExplode, pos[0] + xoff + 0.1f, pos[1] + yoff + 0.3f, ex, ey);
			AtlasDrawSprite(kShipExplode, pos[0] + xoff - 0.2f, pos[1] + yoff - 0.4f, ex, ey);
		}

		thisExplo = thisExplo->next; //ADVANCE
	}
}

void	Explosions::drawHeroDamage(ExploType type)
{
	float	age;
	float	ex, ey;
	float	exs, eys;
	float* pos;
	float	clr, tmp;
	float	xoff, yoff;
	Explo* thisExplo;

	thisExplo = exploRoot[type]->next;
	while (thisExplo)
	{
		age = thisExplo->age * game->speedAdj;
		if (age >= 0)
		{
			clr = age / exploStay[type];
			ex = thisExplo->size * exploSize[type][0] * clr;
			ey = thisExplo->size * exploSize[type][1] * clr;
			exs = ex * 0.7;
			eys = ey * 0.7;
			tmp = clr * clr;
			clr = 1.2 - clr;
			tmp = 0.5 + clr * 0.5;
			pos = game->hero->pos;
			xoff = 0.0;
			yoff = -0.3;

			// tex[HeroDamage] = Image::load(dataLoc("png/enemyExplo.png"));
			glColor4f(tmp, tmp, tmp, clr);
			AtlasDrawSprite(kShipExplode, pos[0] + xoff, pos[1] + yoff, ex, ey);
		}

		thisExplo = thisExplo->next; //ADVANCE
	}
}

void	Explosions::drawHeroDestroyed(ExploType type)
{
	float	age;
	float	ex, ey;
	float	exs, eys;
	float* pos;
	float	clr, tmp;
	float	xoff, yoff;
	Explo* thisExplo;

	thisExplo = exploRoot[type]->next;
	while (thisExplo)
	{
		age = thisExplo->age * game->speedAdj;
		if (age >= 0)
		{
			clr = age / exploStay[type];
			ex = thisExplo->size * exploSize[type][0] * clr;
			ey = thisExplo->size * exploSize[type][1] * clr;
			exs = ex * 0.7;
			eys = ey * 0.7;
			tmp = clr * clr;
			clr = 1.2 - clr;
			tmp = 0.5 + clr * 0.5;
			pos = game->hero->pos;
			xoff = 0.0;
			yoff = -0.3;

			// tex[HeroDestroyed] = Image::load(dataLoc("png/enemyExplo.png"));
			glColor4f(tmp, tmp, tmp, clr);
			AtlasDrawSprite(kShipExplode, pos[0] + xoff, pos[1] + yoff, ex, ey);
		}

		thisExplo = thisExplo->next; //ADVANCE
	}
}



//----------------------------------------------------------
void	Explosions::drawAmmo(ExploType type)
{
	float	age;
	float	ex, ey;
	float	clr;//,tmp;
	float	*pos;
	Explo	*thisExplo;
	int      ammo_type_index = type - EnemyAmmo00;

	thisExplo = exploRoot[type]->next;
	while(thisExplo)
	{
		age = thisExplo->age*game->speedAdj;
		ex = exploSize[type][0]*((age+5.0f)/(exploStay[type]+5.0f));
		ey = exploSize[type][1]*((age+5.0f)/(exploStay[type]+5.0f));
		clr = age/exploStay[type];
		if( (clr = (1.2-clr)) > 1.0)
			clr = 1.0;
		pos = thisExplo->pos;

		glColor4f(1.0, 1.0, 1.0, clr);
		AtlasDrawSprite(kEnemyAmmoExplode00 + ammo_type_index, pos[0], pos[1], ex, ey);

		thisExplo = thisExplo->next; //ADVANCE
	}
}

//----------------------------------------------------------
void	Explosions::drawBurst(ExploType type)
{
	float	age;
	float	ex, ey;
	float	clr,tmp;
	float	*pos;
	Explo	*thisExplo;

	thisExplo = exploRoot[type]->next;
	while(thisExplo)
	{
		age = thisExplo->age*game->speedAdj;
		tmp = 1.0-age/exploStay[type];
		clr = tmp;
		ex = thisExplo->size*exploSize[type][0]*clr;
		ey = thisExplo->size*exploSize[type][1]*clr;
		clr = tmp*0.75;
		pos = thisExplo->pos;

		glColor4f(clr + 0.5, clr + 0.2, clr + 0.1, clr);
		AtlasDrawSprite(kPowerBurst, pos[0], pos[1], ex, ey);
		AtlasDrawSprite(kPowerBurst, pos[0], pos[1], ex, ey);

		thisExplo = thisExplo->next; //ADVANCE
	}
}

//----------------------------------------------------------
void	Explosions::drawShields(ExploType type)
{
	float	age;
	float	ex, ey;
	float	clr,tmp;
	float	*pos;
	Explo	*thisExplo;

	if(!game->hero->isVisible())
		return;

	thisExplo = exploRoot[type]->next;
	while(thisExplo)
	{
		age = thisExplo->age*game->speedAdj;
		tmp = age/exploStay[type];
		clr = 1.0-tmp;
		tmp = 0.5+(clr*0.5);
		ex = exploSize[type][0]*tmp;
		ey = exploSize[type][1]*tmp;
		pos = game->hero->pos;

		glColor4f(clr, clr, 1.0, clr * 0.7);
		AtlasDrawSprite(kHeroShields, pos[0], pos[1], ex, ey);

		thisExplo = thisExplo->next; //ADVANCE
	}
}

//----------------------------------------------------------
void	Explosions::drawLife(ExploType type)
{
	float	age;
	float	ex, ey;
	float	*pos;
	float	clr[4] = { 1.0, 1.0, 1.0, 1.0 };
	float	tmp;
	Explo	*thisExplo;

	thisExplo = exploRoot[type]->next;
	while(thisExplo)
	{
		age = thisExplo->age*game->speedAdj;
		if(age >= 0)
		{
			pos = thisExplo->pos;
			tmp = age/exploStay[type];
			if(type == AddLife)
			{
				clr[0] = clr[1] = tmp;
				clr[2] = 1.0;
				clr[3] = tmp+0.2;
				tmp = 1.0-tmp;
			}
			else if(type == LoseLife)
			{
				clr[0] = 1.0;
				clr[1] = clr[2] = 0.1;
				clr[3] = 1.0-tmp;
			}
			else
			{
				clr[0] = clr[1] = clr[2] = 1.0;
				clr[3] = 1.0-tmp;
			}
			ex = thisExplo->size*exploSize[type][0]*tmp;
			ey = thisExplo->size*exploSize[type][1]*tmp;

			glColor4fv(clr);
			AtlasDrawSprite(kLife, pos[0], pos[1], ex, ey);
		}

		thisExplo = thisExplo->next; //ADVANCE
	}
}

//----------------------------------------------------------
void	Explosions::drawElectric(ExploType type)
{
	float	age;
	float	ex, ey;
	float	alpha,tmp;
	float	*clr;
	float	*pos;
	Explo	*thisExplo;
	float	tOff;

	thisExplo = exploRoot[type]->next;
	while(thisExplo)
	{
		age = thisExplo->age*game->speedAdj;
		if(age >= 0)
		{
			tmp = age/exploStay[type];
			alpha = 1.0-tmp;
			alpha = 5.0*(alpha*alpha);
			clr = thisExplo->clr;
			ex = exploSize[type][0];
			ey = exploSize[type][1]*tmp;
			tmp = (1.0-game->speedAdj)+(game->speedAdj*1.075);
			thisExplo->vel[0] *= tmp;
			thisExplo->vel[1] *= tmp;
			thisExplo->vel[2] *= tmp;
			pos = thisExplo->pos;
			tOff = FRAND;
			glColor4f(clr[0], clr[1], clr[2], clr[3] * alpha);
			AtlasDrawSprite(kElectric, pos[0], pos[1], ex, ey);
		}
		thisExplo = thisExplo->next; //ADVANCE
	}
}

//----------------------------------------------------------
void	Explosions::drawGlitter(ExploType type)
{
	float	age;
	float	ex, ey;
	float	alpha,tmp;
	float	*clr;
	float	*pos;
	Explo	*thisExplo;

	thisExplo = exploRoot[type]->next;
	while(thisExplo)
	{
		age = thisExplo->age*game->speedAdj;
		if( age >= 0)
		{
			tmp = age/exploStay[type];
			alpha = 1.0-tmp;
			//alpha = 5.0*(alpha*alpha);
			clr = thisExplo->clr;
			tmp = alpha*alpha;
			ex = tmp*thisExplo->size*exploSize[type][0];
			ey = tmp*thisExplo->size*exploSize[type][1]+(0.02*age);
			pos = thisExplo->pos;

			glColor4f(clr[0], clr[1], clr[2], clr[3] * alpha);
			AtlasDrawSprite(kGlitter, pos[0], pos[1], ex, ey);
		}
		thisExplo = thisExplo->next; //ADVANCE
	}
}


//====================================================================
int Explo::exploCount = 0;
Explo::Explo()
{
	age		= 0;
	back	= 0;
	next 	= 0;
	size	= 1;
	exploCount++;
}

Explo::~Explo()
{
	exploCount--;
}

void Explo::init(float p[3], int a, float s)
{
	pos[0]	= p[0];
	pos[1]	= p[1];
	pos[2]	= p[2];
	vel[0]	= 0.0;
	vel[1]	= 0.0;
	vel[2]	= 0.0;
	clr[0]	= 1.0;
	clr[1]	= 1.0;
	clr[2]	= 1.0;
	clr[3]	= 1.0;
	age		= (int)(a/(Global::getInstance()->speedAdj) );
	size	= s;
	back	= 0;
	next 	= 0;
}

void Explo::init(float p[3], float v[3], float c[4], int a, float s)
{
	pos[0]	= p[0];
	pos[1]	= p[1];
	pos[2]	= p[2];
	vel[0]	= v[0];
	vel[1]	= v[1];
	vel[2]	= v[2];
	clr[0]	= c[0];
	clr[1]	= c[1];
	clr[2]	= c[2];
	clr[3]	= c[3];
	age		= (int)(a/(Global::getInstance()->speedAdj) );
	size	= s;
	back	= 0;
	next 	= 0;
}

