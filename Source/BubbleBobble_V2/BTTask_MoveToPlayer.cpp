// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_MoveToPlayer.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "MyAIController.h"
#include "BubbleBobble_V2Character.h"
#include "Enemy.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PaperFlipbookComponent.h"
#include "PaperFlipbook.h"

EBTNodeResult::Type UBTTask_MoveToPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	
	AMyAIController* AI = Cast<AMyAIController>(OwnerComp.GetAIOwner());

	const uint8 enemyID = AI->getEnemyID();
	UBlackboardComponent* blackboardComp = OwnerComp.GetBlackboardComponent();
	auto previousNodeResult = blackboardComp->GetValue<UBlackboardKeyType_Object>(enemyID);

 	ABubbleBobble_V2Character* character = Cast<ABubbleBobble_V2Character>(previousNodeResult);
	AEnemy* enemy = Cast<AEnemy>(AI->GetCharacter());

	if (character && enemy) {
		UpdateDirection(enemy, character);
		UpdateSpriteRotation(enemy);

		enemy->GetSprite()->SetFlipbook(AEnemy::GetAnimation(enemy->GetEnemyType()));

		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}

void UBTTask_MoveToPlayer::MoveRight(float Value, AEnemy* enemy)
{
	/*UpdateChar();*/

	// Apply the input to the character motion
	enemy->GetCharacterMovement()->Velocity.X = Value;
}

void UBTTask_MoveToPlayer::UpdateDirection(AEnemy* enemy, ABubbleBobble_V2Character* character) {

	bool canJump = !enemy->HasJustJumped();
	if (!canJump) {
		enemy->StopJumping();
		enemy->SetJustJumped(false);
	}

	FVector myLocation = enemy->GetActorLocation();
	FVector targetLocation = character->GetActorLocation();

	FVector2D myReleventCoordinates = FVector2D(myLocation.X, myLocation.Z);
	FVector2D targetReleventCoordinates = FVector2D(targetLocation.X, targetLocation.Z);

	FVector2D targetDirection = targetReleventCoordinates - myReleventCoordinates;
	float distance = targetDirection.Size();
	targetDirection.Normalize();

	if (distance <= 100.0f) {
		MoveRight(m_LastScalar, enemy); return;
	}

	if (canJump) {

		bool allowJumpingLeft = enemy->GetJumping(true); 
		bool allowJumpingRight = enemy->GetJumping(false);

		bool shouldJump = (allowJumpingLeft && targetDirection.X < 0.0f) || (allowJumpingRight && targetDirection.X > 0.0f) && targetDirection.Y > -0.4f;

		if (shouldJump) {
			enemy->Jump();
			enemy->SetJustJumped(true);
		}
	}

	float MOVEMENT_MULTIPLIER = 1.0f;
	
	if (enemy->GetEnemyType() == AEnemy::Enraged) {
		MOVEMENT_MULTIPLIER = 1.2f;
	}

	float scalar = (targetDirection.X > 0.0f) ? MOVEMENT_MULTIPLIER * AEnemy::GetMovementSpeed() : MOVEMENT_MULTIPLIER * -AEnemy::GetMovementSpeed();
	m_LastScalar = scalar;
	MoveRight(scalar, enemy);
}

void UBTTask_MoveToPlayer::UpdateSpriteRotation(AEnemy* enemy)
{
	// Setup the rotation of the controller based on the direction we are traveling
	const FVector PlayerVelocity = enemy->GetCharacterMovement()->Velocity;
	float TravelDirection = PlayerVelocity.X;
	// Set the rotation so that the character faces his direction of travel.
	if (enemy->Controller != nullptr)
	{
		if (TravelDirection < 0.0f)
		{
			enemy->SetActorRotation(FRotator(0.0, 180.0f, 0.0f));
		}
		else if (TravelDirection > 0.0f)
		{
			enemy->SetActorRotation(FRotator(0.0f, 0.0f, 0.0f));
		}
	}
}


