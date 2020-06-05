// Fill out your copyright notice in the Description page of Project Settings.
#include "Enemy.h"
#include "AIController.h"
#include "BubbleBobble_V2Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "PaperFlipbookComponent.h"
#include "MyAIController.h"
#include "GameFramework/GameSession.h"
#include "Tasks/AITask.h"
#include "PaperFlipbook.h"
#include "BehaviorTree/BehaviorTree.h"
#include "GameFramework/ProjectileMovementComponent.h"

UPaperFlipbook* AEnemy::s_AnimAssets[EnemyType::TotalEnemyTypes] = { nullptr, nullptr };
const TCHAR* AEnemy::s_AnimNames[EnemyType::TotalEnemyTypes] = {
	TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/BubbleBobble/EnemySprites/NormalEnemyWalkAnim.NormalEnemyWalkAnim'"),
	TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/BubbleBobble/EnemySprites/EnragedEnemyRunAnim.EnragedEnemyRunAnim'")
};
const TCHAR* AEnemy::s_BehaviorTreeName = TEXT("BehaviorTree'/Game/2DSideScrollerCPP/Blueprints/BT_Enemy.BT_Enemy'");
const FVector AEnemy::SCALE_SIZE = FVector(5.0f, 5.0f, 5.0f);
const float AEnemy::MOVEMENT_SPEED = 400.0f;
UBehaviorTree* AEnemy::s_BehaviorTree = nullptr;
FVector AEnemy::s_SpawnLocation = FVector::ZeroVector;
bool AEnemy::s_isFirstEnemy = true;

// Sets default values
AEnemy::AEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	this->AIControllerClass = AMyAIController::StaticClass();
	this->AutoPossessAI = EAutoPossessAI::PlacedInWorld;

	auto value = ConstructorHelpers::FObjectFinderOptional<UBehaviorTree>(s_BehaviorTreeName);
	s_BehaviorTree = value.Get();

	if (!s_AnimAssets[0]) {
		for (int x = 0; x < EnemyType::TotalEnemyTypes; ++x) {
			GetAsset((EnemyType)x);
		}
	}

	m_NormalMovementAnim = s_AnimAssets[EnemyType::Normal];
	m_EnragedMovementAnim = s_AnimAssets[EnemyType::Enraged];

	RootComponent->SetWorldScale3D(SCALE_SIZE);
}

void AEnemy::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {

	if (OtherActor->IsA<ABubbleBobble_V2Character>()) {
		ABubbleBobble_V2Character* character = Cast<ABubbleBobble_V2Character>(OtherActor);
		character->LoseLife();
	}
}

void AEnemy::GetAsset(EnemyType et) {

	ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> asset(s_AnimNames[et]);
	s_AnimAssets[et] = asset.Get();
}

void AEnemy::Setup() {

	if (s_isFirstEnemy) {
		s_SpawnLocation = GetActorLocation();
		s_isFirstEnemy = false;
	}

	UCapsuleComponent* Capsule_Component = GetCapsuleComponent();

	Capsule_Component->SetCapsuleRadius(7.5f);
	Capsule_Component->SetCapsuleHalfHeight(7.5f);
	Capsule_Component->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));
	Capsule_Component->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::BeginOverlap);

	UCharacterMovementComponent* Character_Movement = GetCharacterMovement();

	Character_Movement->bOrientRotationToMovement = false;
	Character_Movement->GravityScale = 2.0f;
	Character_Movement->AirControl = 0.80f;
	Character_Movement->JumpZVelocity = 1100.0f;
	Character_Movement->GroundFriction = 3.0f;
	Character_Movement->MaxWalkSpeed = 600.0f;
	Character_Movement->MaxFlySpeed = 600.0f;

	Character_Movement->bConstrainToPlane = true;
	Character_Movement->SetPlaneConstraintNormal(FVector::LeftVector);
	Character_Movement->bUseFlatBaseForFloorChecks = true;

	GetSprite()->SetIsReplicated(true);
	bReplicates = true;

	if (!m_AIController) {
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.Instigator = GetInstigator();
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnInfo.OverrideLevel = GetLevel();
		SpawnInfo.ObjectFlags = RF_Transient;
		m_AIController = GetWorld()->SpawnActor<AMyAIController>(AIControllerClass, GetActorLocation(), GetActorRotation(), SpawnInfo);
	}

	if (m_AIController != nullptr) {
		m_AIController->Possess(this);
		Controller = m_AIController;
	}
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	Setup();

	ABubbleBobble_V2Character* mainCharacter = Cast<ABubbleBobble_V2Character>(GetWorld()->GetFirstPlayerController()->GetCharacter());
	if (mainCharacter) {
		m_TargetCharacter = mainCharacter;
	}
}

