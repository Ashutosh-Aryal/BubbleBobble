// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_MoveToPlayer.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "MyAIController.h"
#include "BubbleBobble_V2Character.h"
#include "Enemy.h"

EBTNodeResult::Type UBTTask_MoveToPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	
	AMyAIController* AI = Cast<AMyAIController>(OwnerComp.GetAIOwner());

	const uint8 enemyID = AI->getEnemyID();
	UBlackboardComponent* blackboardComp = OwnerComp.GetBlackboardComponent();
	auto previousNodeResult = blackboardComp->GetValue<UBlackboardKeyType_Object>(enemyID);

 	ABubbleBobble_V2Character* character = Cast<ABubbleBobble_V2Character>(previousNodeResult);

	if (character) {
		AI->MoveToActor(character, 0, false);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}

