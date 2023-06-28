/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute
 * it and/or use it and/or modify it under the terms of the
 * "Clarified Artistic License"
 */
#include "EnemyFleet.h"

#include <cstdio>
#include <cmath>

#ifdef HAVE_CONFIG_H
#include <chromium-bsu-config.h>
#endif

#include "compatibility.h"

#if defined(HAVE_APPLE_OPENGL_FRAMEWORK) || defined(HAVE_OPENGL_GL_H)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include "gettext.h"
#include "extern.h"
#include "Ammo.h"
#include "Audio.h"
#include "Global.h"
#include "Config.h"
#include "Explosions.h"
#include "EnemyAmmo.h"
#include "HeroAircraft.h"
#include "ScreenItemAdd.h"
#include "StatusDisplay.h"
#include "Image.h"
#include "Atlas.h"

//====================================================================
EnemyFleet::EnemyFleet()
{
	game = Global::getInstance();

	currentShip = 0;
	float pos[3] = { 0.0, 0.0, 0.0 };
	squadRoot = EnemyAircraft::makeNewEnemy(EnemyStraight, pos);
}

EnemyFleet::~EnemyFleet()
{
	clear();
	delete squadRoot;
}


//----------------------------------------------------------
void	EnemyFleet::clear()
{
	EnemyAircraft	*cur;
	EnemyAircraft *del;

	currentShip = 0;
	cur = squadRoot->next;
	while(cur)
	{
		del = cur;
		cur = cur->next;
		killEnemy(del);
	}
	squadRoot->next = 0;
}

//----------------------------------------------------------
void	EnemyFleet::drawGL()
{
	EnemyAircraft	*thisEnemy;
	thisEnemy = squadRoot->next;
	int num = 0;

	while(thisEnemy)
	{
		num++;

		{
			const float px = thisEnemy->pos[0];
			const float py = thisEnemy->pos[1];
			const float szx = thisEnemy->size[0];
			const float szy = thisEnemy->size[1];

			glColor4f(1.0, 1.0, 1.0, 1.0);
			AtlasDrawSprite(kEnemy00 + thisEnemy->type, px, py, szx, szy);
		}

		switch(thisEnemy->type)
		{
			case EnemyStraight:
				if(thisEnemy->preFire)
				{
					const float px = thisEnemy->pos[0];
					const float py = thisEnemy->pos[1] - 0.9f;
					const float szx = 0.55f * thisEnemy->preFire;
					const float szy = szx + 0.1f;
					
					// extraTex[EnemyStraight] = Image::load(dataLoc("png/enemyAmmo00.png"));
					glColor4f(1.0, 1.0, 1.0, 1.0);
					AtlasDrawSprite(kEnemyAmmo00, px, py, szx, szy);
				}
			 	if(!((thisEnemy->age-192)%256))
				{
					retarget(EnemyGnat, game->hero);
				}
				break;
			case EnemyOmni:
				{
					const float px = thisEnemy->pos[0];
					const float py = thisEnemy->pos[1];
					const float szx = thisEnemy->size[0];
					const float szy = thisEnemy->size[1];
					const float rot = -(thisEnemy->age * 8);

					// 	extraTex[EnemyOmni] = Image::load(dataLoc("png/enemy01-extra.png")
					glColor4f(1.0, 0.0, 0.0, 1.0);
					AtlasDrawSprite(kEnemyExtra01, px, py, szx, szy);

				}
				break;
			case EnemyTank:
				if(thisEnemy->preFire)
				{
				}
				break;
			case EnemyBoss00:
				if(thisEnemy->preFire)
				{
					{
						const float px = thisEnemy->pos[0] + 1.1f;
						const float py = thisEnemy->pos[1] - 0.4f;
						const float szx = 0.4 + 0.6 * thisEnemy->preFire;
						const float szy = szx;

						// extraTex[EnemyBoss00] = Image::load(dataLoc("png/enemy03-extra.png"));
						glColor4f(1.0, 1.0, 1.0, thisEnemy->preFire);
						AtlasDrawSprite(kEnemyExtra03, px, py, szx, szy);

					}

					{
						const float px = thisEnemy->pos[0] - 1.1f;
						const float py = thisEnemy->pos[1] - 0.4f;
						const float szx = 0.4 + 0.6 * thisEnemy->preFire;
						const float szy = szx;

						// extraTex[EnemyBoss00] = Image::load(dataLoc("png/enemy03-extra.png"));
						glColor4f(1.0, 1.0, 1.0, thisEnemy->preFire);
						AtlasDrawSprite(kEnemyExtra03, px, py, szx, szy);

					}
				}
				break;
			case EnemyBoss01:
				if(thisEnemy->preFire)
				{
					if(thisEnemy->shootSwap)
					{
						const float px = thisEnemy->pos[0] - 1.22f;
						const float py = thisEnemy->pos[1] - 1.22f;
						const float szx = 0.9 * thisEnemy->preFire;
						const float szy = szx;

						// extraTex[EnemyBoss01] = Image::load(dataLoc("png/enemyAmmo00.png"));
						glColor4f(1.0, 1.0, 1.0, thisEnemy->preFire);
						AtlasDrawSprite(kEnemyAmmo00, px, py, szx, szy);
						AtlasDrawSprite(kEnemyAmmo00, px, py, szx + 0.2f, szy+0.2f);
					}
					else
					{
						const float px = thisEnemy->pos[0] + 0.55f;
						const float py = thisEnemy->pos[1] - 1.7f;
						const float szx = 0.9 * thisEnemy->preFire;
						const float szy = szx;

						// extraTex[EnemyBoss01] = Image::load(dataLoc("png/enemyAmmo00.png"));
						glColor4f(1.0, 1.0, 1.0, thisEnemy->preFire);
						AtlasDrawSprite(kEnemyAmmo00, px, py, szx, szy);
						AtlasDrawSprite(kEnemyAmmo00, px, py, szx + 0.3f, szy + 0.3f);
					}
				}
			 	if(!((thisEnemy->age-272)%256))
				{
					retarget(EnemyGnat, game->hero);
				}
				break;
			default:
				break;
		}
		thisEnemy = thisEnemy->next;
	}
}

//----------------------------------------------------------
void	EnemyFleet::toFirst()
{
	currentShip = squadRoot->next;
}

//----------------------------------------------------------
EnemyAircraft	*EnemyFleet::getShip()
{
	EnemyAircraft *retVal = currentShip;
	if(currentShip)
		currentShip = currentShip->next;
	return retVal;
}

//----------------------------------------------------------
void	EnemyFleet::retarget(EnemyType et, ScreenItem *newTarget)
{
	EnemyAircraft	*thisEnemy = squadRoot->next;
	while(thisEnemy)
	{
		if(thisEnemy->type == et)
		{
			thisEnemy->target = newTarget;
		}
		thisEnemy = thisEnemy->next;
	}
}

//----------------------------------------------------------
void	EnemyFleet::update()
{
	EnemyAircraft	*thisEnemy;
	EnemyAircraft	*backEnemy;
	EnemyAircraft	*nextEnemy;
	//-- clean up enemies
	thisEnemy = squadRoot->next;

	while(thisEnemy)
	{
		thisEnemy->update();
		float p[3] = { thisEnemy->pos[0], thisEnemy->pos[1], thisEnemy->pos[2] };

		//-------------- Add some damage explosions to the bosses so
		//               we know when they're hurting...
		if((int)thisEnemy->type >= (int)EnemyBoss00)
		{
			float size = thisEnemy->size[0]*0.7;
			float s[2] = { size, size };
			if( thisEnemy->damage > thisEnemy->baseDamage*0.7 )
				if( !(game->gameFrame%18) )
				{
					p[0] = thisEnemy->pos[0] + SRAND*s[0];
					p[1] = thisEnemy->pos[1] + SRAND*s[1];
					p[2] = thisEnemy->pos[2];
					game->explosions->addExplo(Explosions::EnemyDamage, p, 0, 1.0);
				}
			if( thisEnemy->damage > thisEnemy->baseDamage*0.5 )
				if( !(game->gameFrame%10) )
				{
					p[0] = thisEnemy->pos[0] + SRAND*s[0];
					p[1] = thisEnemy->pos[1] + SRAND*s[1];
					p[2] = thisEnemy->pos[2];
					game->explosions->addExplo(Explosions::EnemyDamage, p, 0, 1.0);
				}
			if( thisEnemy->damage > thisEnemy->baseDamage*0.3 )
				if( !(game->gameFrame%4) )
				{
					p[0] = thisEnemy->pos[0] + SRAND*s[0];
					p[1] = thisEnemy->pos[1] + SRAND*s[1];
					p[2] = thisEnemy->pos[2];
					game->explosions->addExplo(Explosions::EnemyDamage, p, 0, 1.0);
				}
		}
		//-------------- Delete enemies that got through...

		if( thisEnemy->pos[1] < -8.0 && thisEnemy->type != EnemyGnat)
			game->statusDisplay->enemyWarning( 1.0-((thisEnemy->pos[1]+14.0)/6.0) );
		if( thisEnemy->pos[1] < -14.0 )
		{
			thisEnemy->damage = 1;
			thisEnemy->age = 0;
			game->hero->loseLife();
			game->tipShipPast++;
		}

		//-------------- If enemies are critically damaged, destroy them...
		if( thisEnemy->damage > 0 )
		{

			backEnemy = thisEnemy->back;
			nextEnemy = thisEnemy->next;
			backEnemy->next = nextEnemy;
			if(nextEnemy)
				nextEnemy->back = backEnemy;

			if(	thisEnemy->age ) //-- set age to 0 for silent deletion...
			{
				switch(thisEnemy->type)
				{
					case EnemyBoss01: //-- BIG explosion for the Boss...
					case EnemyBoss00: //-- BIG explosion for the Boss...
						//-- now for the BIG one...
						destroyAll();
						bossExplosion(thisEnemy);

						if( game->gameMode != Global::HeroDead )
						{
							//--*** TRIGGER END OF LEVEL ***--//
							game->hero->addScore(5000.0);
							game->gameMode = Global::LevelOver;
							game->heroSuccess = 0;
						}
						break;
					case EnemyOmni:
						game->hero->addScore(25.0);
						game->explosions->addExplo(Explosions::EnemyDamage, thisEnemy->pos);
						game->explosions->addExplo(Explosions::EnemyDamage, thisEnemy->pos, -3, 0.7);
						game->explosions->addExplo(Explosions::EnemyAmmo04, thisEnemy->pos);
						game->audio->playSound(Audio::ExploPop, thisEnemy->pos);
						break;
					case EnemyRayGun:
						game->hero->addScore(1000.0);
						game->explosions->addExplo(Explosions::EnemyDestroyed, p);
						p[0] = thisEnemy->pos[0]+0.55;
						game->explosions->addExplo(Explosions::EnemyDestroyed, p, -5, 1.5);
						p[0] = thisEnemy->pos[0]-0.5;
						p[1] = thisEnemy->pos[1]+0.2;
						game->explosions->addExplo(Explosions::EnemyDestroyed, p, -15);
						p[0] = thisEnemy->pos[0];
						game->explosions->addExplo(Explosions::EnemyDestroyed, p, -20, 2.0);
						game->explosions->addExplo(Explosions::EnemyDamage, p, -30, 2.0);
						game->audio->playSound(Audio::Explosion, thisEnemy->pos);
						game->audio->playSound(Audio::ExploBig, thisEnemy->pos);
						break;
					case EnemyTank:
						game->hero->addScore(1500.0);
						p[0] = thisEnemy->pos[0];
						p[1] = thisEnemy->pos[1];
						game->explosions->addExplo(Explosions::EnemyDestroyed, p, -5, 2.5);
						p[0] = thisEnemy->pos[0]-0.9;
						p[1] = thisEnemy->pos[1]-1.0;
						game->explosions->addExplo(Explosions::EnemyDestroyed, p, -0, 1.5);
						p[0] = thisEnemy->pos[0]+1.0;
						p[1] = thisEnemy->pos[1]-0.8;
						game->explosions->addExplo(Explosions::EnemyDestroyed, p, -13, 2.0);
						game->explosions->addExplo(Explosions::EnemyDestroyed, p,  -2, 1.0);
						p[0] = thisEnemy->pos[0]+0.7;
						p[1] = thisEnemy->pos[1]+0.7;
						game->explosions->addExplo(Explosions::EnemyDestroyed, p, -20, 1.7);
						p[0] = thisEnemy->pos[0]-0.7;
						p[1] = thisEnemy->pos[1]+0.9;
						game->explosions->addExplo(Explosions::EnemyDestroyed, p, -8, 1.5);
						game->audio->playSound(Audio::Explosion, thisEnemy->pos);
						game->audio->playSound(Audio::ExploBig, thisEnemy->pos);
						break;
					case EnemyGnat:
						game->hero->addScore(10.0);
						game->explosions->addExplo(Explosions::EnemyAmmo04, thisEnemy->pos);
						game->audio->playSound(Audio::ExploPop, thisEnemy->pos);
						break;
					default:	//-- Add extra Damage explosion delayed for nice bloom
						game->hero->addScore(75.0);
						game->explosions->addExplo(Explosions::EnemyDestroyed, thisEnemy->pos);
						game->explosions->addExplo(Explosions::EnemyDamage, thisEnemy->pos, -15);
						game->audio->playSound(Audio::Explosion, thisEnemy->pos);
						break;
				}
			}

			killEnemy(thisEnemy);

			thisEnemy = backEnemy;
		}
		thisEnemy = thisEnemy->next;
	}
}

//----------------------------------------------------------
void	EnemyFleet::bossExplosion(EnemyAircraft *thisEnemy)
{
	double  a, b, c, d;
	float p[3] = { thisEnemy->pos[0], thisEnemy->pos[1], thisEnemy->pos[2] };
	float xsin,ycos,r;
	float scaleX,scaleY;
	int i;
	float ii;
       int randval;

	for(i = 4; i > 0; i--)
	{
		a = thisEnemy->size[0]*(i*0.2);
		b = thisEnemy->size[1]*(i*0.2);
		p[0] = thisEnemy->pos[0]+a*FRAND;
		p[1] = thisEnemy->pos[1]+b*FRAND;
               randval = (int)(-FRAND*8.0);
		game->explosions->addExplo(Explosions::EnemyDestroyed, p, randval, 1.5+FRAND);
		p[0] = thisEnemy->pos[0]-a*FRAND;
		p[1] = thisEnemy->pos[1]+b*FRAND;
               randval = (int)(-FRAND*8.0);
		game->explosions->addExplo(Explosions::EnemyDestroyed, p, randval, 1.5+FRAND);
		p[0] = thisEnemy->pos[0]+a*FRAND;
		p[1] = thisEnemy->pos[1]-b*FRAND;
               randval = (int)(-FRAND*8.0);
		game->explosions->addExplo(Explosions::EnemyDestroyed, p, randval, 1.5+FRAND);
		p[0] = thisEnemy->pos[0]-a*FRAND;
		p[1] = thisEnemy->pos[1]-b*FRAND;
               randval = (int)(-FRAND*8.0);
		game->explosions->addExplo(Explosions::EnemyDestroyed, p, randval, 1.5+FRAND);
	}

	switch(thisEnemy->type)
	{
		case EnemyBoss00:
			a = 1.55;
			b = 1.56;
			c = 1.58;
			d = 1.50;
			scaleX = 4.0;
			scaleY = 2.0;
			break;
		case EnemyBoss01:
			a = 1.0;
			b = 1.0;
			c = 1.75;
			d = 1.75;
			scaleX = 3.5;
			scaleY = 4.2;
			break;
		default:
			a = b = c = d = 1.0;
			scaleX = scaleY = 1.0;
			break;
	}

	//-- Boss Visual Explosion
	for(i = 0; i < 100; i++)
	{
		ii = i*0.5;
		r = ii/40.0;
		xsin = r*scaleX*sin(ii* a );
		ycos = r*scaleY*cos(ii* b );
		p[0] = thisEnemy->pos[0]+xsin;
		p[1] = thisEnemy->pos[1]+ycos;
		if(!(i%4))
			game->explosions->addExplo(Explosions::EnemyDestroyed, p, (int)(-ii*2.0f), 1.5+FRAND);
		xsin = r*scaleX*sin(i* c );
		ycos = r*scaleY*cos(i* d );
		p[0] = thisEnemy->pos[0]+xsin* 1.5;
		p[1] = thisEnemy->pos[1]-ycos* 1.5;
		if(!(i%5))
		{
			game->explosions->addExplo(Explosions::EnemyDestroyed, p, (int)(-40.0f-ii*3.0f), 1.5+FRAND);
			game->explosions->addExplo(Explosions::EnemyDamage, p, (int)(-60.0f-ii*3.0f), 1.5+FRAND);
		}
		p[0] = thisEnemy->pos[0]+xsin* 1.5;
		p[1] = thisEnemy->pos[1]+ycos* 1.0;
		if(!(i%3))
			game->explosions->addExplo(Explosions::EnemyDestroyed, p, (int)-ii, 1.5+FRAND);
		p[0] = thisEnemy->pos[0]-ycos;
		p[1] = thisEnemy->pos[1]+xsin;
		if(!(i%3))
			game->explosions->addExplo(Explosions::EnemyDestroyed, p, (int)-ii, 1.5+FRAND);
	}

	//-- Boss Audio Explosion
	p[0] = -10.0; p[1] = -5.0;
	game->audio->playSound(Audio::Explosion, p);
	game->audio->playSound(Audio::Explosion, p, -27);
	game->audio->playSound(Audio::ExploPop, p, -45);
	p[0] =  10.0;
	game->audio->playSound(Audio::Explosion, p,  -3);
	game->audio->playSound(Audio::Explosion, p, -25);
	game->audio->playSound(Audio::ExploPop,  p, -40);
	p[0] =  0.0;
	game->audio->playSound(Audio::ExploBig, p,   0);
	game->audio->playSound(Audio::ExploBig, p, -60);
	game->audio->playSound(Audio::ExploPop, p, -55);
	game->audio->playSound(Audio::ExploPop, p, -80);
	game->audio->playSound(Audio::ExploPop, p, -120);
	game->audio->playSound(Audio::ExploPop, p, -160);
}

//----------------------------------------------------------
void	EnemyFleet::destroyAll()
{
	EnemyAircraft	*thisEnemy;

	thisEnemy = squadRoot->next;
	while(thisEnemy)
	{
		thisEnemy->damage = 1;
		thisEnemy = thisEnemy->next;
	}
}

//----------------------------------------------------------
void	EnemyFleet::addEnemy(EnemyAircraft *newEnemy)
{
	EnemyAircraft *first;
	if(newEnemy->over) // insert this enemy after 'over' aircraft in list
	{
		EnemyAircraft *testEnemy = squadRoot->next;
		EnemyAircraft *oldEnemy = newEnemy->over;
		while(testEnemy)
		{
			if(testEnemy == oldEnemy)
			{
				newEnemy->back = oldEnemy;
				newEnemy->next = oldEnemy->next;
				if(testEnemy->next)
				{
					first = testEnemy->next;
					first->back = newEnemy;
				}
				testEnemy->next = newEnemy;
			}
			testEnemy = testEnemy->next;
		}
	}
	else
	{
		newEnemy->back = squadRoot;
		newEnemy->next = squadRoot->next;
		if(squadRoot->next)
		{
			first = squadRoot->next;
			first->back = newEnemy;
		}
		squadRoot->next = newEnemy;
	}

}

//----------------------------------------------------------
void	EnemyFleet::killEnemy(EnemyAircraft *enemy)
{
	game->itemAdd->killScreenItem(enemy);
}
