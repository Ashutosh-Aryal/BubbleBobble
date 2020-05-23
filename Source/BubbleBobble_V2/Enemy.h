// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "Enemy.generated.h"

UCLASS()
class BUBBLEBOBBLE_V2_API AEnemy : public APaperCharacter
{
	GENERATED_BODY()

	static const FVector SCALE_SIZE;
	void Setup();
public:
	// Sets default values for this character's properties
	AEnemy();

protected:

	enum EnemyType {
		Normal = 0, 
		Enraged = 1, 
		TotalEnemyTypes = 2
	};

	EnemyType m_EnemyType = EnemyType::Normal;

	static void GetAsset(enum EnemyType et);
	static class UPaperFlipbook* s_AnimAssets[EnemyType::TotalEnemyTypes];
	static const TCHAR* s_AnimNames[EnemyType::TotalEnemyTypes];
	static const TCHAR* s_BehaviorTreeName;

	// The animation to play while running around
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* m_NormalMovementAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* m_EnragedMovementAnim;

	class AMyAIController* m_AIController;
	class ABubbleBobble_V2Character* m_TargetCharacter;
	FVector m_SpawnLocation = FVector::ZeroVector;
	const float HORIZONTAL_SPAWN_DISTANCE_FROM_CHARACTER = 10.0f;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	void MoveRight(float Value);
	void UpdateCharacter();
	void UpdateAnimation();

	class AMyAIController* isAIControllerInUWorld();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Behavior Tree")
	class UBehaviorTree* m_BehaviorTree = nullptr;
};
