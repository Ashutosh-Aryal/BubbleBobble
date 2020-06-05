// Copyright Epic Games, Inc. All Rights Reserved.

#include "BubbleBobble_V2Character.h"
#include "PaperFlipbookComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Camera/CameraComponent.h"
#include "PaperFlipbook.h"
#include "Bubble.h"
#include "Enemy.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/KismetSystemLibrary.h"

DEFINE_LOG_CATEGORY_STATIC(SideScrollerCharacter, Log, All);

//////////////////////////////////////////////////////////////////////////
// ABubbleBobble_V2Character

ABubbleBobble_V2Character::ABubbleBobble_V2Character()
{
	// Use only Yaw from the controller and ignore the rest of the rotation.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Set the size of our collision capsule.
	GetCapsuleComponent()->SetCapsuleHalfHeight(10.5f);
	GetCapsuleComponent()->SetCapsuleRadius(10.5f);

	// Create a camera boom attached to the root (capsule)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 500.0f;
	CameraBoom->SocketOffset = FVector(0.0f, 0.0f, 75.0f);
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	

	// Create an orthographic camera (no perspective) and attach it to the boom
	SideViewCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("SideViewCamera"));
	SideViewCameraComponent->ProjectionMode = ECameraProjectionMode::Orthographic;
	SideViewCameraComponent->OrthoWidth = 2048.0f;
	SideViewCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	// Prevent all automatic rotation behavior on the camera, character, and camera component
	CameraBoom->SetUsingAbsoluteRotation(true);
	SideViewCameraComponent->bUsePawnControlRotation = false;
	SideViewCameraComponent->bAutoActivate = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	// Configure character movement
	GetCharacterMovement()->GravityScale = 2.0f;
	GetCharacterMovement()->AirControl = 0.80f;
	GetCharacterMovement()->JumpZVelocity = 1050.0f;
	GetCharacterMovement()->GroundFriction = 3.0f;
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	GetCharacterMovement()->MaxFlySpeed = 600.0f;

	// Lock character motion onto the XZ plane, so the character can't move in or out of the screen
	GetCharacterMovement()->bConstrainToPlane = true;

	GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0.0f, -1.0f, 0.0f));

	// Behave like a traditional 2D platformer character, with a flat bottom instead of a curved capsule bottom
	// Note: This can cause a little floating when going up inclines; you can choose the tradeoff between better
	// behavior on the edge of a ledge versus inclines by setting this to true or false
	GetCharacterMovement()->bUseFlatBaseForFloorChecks = true;

    // 	TextComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("IncarGear"));
    // 	TextComponent->SetRelativeScale3D(FVector(3.0f, 3.0f, 3.0f));
    // 	TextComponent->SetRelativeLocation(FVector(35.0f, 5.0f, 20.0f));
    // 	TextComponent->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
    // 	TextComponent->SetupAttachment(RootComponent);

	// Enable replication on the Sprite component so animations show up when networked
	GetSprite()->SetIsReplicated(true);
	bReplicates = true;

	GetCapsuleComponent()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ABubbleBobble_V2Character::BeginOverlap);
}

void ABubbleBobble_V2Character::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	
	if (OtherActor->IsA<ABubble>()) {
		ABubble* bubble = Cast<ABubble>(OtherActor);

		if (bubble->GetBubbleType() != ABubble::ShotBubble) {
			m_PointsEarned += Cast<ABubble>(OtherActor)->Pop(0);
		}
	}

}

void ABubbleBobble_V2Character::LoseLife() {

	if (m_DeathTimer > 0.0f) { return;  }

	m_NumLives -= 1;
	GetSprite()->SetFlipbook(DeathAnimation);
	m_DeathTimer = 2.0f;
};

void ABubbleBobble_V2Character::BeginPlay() {
	Super::BeginPlay();

	m_SpawnLocation = GetActorLocation();

	FVector enemySpawnLocation = GetActorLocation();
	enemySpawnLocation.X -= 350.0f;

	SpawnActor<AEnemy>(enemySpawnLocation);
}

//////////////////////////////////////////////////////////////////////////
// Animation

void ABubbleBobble_V2Character::UpdateAnimation()
{
	const FVector PlayerVelocity = GetVelocity();
	const float PlayerSpeedSqr = PlayerVelocity.SizeSquared();

	if (m_ShootAnimTimer > 0.0f) {

		if (!m_HasShotBubble) {
			GetSprite()->SetFlipbook(ShootAnimation);
			SpawnActor<ABubble>();
			m_HasShotBubble = true;
		}
	}
	else if (!GetMovementComponent()->IsFalling()) {
		m_HasShotBubble = false;
		UPaperFlipbook* DesiredAnimation = (PlayerSpeedSqr > 0.0f) ? RunningAnimation : IdleAnimation;
		if (GetSprite()->GetFlipbook() != DesiredAnimation)
		{
			GetSprite()->SetFlipbook(DesiredAnimation);
		}
	}
	else {
		m_HasShotBubble = false;
		if (PlayerVelocity.Z > 0) {
			GetSprite()->SetFlipbook(JumpingAnimation); return;
		}

		GetSprite()->SetFlipbook(FallingAnimation);

		bool isLastFrame = GetSprite()->GetPlaybackPositionInFrames() == GetSprite()->GetFlipbookLengthInFrames() - 1;
		if (isLastFrame) {
			GetSprite()->Stop(); // we want the falling animation to stop on the last frame
			return;
		}
	}

	GetSprite()->Play();
}

void ABubbleBobble_V2Character::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (m_NumLives < 0) {
		TEnumAsByte<EQuitPreference::Type> QuitPreference(EQuitPreference::Quit);
		UKismetSystemLibrary::QuitGame(GetWorld(), GetWorld()->GetFirstPlayerController(), QuitPreference, false);
	}

	if (m_DeathTimer > 0.0f) {
		m_DeathTimer -= DeltaSeconds; 

		if (m_DeathTimer <= 0.0f) {

			SetActorLocation(m_SpawnLocation, false, nullptr, ETeleportType::TeleportPhysics);
			GetSprite()->SetFlipbook(IdleAnimation);
		}
		return;
	}

	if (m_shouldShoot) {
		m_ShootAnimTimer = MAX_SHOOT_ANIM_TIMER;
		m_shouldShoot = false;
	}

	m_ShootAnimTimer -= DeltaSeconds;
	
	UpdateAnimation();
	UpdateCharacter(DeltaSeconds);	
}

void ABubbleBobble_V2Character::Shoot() {
	if (m_ShootAnimTimer <= 0.0f) {
		m_shouldShoot = true;
	}
}

template <class T>
void ABubbleBobble_V2Character::SpawnActor(FVector pSpawnLocation) {
	FActorSpawnParameters fasp;
	fasp.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	fasp.bNoFail = true;
	fasp.Owner = this;

	if (pSpawnLocation.Equals(FVector::ZeroVector)) {
		pSpawnLocation = GetActorLocation();
	}

	GetWorld()->SpawnActor<T>(pSpawnLocation, GetActorRotation(), fasp);
}

//////////////////////////////////////////////////////////////////////////
// Input

void ABubbleBobble_V2Character::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Note: the 'Jump' action and the 'MoveRight' axis are bound to actual keys/buttons/sticks in DefaultInput.ini (editable from Project Settings..Input)
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ABubbleBobble_V2Character::PlayerJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ABubbleBobble_V2Character::PlayerStopJumping);
	PlayerInputComponent->BindAction("ShootBubble", IE_Pressed, this, &ABubbleBobble_V2Character::Shoot);
	
	PlayerInputComponent->BindAxis("MoveRight", this, &ABubbleBobble_V2Character::MoveRight);

	PlayerInputComponent->BindTouch(IE_Pressed, this, &ABubbleBobble_V2Character::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ABubbleBobble_V2Character::TouchStopped);
}

void ABubbleBobble_V2Character::PlayerJump() {
	
	if (m_DeathTimer > 0.0f) { return;  }
	Jump();
}

void ABubbleBobble_V2Character::PlayerStopJumping() {
	if (m_DeathTimer > 0.0f) { return; }
	StopJumping();
}

void ABubbleBobble_V2Character::MoveRight(float Value)
{
	/*UpdateChar();*/

	if (m_DeathTimer > 0.0f) { return; }

	// Apply the input to the character motion
	AddMovementInput(FVector(1.0f, 0.0f, 0.0f), Value);
}

void ABubbleBobble_V2Character::TouchStarted(const ETouchIndex::Type FingerIndex, const FVector Location)
{

	// Jump on any touch

	if (m_DeathTimer > 0.0f) { return;  }

	Jump();
}

void ABubbleBobble_V2Character::TouchStopped(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	// Cease jumping once touch stopped

	if (m_DeathTimer > 0.0f) { return; }

	StopJumping();
}

void ABubbleBobble_V2Character::UpdateCharacter(float deltaTime)
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
