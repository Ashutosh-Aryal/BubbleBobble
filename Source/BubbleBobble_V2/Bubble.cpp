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
}

void ABubble::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {

	if (OtherActor->IsA<AEnemy>() && OtherComp->IsA<UCapsuleComponent>() && m_HasReset && m_BubbleType == BubbleType::ShotBubble) {
		m_HasReset = false;
		OtherActor->Destroy();
		SpawnNewEnemy();
		m_BubbleType = BubbleType::FloatingBubbleWithEnemy;
		m_AnimationTimer = 10.0f;
	}
}

void ABubble::SpawnNewEnemy() {
	FActorSpawnParameters fasp;
	fasp.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	fasp.bNoFail = true;
	fasp.Owner = this;

	auto mainCharacter = GetWorld()->GetFirstPlayerController()->GetPawn();

	FVector location = mainCharacter->GetActorLocation();
	location.X += 100.0f;

	GetWorld()->SpawnActor<AEnemy>(location, mainCharacter->GetActorRotation(), fasp);
}

// Called every frame
void ABubble::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Handle collisions here

	m_AnimationTimer -= DeltaTime;

	if (m_AnimationTimer <= 0.0f) {
		m_BubbleType = BubbleType::PoppedBubble;
		Destroy();
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
}