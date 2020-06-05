// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "Enemy.generated.h"

UCLASS()
class BUBBLEBOBBLE_V2_API AEnemy : public APaperCharacter
{
	GENERATED_BODY()

	static const float MOVEMENT_SPEED;
	static const FVector SCALE_SIZE;
	FVector m_LastLocation = FVector::ZeroVector;
	float m_LastScalar = 0;
	bool m_JustJumped = false;
	bool m_AllowJumpingLeft = false;
	bool m_AllowJumpingRight = false;
	void Setup();

	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
public:
	// Sets default values for this character's properties
	AEnemy();
	static const float GetMovementSpeed() { return MOVEMENT_SPEED; };
	enum EnemyType {
		Normal = 0,
		Enraged = 1,
		TotalEnemyTypes = 2
	};

	static class UPaperFlipbook* GetAnimation(EnemyType et) {
		return s_AnimAssets[et];
	};

	const EnemyType GetEnemyType() const { return m_EnemyType; };
	void SetEnemyType(EnemyType et) {
		m_EnemyType = et;
	};

	const bool HasJustJumped() const {
		return m_JustJumped;
	};
	void SetJustJumped(bool bJustJumped) {
		m_JustJumped = bJustJumped;
	}

	bool GetJumping(bool getJumpingLeft) {
		if (getJumpingLeft) { return m_AllowJumpingLeft; };
		return m_AllowJumpingRight;
	}

	UFUNCTION(BlueprintCallable, Category = "HandleJumps")
	void SetJumping(bool pAllowLeftJumps, bool pAllowRightJumps) {
		m_AllowJumpingLeft = pAllowLeftJumps;
		m_AllowJumpingRight = pAllowRightJumps;
	};

protected:

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
	const float HORIZONTAL_SPAWN_DISTANCE_FROM_CHARACTER = 350.0f;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveRight(float Value);

	void UpdateSpriteRotation();
	void UpdateAnimation();
	void UpdateDirection();

public:	
	static class UBehaviorTree* s_BehaviorTree;
	static FVector s_SpawnLocation;
	static bool s_isFirstEnemy;
};
