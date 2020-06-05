// Fill out your copyright notice in the Description page of Project Settings.


#include "JumpTriggerVolume.h"
#include "Enemy.h"
#include "BubbleBobble_V2Character.h"

AJumpTriggerVolume::AJumpTriggerVolume() : Super() {

	OnActorBeginOverlap.AddDynamic(this, &AJumpTriggerVolume::OnOverlapBegin);
	OnActorEndOverlap.AddDynamic(this, &AJumpTriggerVolume::OnOverlapEnd);
}

void AJumpTriggerVolume::OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor) { }

void AJumpTriggerVolume::OnOverlapEnd(AActor* OverlappedActor, AActor* OtherActor) { }