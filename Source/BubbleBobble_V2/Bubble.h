// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bubble.generated.h"

UCLASS()
class BUBBLEBOBBLE_V2_API ABubble : public AActor
{
	GENERATED_BODY()

	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	bool m_HasReset = true;
	static const int POINTS_PER_BUBBLE = 100;
	FVector m_LastLocation = FVector::ZeroVector;

	static TArray<ABubble*> s_ActiveBubbles;
public:
	// Sets default values for this actor's properties
	ABubble();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	enum BubbleType {
		ShotBubble = 0,
		FloatingBubble = 1,
		FloatingBubbleWithEnemy = 2,
		PoppedBubble = 3,
		TotalBubbleTypes = 4
	};

	const BubbleType GetBubbleType() const { return m_BubbleType; };


	int Pop(float poppedBubblesCount = -1);

protected:

	BubbleType m_BubbleType = BubbleType::ShotBubble;

	static class UPaperFlipbook* GetAsset(enum BubbleType bt);
	static class UPaperFlipbook* s_AnimAssets[BubbleType::TotalBubbleTypes];
	static const TCHAR* s_AssetNames[BubbleType::TotalBubbleTypes];

	static const FVector SCALE_SIZE;
	static const float SPHERE_RADIUS;
	static const FVector FORWARD_VELOCITY;
	static const float TIMER_IF_COLLIDES_WITH_ENEMY;
	static const FVector UPWARD_VELOCITY;
	float m_AnimationTimer = 10.5f;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	class UPaperFlipbook* m_FlipbookAnims[BubbleType::TotalBubbleTypes];

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbookComponent* m_PaperFlipbookComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Collisions)
	class USphereComponent* m_CollisionComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	class UProjectileMovementComponent* m_MoveComponent = nullptr;

	void SpawnNewEnemy(FVector location, bool bSpawnEnragedEnemy = false);
};
