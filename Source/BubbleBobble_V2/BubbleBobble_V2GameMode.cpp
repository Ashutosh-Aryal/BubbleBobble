// Copyright Epic Games, Inc. All Rights Reserved.

#include "BubbleBobble_V2GameMode.h"
#include "BubbleBobble_V2Character.h"

ABubbleBobble_V2GameMode::ABubbleBobble_V2GameMode()
{
	// Set default pawn class to our character
	DefaultPawnClass = ABubbleBobble_V2Character::StaticClass();	
}
