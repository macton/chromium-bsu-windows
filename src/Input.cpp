#include "Input.h"

uint32_t g_InputPadDown;

void Input_Reset()
{ 
	g_InputPadDown = 0;
}

void Input_Update()
{
}

//
// Temporarily bridge input from existing setup
//
#include <SDL.h>

void KeyDown(SDL_Event* event)
{
	switch (event->key.keysym.sym)
	{
	case SDLK_LEFT:
		g_InputPadDown |= 1 << kInputPadLeft;
		break;
	case SDLK_RIGHT:
		g_InputPadDown |= 1 << kInputPadRight;
		break;
	case SDLK_UP:
		g_InputPadDown |= 1 << kInputPadUp;
		break;
	case SDLK_DOWN:
		g_InputPadDown |= 1 << kInputPadDown;
		break;
	case SDLK_SPACE:
		g_InputPadDown |= 1 << kInputPadButtonPrimary;
		break;
	}	
}

void KeyUp(SDL_Event* event)
{
	switch (event->key.keysym.sym)
	{
	case SDLK_LEFT:
		g_InputPadDown &= ~(1 << kInputPadLeft);
		break;
	case SDLK_RIGHT:
		g_InputPadDown &= ~(1 << kInputPadRight);
		break;
	case SDLK_UP:
		g_InputPadDown &= ~(1 << kInputPadUp);
		break;
	case SDLK_DOWN:
		g_InputPadDown &= ~(1 << kInputPadDown);
		break;
	case SDLK_SPACE:
		g_InputPadDown &= ~(1 << kInputPadButtonPrimary);
		break;
	}
}

void Input_SDL_Bridge_Event(SDL_Event* event)
{
	switch (event->type)
	{
	case SDL_KEYDOWN:
		KeyDown(event);
		break;
	case SDL_KEYUP:
		KeyUp(event);
		break;
	default:
		break;
	}
}

