// Fill out your copyright notice in the Description page of Project Settings.

#include "Bubble.h"
#include "Components/SphereComponent.h"
#include "PaperFlipbookComponent.h"
#include "PaperFlipbook.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Enemy.h"

UPaperFlipbook* ABubble::s_AnimAssets[BubbleType::TotalBubbleTypes] = { nullptr, nullptr, nullptr, nullptr };

const TCHAR* ABubble::s_AssetNames[BubbleType::TotalBubbleTypes] = { 
	TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/BubbleBobble/OtherSprites/ShotBubbleAnim.ShotBubbleAnim'"),
	TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/BubbleBobble/OtherSprites/EmptyFloatingBubbleAnim.EmptyFloatingBubbleAnim'"),
	TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/BubbleBobble/OtherSprites/EnemyInBubbleAnim.EnemyInBubbleAnim'"),
	TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/BubbleBobble/OtherSprites/PoppedBubbleAnim.PoppedBubbleAnim'")
};

const FVector ABubble::FORWARD_VELOCITY = 1000.0f * FVector::ForwardVector;
const FVector ABubble::UPWARD_VELOCITY = 60.0f * FVector::UpVector;
const float ABubble::SPHERE_RADIUS = 8.0f;
const FVector ABubble::SCALE_SIZE = FVector(4.0f, 4.0f, 4.0f);
const float ABubble::TIMER_IF_COLLIDES_WITH_ENEMY = 15.0f;
TArray<ABubble*> ABubble::s_ActiveBubbles = TArray<ABubble*>();

// Sets default values
ABubble::ABubble()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	m_CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	m_CollisionComponent->SetSphereRadius(SPHERE_RADIUS);
	m_CollisionComponent->SetCollisionProfileName("Bubble");
	SetRootComponent(m_CollisionComponent);

	m_MoveComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
	m_MoveComponent->ProjectileGravityScale = 0.0f;
	m_MoveComponent->UpdatedComponent = RootComponent;
	m_MoveComponent->bConstrainToPlane = true;
	m_MoveComponent->SetPlaneConstraintNormal(FVector::LeftVector);

	m_PaperFlipbookComponent = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("Animation"));
	m_PaperFlipbookComponent->SetupAttachment(RootComponent);
	m_PaperFlipbookComponent->SetActive(true);

	for (int x = 0; x < BubbleType::TotalBubbleTypes; x++) {
		m_FlipbookAnims[x] = GetAsset((BubbleType) x);
	}

	RootComponent->SetWorldScale3D(SCALE_SIZE);

	bReplicates = true;
	m_CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ABubble::BeginOverlap);
}

UPaperFlipbook* ABubble::GetAsset(BubbleType bt) {

	if (s_AnimAssets[bt] != NULL) {
		return s_AnimAssets[bt];
	}

	ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> asset(s_AssetNames[bt]);
	s_AnimAssets[bt] = asset.Get();
	
	return s_AnimAssets[bt];
}

// Called when the game starts or when spawned
void ABubble::BeginPlay()
{
	Super::BeginPlay();
	s_ActiveBubbles.Add(this);
}

void ABubble::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {

	if (OtherActor->IsA<AEnemy>() && OtherComp->IsA<UCapsuleComponent>() && m_HasReset && m_BubbleType == BubbleType::ShotBubble) {
		m_HasReset = false;
		OtherActor->Destroy();

		FVector spawnLocation = AEnemy::s_SpawnLocation;
		FVector currentPlayerLocation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
		float distance = (currentPlayerLocation - spawnLocation).Size();

		if (distance < 350.0f) {
			spawnLocation.X += 350.0f;
			distance = (currentPlayerLocation - spawnLocation).Size();
		}

		SpawnNewEnemy(spawnLocation);
		m_BubbleType = BubbleType::FloatingBubbleWithEnemy;
		m_AnimationTimer = 10.0f;
	}
}

void ABubble::SpawnNewEnemy(FVector location, bool bSpawnEnragedEnemy) {
	
	FActorSpawnParameters fasp;
	fasp.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	fasp.bNoFail = true;

	AEnemy* enemy = GetWorld()->SpawnActor<AEnemy>(location, GetActorRotation(), fasp);

	if (bSpawnEnragedEnemy) {
		enemy->SetEnemyType(AEnemy::Enraged);
	}
}

int ABubble::Pop(float poppedBubblesCount) {

	s_ActiveBubbles.Remove(this);

	if (poppedBubblesCount != -1) {

		ABubble* adjacentBubble = nullptr;

		float epsilon = 0.5f; // Set epsilon val here
		int x = 0;

		for (; x < s_ActiveBubbles.Num(); ++x) {

			if (!s_ActiveBubbles[x] || s_ActiveBubbles[x]->IsActorBeingDestroyed()) { break; }
			FVector distanceVector = (GetActorLocation() - s_ActiveBubbles[x]->GetActorLocation());
			FVector2D releventCoordinates = FVector2D(distanceVector.X, distanceVector.Z);
			float distance = releventCoordinates.Size();

			if (distance - SPHERE_RADIUS <= epsilon) {
				adjacentBubble = s_ActiveBubbles[x]; break;
			}
		}

		if (x != s_ActiveBubbles.Num()) {
			s_ActiveBubbles.RemoveAt(x);
		}

		bool shouldIncrementPoints = GetBubbleType() == FloatingBubbleWithEnemy;
		m_BubbleType = BubbleType::PoppedBubble;
		int pointsMultiplier = poppedBubblesCount + 1;
		const int ADDED_POINTS = pointsMultiplier * POINTS_PER_BUBBLE;

		Destroy();
		if (!adjacentBubble) {
			return (shouldIncrementPoints) ? ADDED_POINTS : 0;
		}
		
		return (shouldIncrementPoints) ? 
			adjacentBubble->Pop(++poppedBubblesCount) + ADDED_POINTS : 
			adjacentBubble->Pop(poppedBubblesCount);

	} else if (m_BubbleType == FloatingBubbleWithEnemy) {
		SpawnNewEnemy(GetActorLocation(), true);
	}

	m_BubbleType = BubbleType::PoppedBubble;
	Destroy();
	return 0;
}

// Called every frame
void ABubble::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Handle collisions here

	m_AnimationTimer -= DeltaTime;

	if (m_AnimationTimer <= 0.0f) {
		Pop();
	}
	else if (m_BubbleType == BubbleType::ShotBubble) {

		if (m_AnimationTimer <= 10.0f) {
			m_BubbleType = BubbleType::FloatingBubble;
			m_MoveComponent->SetVelocityInLocalSpace(UPWARD_VELOCITY);
		}
		else {
			m_MoveComponent->SetVelocityInLocalSpace(FORWARD_VELOCITY);
		}
	}
	else if (m_BubbleType == BubbleType::FloatingBubbleWithEnemy) {
		m_MoveComponent->SetVelocityInLocalSpace(UPWARD_VELOCITY);
	}

	m_PaperFlipbookComponent->SetFlipbook(m_FlipbookAnims[m_BubbleType]);
	m_PaperFlipbookComponent->Play();
	m_HasReset = true;
	m_LastLocation = GetActorLocation();
}