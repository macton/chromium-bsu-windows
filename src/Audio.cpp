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

#include "Audio.h"

#include "Config.h"
#include "Global.h"

#include <sstream>
#include <string>
#include <cstdlib>
#include <cmath>
using namespace std;

//====================================================================
Audio::Audio()
{
	Config	*config = Config::instance();

    fileNames[HeroAmmo00]	= "wav/boom.wav";
	fileNames[PowerUp]		= "wav/power.wav";
	fileNames[Explosion]	= "wav/exploStd.wav";
	fileNames[ExploPop]		= "wav/exploPop.wav";
	fileNames[ExploBig]		= "wav/exploBig.wav";
	fileNames[LoseLife]		= "wav/life_lose.wav";
	fileNames[AddLife]		= "wav/life_add.wav";
	fileNames[MusicGame]	= "wav/music_game.wav";
	fileNames[MusicMenu]	= "wav/music_menu.wav";

	musicMax = 1;
	musicIndex = 0;
}

Audio::~Audio()
{
}

/**
 * NOOP
 */
//----------------------------------------------------------
void	Audio::update()
{
}

/**
 * NOOP
 */
//----------------------------------------------------------
void	Audio::playSound(SoundType, float *, int )
{
}

/**
 * NOOP
 */
//----------------------------------------------------------
void	Audio::initSound()
{
}

/**
 * stop CD track
 */
//----------------------------------------------------------
void	Audio::stopMusic()
{
}

/**
 * pause/resume CDROM playback
 */
//----------------------------------------------------------
void	Audio::pauseGameMusic(bool status)
{
}

/**
 * switch between menu and game music modes
 */
//----------------------------------------------------------
void	Audio::setMusicMode(SoundType mode)
{
}

/**
 * set volume for music channel.
 */
//----------------------------------------------------------
void	Audio::setMusicVolume(float value)
{
}

/**
 * NOOP
 */
//----------------------------------------------------------
void	Audio::setSoundVolume(float)
{
}

/**
 * play CD track at index
 */
//----------------------------------------------------------
void	Audio::setMusicIndex(int index)
{
}

/**
 * skip to next CD track
 */
//----------------------------------------------------------
void	Audio::nextMusicIndex()
{
	int i = musicIndex + 1;
	if(i >= musicMax )
		i = 0;
	setMusicIndex(i);
}

//====================================================================
SoundInfo::SoundInfo()
{
	type = Audio::Explosion;
	pos[0] = 0.0;
	pos[1] = 0.0;
	pos[2] = 0.0;
	age	= 1;
	back = next = 0;
}
SoundInfo::SoundInfo(Audio::SoundType t, float p[3], int a)
{
	type = t;
	pos[0] = p[0];
	pos[1] = p[1];
	pos[2] = p[2];
	age	= a;
	back = next = 0;
}



