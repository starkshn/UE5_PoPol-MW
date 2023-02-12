// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AW.h"
#include "GameFramework/Character.h"
#include "AWMyCharacter.generated.h"

UCLASS()
class AW_API AAWMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AAWMyCharacter();

public:
	enum class ControlMode
	{
		GTA,
		DAIBLO,
	};

	enum class PlayerStates
	{
		OnGround,
		Climb,
	};

	enum class ClimbState
	{
		CanGrab,
		HangingOnLedge,
		MovingOnLedged,
		ReleasedLedge,
		ClimbUp,
	};
	
protected:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	void UpDown(float NewAxisValue);
	void LeftRight(float NewAxisValue);
	void LookUp(float NewAxisValue);
	void Turn(float NewAxisValue);
	void Jump();

	void ViewChange();
	void RunP();
	void RunR();

public:
	void SetControlMode(ControlMode Mode);

public:
	void CalDepth(FVector HitLocation);

	UFUNCTION()
	void OnVaultMontageEnded(UAnimMontage* Montage, bool bInterrupted);

public:
	UPROPERTY(VisibleAnywhere, Category = Camera)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* SK_Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Character, meta = (AllowPrivateAccess = true))
	UCapsuleComponent* CapsuleComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Anim, meta = (AllowPrivateAccess = true))
	class UAWMyAnimInstance* MyAnimInstance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Climbsystem, meta = (AllowPrivateAccess = true))
	class UClimbSystem* MyClimbSystem;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Movement , meta = (AllowPrivateAccess = true))
	bool IsRunning;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Vault, meta = (AllowPrivateAccess = true))
	FVector StartingLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Vault, meta = (AllowPrivateAccess = true))
	FVector LoopingLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Vault, meta = (AllowPrivateAccess = true))
	FVector EndingLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = PlayerState, meta = (AllowPrivateAccess = true))
	int32 PlayerCurState;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = WorldDirection, meta = (AllowPrivateAccess = true))
	FVector WorldDirection;
	

private:
	ControlMode		CurrentControlMode = ControlMode::GTA;
	FVector			DirectionToMove = FVector::ZeroVector;
	float			ArmLengthTo = 0.0f;
	FRotator		ArmRotationTo = FRotator::ZeroRotator;
	float			ArmLengthSpeed = 0.f;
	float			ArmRotationSpeed = 0.f;

	float			LineTraceLifeTime = 2.f;
	int32			LineTracePriority = 2;
	int32			LineTraceThickness = 1;

	bool			IsJumping = false;

	// Climb
	ClimbState		PlayerClimbState;
};
