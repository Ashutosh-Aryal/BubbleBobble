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

UPaperFlipbook* AEnemy::s_AnimAssets[EnemyType::TotalEnemyTypes] = { nullptr, nullptr };
const TCHAR* AEnemy::s_AnimNames[EnemyType::TotalEnemyTypes] = {
	TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/BubbleBobble/EnemySprites/NormalEnemyWalkAnim.NormalEnemyWalkAnim'"),
	TEXT("PaperFlipbook'/Game/2DSideScroller/Sprites/BubbleBobble/EnemySprites/EnragedEnemyRunAnim.EnragedEnemyRunAnim'")
};
const TCHAR* AEnemy::s_BehaviorTreeName = TEXT("BehaviorTree'/Game/2DSideScrollerCPP/Blueprints/BT_Enemy.BT_Enemy'");
const FVector AEnemy::SCALE_SIZE = FVector(5.0f, 5.0f, 5.0f);

// Sets default values
AEnemy::AEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	this->AIControllerClass = AMyAIController::StaticClass();
	this->AutoPossessAI = EAutoPossessAI::PlacedInWorld;

	auto value = ConstructorHelpers::FObjectFinderOptional<UBehaviorTree>(s_BehaviorTreeName);
	m_BehaviorTree = value.Get();

	if (!s_AnimAssets[0]) {
		for (int x = 0; x < EnemyType::TotalEnemyTypes; ++x) {
			GetAsset((EnemyType)x);
		}
	}

	m_NormalMovementAnim = s_AnimAssets[EnemyType::Normal];
	m_EnragedMovementAnim = s_AnimAssets[EnemyType::Enraged];

	RootComponent->SetWorldScale3D(SCALE_SIZE);
}

void AEnemy::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Note: the 'Jump' action and the 'MoveRight' axis are bound to actual keys/buttons/sticks in DefaultInput.ini (editable from Project Settings..Input)
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveRight", this, &AEnemy::MoveRight);

	//PlayerInputComponent->BindTouch(IE_Pressed, this, &ABubbleBobble_V2Character::TouchStarted);
	//PlayerInputComponent->BindTouch(IE_Released, this, &ABubbleBobble_V2Character::TouchStopped);
}

void AEnemy::MoveRight(float Value)
{
	/*UpdateChar();*/

	// Apply the input to the character motion
	AddMovementInput(FVector(1.0f, 0.0f, 0.0f), Value);
}

void AEnemy::GetAsset(EnemyType et) {

	ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> asset(s_AnimNames[et]);
	s_AnimAssets[et] = asset.Get();
}

void AEnemy::Setup() {

	GetCapsuleComponent()->SetCapsuleRadius(7.5f);
	GetCapsuleComponent()->SetCapsuleHalfHeight(7.5f);

	UCharacterMovementComponent* Character_Movement = GetCharacterMovement();

	Character_Movement->bOrientRotationToMovement = false;
	Character_Movement->GravityScale = 2.0f;
	Character_Movement->AirControl = 0.80f;
	Character_Movement->JumpZVelocity = 1050.0f;
	Character_Movement->GroundFriction = 3.0f;
	Character_Movement->MaxWalkSpeed = 600.0f;
	Character_Movement->MaxFlySpeed = 600.0f;

	Character_Movement->bConstrainToPlane = true;
	Character_Movement->SetPlaneConstraintNormal(FVector::LeftVector);
	Character_Movement->bUseFlatBaseForFloorChecks = true;

	GetSprite()->SetIsReplicated(true);
	bReplicates = true;

	AMyAIController* NewController = isAIControllerInUWorld();

	if (!NewController) {
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.Instigator = GetInstigator();
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnInfo.OverrideLevel = GetLevel();
		SpawnInfo.ObjectFlags |= RF_Transient;
		NewController = GetWorld()->SpawnActor<AMyAIController>(AIControllerClass, GetActorLocation(), GetActorRotation(), SpawnInfo);
		if (NewController != nullptr)
		{
			// if successful will result in setting this->Controller 
			// as part of possession mechanics
			NewController->Possess(this);
		}
	}
	else {
		NewController->Possess(this);
	}

	Controller = NewController;
}

AMyAIController* AEnemy::isAIControllerInUWorld() {
	for (FConstControllerIterator it = GetWorld()->GetControllerIterator(); it; ++it)
	{
		AMyAIController* AIController = Cast<AMyAIController>(*it);
		if (AIController) {
			return AIController;
		}
	}

	return nullptr;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	ABubbleBobble_V2Character* mainCharacter = Cast<ABubbleBobble_V2Character>(GetWorld()->GetFirstPlayerController()->GetCharacter());

	Setup();
	//GetCapsuleComponent()->SetCapsuleHalfHeight(10.0f);

	if (mainCharacter) {
		m_TargetCharacter = mainCharacter;
		m_SpawnLocation = m_TargetCharacter->GetActorLocation();
		m_SpawnLocation.X -= HORIZONTAL_SPAWN_DISTANCE_FROM_CHARACTER;
	}
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!m_TargetCharacter) {
		m_TargetCharacter = Cast<ABubbleBobble_V2Character>(GetWorld()->GetFirstPlayerController()->GetCharacter());
		m_SpawnLocation = m_TargetCharacter->GetActorLocation();
		m_SpawnLocation.X -= HORIZONTAL_SPAWN_DISTANCE_FROM_CHARACTER;
	}

	UpdateCharacter();
	UpdateAnimation();
}

void AEnemy::UpdateAnimation() {
	if (m_EnemyType == Normal) {
		GetSprite()->SetFlipbook(m_NormalMovementAnim);
	}
	else {
		GetSprite()->SetFlipbook(m_EnragedMovementAnim);
	}
}

void AEnemy::UpdateCharacter()
{
	// Setup the rotation of the controller based on the direction we are traveling
	const FVector PlayerVelocity = GetVelocity();
	float TravelDirection = PlayerVelocity.X;
	// Set the rotation so that the character faces his direction of travel.
	if (Controller != nullptr)
	{
		if (TravelDirection < 0.0f)
		{
			Controller->SetControlRotation(FRotator(0.0, 180.0f, 0.0f));
		}
		else if (TravelDirection > 0.0f)
		{
			Controller->SetControlRotation(FRotator(0.0f, 0.0f, 0.0f));
		}
	}
}


