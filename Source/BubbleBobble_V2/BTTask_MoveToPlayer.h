// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_MoveToPlayer.generated.h"

/**
 * 
 */
UCLASS()
class BUBBLEBOBBLE_V2_API UBTTask_MoveToPlayer : public UBTTaskNode
{ 
	GENERATED_BODY()
	void UpdateDirection(class AEnemy* enemy, class ABubbleBobble_V2Character* character);
	void UpdateSpriteRotation(class AEnemy* enemy);

	void MoveRight(float Value, class AEnemy* enemy);

	float m_LastScalar = 0.0f;
public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
