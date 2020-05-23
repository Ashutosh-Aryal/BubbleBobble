// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BubbleBobble_V2Character.h"
#include "Enemy.h"

AMyAIController::AMyAIController(): AAIController() {

	m_BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(FName("Behavior Tree Component"));
	m_BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(FName("Blackboard Component"));

	//m_BehaviorTreeComponent->Add
	
	
	/* TODO Add perceptions later?
	m_AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(FName("AI Perception Component"));

	m_AISightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(FName("AI Sight Config"));
	
	m_AISightConfig->SightRadius = SIGHT_RADIUS;
	m_AISightConfig->LoseSightRadius = LOSE_SIGHT_RADIUS;
	m_AISightConfig->PeripheralVisionAngleDegrees = SIGHT_ANGLE;

	m_AISightConfig->DetectionByAffiliation.bDetectEnemies = true;
	m_AISightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	m_AISightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	m_AIPerceptionComponent->ConfigureSense(*m_AISightConfig);**/
}

void AMyAIController::OnPossess(APawn* InPawn) {

	Super::OnPossess(InPawn);

	AEnemy* enemy = Cast<AEnemy>(InPawn);

	if (enemy && enemy->m_BehaviorTree) {
		m_BlackboardComponent->InitializeBlackboard(*enemy->m_BehaviorTree->BlackboardAsset);
		m_EnemyID = m_BlackboardComponent->GetKeyID(BLACKBOARD_KEY);
		m_BehaviorTreeComponent->StartTree(*enemy->m_BehaviorTree);
	} 
}