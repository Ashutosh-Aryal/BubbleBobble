// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "MyAIController.generated.h"

/**
 *
 */
UCLASS()
class BUBBLEBOBBLE_V2_API AMyAIController : public AAIController
{
	GENERATED_BODY()

	const FName BLACKBOARD_KEY = FName("Target");
	static const TCHAR* s_AssetName;

	/* TODO - Perception
	void OnPerceptionUpdated(TArray<AActor*> pUpdatedActors);

	const float SIGHT_RADIUS = 100.0f;
	const float LOSE_SIGHT_RADIUS = 150.0f;
	const float SIGHT_ANGLE = 130.f;
	**/

protected:

	class UBlackboardComponent* m_BlackboardComponent = nullptr;
	
	class UBehaviorTreeComponent* m_BehaviorTreeComponent = nullptr;
	class UBehaviorTree* m_BehaviorTree = nullptr;

	uint8 m_EnemyID = 0;
	
	/* TODO - Perception
	class UAISenseConfig_Sight* m_AISightConfig = nullptr;
	class UAIPerceptionComponent* m_AIPerceptionComponent = nullptr;
	**/
public:

	AMyAIController();

	void OnPossess(APawn* InPawn) override;

	const uint8& getEnemyID() const { return m_EnemyID; }
};