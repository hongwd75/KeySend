#pragma once
#include<string>
#include"mydefines.h"

typedef struct
{
	std::string name;
	int	value;
	KeyBindSTATE what;
} VIRTULKEY_CHAR2VALUE;

extern VIRTULKEY_CHAR2VALUE VIRTULKEY_CHAR2VALUE_list[90];