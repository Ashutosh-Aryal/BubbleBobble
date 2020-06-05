// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerVolume.h"
#include "JumpTriggerVolume.generated.h"

/**
 * 
 */
UCLASS()
class BUBBLEBOBBLE_V2_API AJumpTriggerVolume : public ATriggerVolume
{
	GENERATED_BODY()

public: 
	AJumpTriggerVolume();

	// overlap begin function
	UFUNCTION()
		void OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor);

	// overlap end function
	UFUNCTION()
		void OnOverlapEnd(class AActor* OverlappedActor, class AActor* OtherActor);
};
