/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute
 * it and/or use it and/or modify it under the terms of the
 * "Clarified Artistic License"
 */
#ifndef ScreenItem_h
#define ScreenItem_h

#include <stdio.h>
#include <math.h>

class ScreenItem
{
public:
	enum ItemType { ItemHero, ItemEnemy, ItemPowerUp };

	ScreenItem(ItemType);
	virtual ~ScreenItem();

	virtual void init();

	virtual float	*getPos();
	float	pos[3];
	float	vel[3];

	ItemType	itemType() { return screenItemType; }

	ScreenItem *next;
	ScreenItem *back; 

	char description[128];
	const char* DescriptionString()
	{
		int start_x = 10 + (int)floorf(pos[0]);
		// sprintf(description, "--------------------");
		// description[start_x] = '*';
		sprintf(description, "%d", start_x);
		// sprintf(description, "age: %d pos: %f, %f vel: %f, %f", age, pos[0], pos[1], vel[0], vel[1]);
		return description;
	}

	virtual const char* NameString()
	{
		return "ScreenItem";
	}


protected:
	int		age;

private:
	ItemType	screenItemType;

	static int	itemCount;
};


#endif // ScreenItem_h
