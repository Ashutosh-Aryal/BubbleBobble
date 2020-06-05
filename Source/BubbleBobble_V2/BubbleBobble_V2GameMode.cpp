// Copyright Epic Games, Inc. All Rights Reserved.

#include "BubbleBobble_V2GameMode.h"
#include "BubbleBobble_V2Character.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

ABubbleBobble_V2GameMode::ABubbleBobble_V2GameMode(): Super()
{
	// Set default pawn class to our character
	DefaultPawnClass = ABubbleBobble_V2Character::StaticClass();

	static ConstructorHelpers::FClassFinder<UUserWidget> HUDClassFinder(TEXT("WidgetBlueprint'/Game/2DSideScrollerCPP/Blueprints/InGameHUD'"));
	HUDClass = HUDClassFinder.Class;
}