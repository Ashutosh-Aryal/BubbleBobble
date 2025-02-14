// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "BubbleBobble_V2Character.generated.h"

class UTextRenderComponent;

/**
 * This class is the default character for BubbleBobble_V2, and it is responsible for all
 * physical interaction between the player and the world.
 *
 * The capsule component (inherited from ACharacter) handles collision with the world
 * The CharacterMovementComponent (inherited from ACharacter) handles movement of the collision capsule
 * The Sprite component (inherited from APaperCharacter) handles the visuals
 */
UCLASS(config=Game)
class ABubbleBobble_V2Character : public APaperCharacter
{
	GENERATED_BODY()

		float wtf = 0;

	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** Side view camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess="true"))
	class UCameraComponent* SideViewCameraComponent;

	/** Camera boom positioning the camera beside the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UTextRenderComponent* TextComponent;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = "NumLives")
	int m_NumLives = 3;

	int m_PointsEarned = 0;

	float m_DeathTimer = 0.0f;

	void PlayerJump();
	void PlayerStopJumping();

protected:
	// The animation to play while running around
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Animations)
	class UPaperFlipbook* RunningAnimation;

	// The animation to play while idle (standing still)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* IdleAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* JumpingAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* FallingAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* ShootAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* DeathAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SpawnActor)
	TSubclassOf<class ABubble> m_ActorToSpawn;

	/** Called to choose the correct animation to play based on the character's movement state */
	void UpdateAnimation();

	/** Called for side to side input */
	void MoveRight(float Value);

	void UpdateCharacter(float deltaTime);

	/** Handle touch inputs. */
	void TouchStarted(const ETouchIndex::Type FingerIndex, const FVector Location);

	/** Handle touch stop event. */
	void TouchStopped(const ETouchIndex::Type FingerIndex, const FVector Location);

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	virtual void BeginPlay() override;
	// End of APawn interface

	FVector m_SpawnLocation = FVector::ZeroVector;
	const float MAX_SHOOT_ANIM_TIMER = 0.25f;
	
	float m_ShootAnimTimer = 0.0f;
	bool m_shouldShoot = false;
	bool m_HasShotBubble = false;

	template <class T>
	void SpawnActor(FVector pSpawnLocation = FVector::ZeroVector);

	void Shoot();

public:
	ABubbleBobble_V2Character();

	UFUNCTION(BlueprintCallable, Category="NumLives")
	int getNumLives() const {
		return m_NumLives;
	};

	UFUNCTION(BlueprintCallable, Category="NumLives")
	void setNumLives(const int& pNumLives) {
		m_NumLives = pNumLives;
	};

	UFUNCTION(BlueprintCallable, Category = "NumPoints")
	int getNumPoints() const {
		return m_PointsEarned;
	};

	void LoseLife();

	/** Returns SideViewCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetSideViewCameraComponent() const { return SideViewCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
};
