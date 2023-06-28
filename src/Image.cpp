/*
 * Copyright 2008 Tristan Heaven
 * Copyright 2008 Paul Wise
 *
 * "Chromium B.S.U." is free software; you can redistribute
 * it and/or use it and/or modify it under the terms of the
 * "Clarified Artistic License"
 */

#ifdef HAVE_CONFIG_H
#include <chromium-bsu-config.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

#if defined(HAVE_APPLE_OPENGL_FRAMEWORK) || defined(HAVE_OPENGL_GLU_H)
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

#include "Image.h"

// Load image from file and return the texture id
//====================================================================
GLuint Image::load(const char *filename)
{
	GLuint texture = 0;

	SDL_Surface *image = IMG_Load(filename);
	if (!image)
	{
		fprintf(stderr, "Failed to load %s: %s\n", filename, IMG_GetError());
		return texture;
	}

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	SDL_LockSurface( image );

	GLint pack, unpack;
	glGetIntegerv(GL_PACK_ALIGNMENT, &pack);
	glGetIntegerv(GL_UNPACK_ALIGNMENT, &unpack);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexImage2D(GL_TEXTURE_2D, 0, image->format->BytesPerPixel, image->w, image->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->pixels);

	glPixelStorei(GL_PACK_ALIGNMENT, pack);
	glPixelStorei(GL_UNPACK_ALIGNMENT, unpack);

	SDL_UnlockSurface( image );

	SDL_FreeSurface( image );

	return texture;
}
