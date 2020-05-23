// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_CheckForPlayer.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "BubbleBobble_V2Character.h"
#include "MyAIController.h"
#include "Enemy.h"


UBTService_CheckForPlayer::UBTService_CheckForPlayer() {
	bCreateNodeInstance = true;
}

void UBTService_CheckForPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) {

	AMyAIController* AI_Controller = Cast<AMyAIController>(OwnerComp.GetAIOwner());
	ABubbleBobble_V2Character* character = Cast<ABubbleBobble_V2Character>(GetWorld()->GetFirstPlayerController()->GetCharacter());

	if (AI_Controller && character) {
		OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Object>(AI_Controller->getEnemyID(), character);
	}
}