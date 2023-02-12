// Fill out your copyright notice in the Description page of Project Settings.


#include "AWMyCharacter.h"
#include "AWMyAnimInstance.h"

#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

#include "ClimbSystem.h"

AAWMyCharacter::AAWMyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = GetCapsuleComponent();
	CapsuleComp = GetCapsuleComponent();
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));
	MyClimbSystem = CreateDefaultSubobject<UClimbSystem>(TEXT("ClimbSystem"));

	SpringArm->SetupAttachment(GetCapsuleComponent());
	Camera->SetupAttachment(SpringArm);

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -70.f), FRotator(0.f, -90.f, 0.f));
	SpringArm->TargetArmLength = 400.f;
	SpringArm->SetRelativeRotation(FRotator(-15.f, 0.f, 0.f));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MY_Mesh(TEXT("/Script/Engine.SkeletalMesh'/Game/AWFiles/Character_RootBone/AW_Eve_By_J_Gonzales.AW_Eve_By_J_Gonzales'"));
	if (MY_Mesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(MY_Mesh.Object);
	}

	static ConstructorHelpers::FClassFinder<UAnimInstance> MY_Anim(TEXT("/Script/Engine.AnimBlueprint'/Game/AWFiles/Character_RootBone/BlueprintClass/Anim/BP_AWMyCharacterAnim.BP_AWMyCharacterAnim_C'"));
	if (MY_Anim.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(MY_Anim.Class);
	}

	// GetCharacterMovement()->MaxWalkSpeed = 400.f;

	GetCharacterMovement()->JumpZVelocity = 400.0f;

	PlayerCurState = int32(PlayerStates::OnGround);

	SetControlMode(ControlMode::GTA);
	ArmLengthSpeed = 3.f;
	ArmRotationSpeed = 10.f;
	IsRunning = 0;

	StartingLocation = FVector::ZeroVector;
	LoopingLocation = FVector::ZeroVector;
	EndingLocation = FVector::ZeroVector;
}

void AAWMyCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	ABLOG(Warning, TEXT("AWMyCharacter PostInitializeComponents!"));

	MyAnimInstance = Cast<UAWMyAnimInstance>(GetMesh()->GetAnimInstance());
	if (MyAnimInstance)
	{
		MyAnimInstance->OnMontageEnded.AddDynamic(this, &AAWMyCharacter::OnVaultMontageEnded);
	}
}

void AAWMyCharacter::BeginPlay()
{
	Super::BeginPlay();
	ABLOG(Warning, TEXT("AWMyCharacter BeginPlay!"));
}

void AAWMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, ArmLengthTo, DeltaTime, ArmLengthSpeed);

	switch (CurrentControlMode)
	{
		case ControlMode::DAIBLO:
		{
			FRotator Rotator = FMath::RInterpTo(SpringArm->GetRelativeRotation(), ArmRotationTo, DeltaTime, ArmRotationSpeed);
			SpringArm->SetRelativeRotation(Rotator);
		}
		break;
	}

	switch (CurrentControlMode)
	{
		case ControlMode::DAIBLO:
		{
			if (DirectionToMove.SizeSquared() > 0.f)
			{
				GetController()->SetControlRotation(FRotationMatrix::MakeFromX(DirectionToMove).Rotator());
				AddMovementInput(DirectionToMove);
			}
		}
		break;
	}
}

void AAWMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("UpDown"), this, &AAWMyCharacter::UpDown);
	PlayerInputComponent->BindAxis(TEXT("LeftRight"), this, &AAWMyCharacter::LeftRight);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AAWMyCharacter::LookUp);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AAWMyCharacter::Turn);

	PlayerInputComponent->BindAction(TEXT("ViewChange"), IE_Pressed, this, &AAWMyCharacter::ViewChange);
	PlayerInputComponent->BindAction(TEXT("Run"), IE_Pressed, this, &AAWMyCharacter::RunP);
	PlayerInputComponent->BindAction(TEXT("Run"), IE_Released, this, &AAWMyCharacter::RunR);
	// PlayerInputComponent->BindAction(TEXT("Vault"), IE_Pressed, this, &AAWMyCharacter::Vault);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &AAWMyCharacter::Jump);
}

void AAWMyCharacter::UpDown(float NewAxisValue)
{
	switch (PlayerCurState)
	{
		case int32(PlayerStates::OnGround):
		{
			switch (CurrentControlMode)
			{
			case ControlMode::GTA:
			{
				AddMovementInput(FRotationMatrix(FRotator(0.f, GetControlRotation().Yaw, 0.f)).GetUnitAxis(EAxis::X), NewAxisValue);
			}
			break;
			case ControlMode::DAIBLO:
			{
				DirectionToMove.X = NewAxisValue;
			}
			break;
			}
		}
		break;
		case int32(PlayerStates::Climb):
		{
			AddMovementInput(FRotationMatrix(FRotator(0.f, 0.f, 0.f)).GetUnitAxis(EAxis::Z), NewAxisValue);
		}
		break;
		
	}
}

void AAWMyCharacter::LeftRight(float NewAxisValue)
{
	switch (PlayerCurState)
	{
		case int32(PlayerStates::OnGround):
		{
			switch (CurrentControlMode)
			{
			case ControlMode::GTA:
			{
				AddMovementInput(FRotationMatrix(FRotator(0.f, GetControlRotation().Yaw, 0.f)).GetUnitAxis(EAxis::Y), NewAxisValue);
			}
			break;
			case ControlMode::DAIBLO:
			{
				DirectionToMove.Y = NewAxisValue;
			}
			break;
			}
		}
		break;
		case int32(PlayerStates::Climb):
		{
			AddMovementInput(FRotationMatrix(FRotator(0.f, 0.f, 0.f)).GetUnitAxis(EAxis::Y), NewAxisValue);
		}
		break;
	}
}

void AAWMyCharacter::LookUp(float NewAxisValue)
{
	switch (CurrentControlMode)
	{
		case ControlMode::GTA:
		{
			AddControllerPitchInput(NewAxisValue);
		}
		break;
	}
}

void AAWMyCharacter::Turn(float NewAxisValue)
{
	switch (CurrentControlMode)
	{
		case ControlMode::GTA:
		{
			AddControllerYawInput(NewAxisValue);
		}
		break;
	}
}

void AAWMyCharacter::ViewChange()
{
	switch (CurrentControlMode)
	{
		case ControlMode::GTA:
		{
			GetController()->SetControlRotation(GetActorRotation());
			SetControlMode(ControlMode::DAIBLO);
		}
		break;
		case ControlMode::DAIBLO:
		{
			GetController()->SetControlRotation(SpringArm->GetRelativeRotation());
			SetControlMode(ControlMode::GTA);
		}
		break;
	}
}

void AAWMyCharacter::RunP()
{
	IsRunning = true;
	GetCharacterMovement()->MaxWalkSpeed = 800.f;
}

void AAWMyCharacter::RunR()
{
	IsRunning = false;
	GetCharacterMovement()->MaxWalkSpeed = 400.f;
}


void AAWMyCharacter::Jump()
{
	bPressedJump = true;
}

void AAWMyCharacter::CalDepth(FVector HitLocation)
{
	FVector TempHitLocation = HitLocation;
	float	Distance = 20.f;
	float	Size = 150.f;
	int		DepthLoop = 10;
	int32	FirstHitNum = 0;

	FVector TempStartLocation;
	FVector TempLoopLocation;
	FVector TempEndLocation;

	TempHitLocation.Z = TempHitLocation.Z + 140.f;

	for (int i = 0; i <= DepthLoop; ++i)
	{
		FHitResult HitResult;
		FCollisionQueryParams Params(NAME_None, false, this);
		Params.AddIgnoredActor(this);
		bool bResult = false;

		FVector Dir = GetActorUpVector() * -1;
		FVector Start = TempHitLocation + (GetActorForwardVector() * Distance * i);
		FVector End = Start + Dir * Size;
		FColor DrawColor;

		bResult = GetWorld()->LineTraceSingleByChannel
		(
			OUT HitResult,
			Start,
			End,
			ECollisionChannel::ECC_GameTraceChannel2,
			Params
		);

		if (bResult)
		{
			DrawColor = FColor::Green;
			if (StartingLocation == FVector::ZeroVector)
			{
				// StartLocation
				DrawDebugLine(GetWorld(), HitResult.TraceStart, HitResult.TraceEnd, FColor::White, false, LineTraceLifeTime, LineTracePriority, LineTraceThickness);

				TempStartLocation = HitResult.Location;
				TempStartLocation.Z = TempStartLocation.Z - 4.f;
				// StartLocation
				DrawDebugSphere(GetWorld(), TempStartLocation, 8.f, 10, FColor::White, false, 2.f, 2, 1);

				StartingLocation = TempStartLocation;
			}
			else
			{
				DrawDebugLine(GetWorld(), HitResult.TraceStart, HitResult.TraceEnd, DrawColor, false, LineTraceLifeTime, LineTracePriority, LineTraceThickness);

				TempLoopLocation = HitResult.Location;
				TempLoopLocation.Z = TempLoopLocation.Z - 4.f;

				LoopingLocation = TempLoopLocation;
			}
		}
		else
		{
			DrawColor = FColor::Red;
			DrawDebugLine(GetWorld(), HitResult.TraceStart, HitResult.TraceEnd, DrawColor, false, LineTraceLifeTime, LineTracePriority, LineTraceThickness);

			FHitResult FHitResult;
			FCollisionQueryParams FParams(NAME_None, false, this);
			FParams.AddIgnoredActor(this);

			FVector FStart = HitResult.TraceStart + GetActorForwardVector() * 130.f;
			FVector FEnd = FStart + (GetActorUpVector() * -1) * 500.f;

			bool FbResult = GetWorld()->LineTraceSingleByChannel
			(
				OUT FHitResult,
				FStart, FEnd,
				ECollisionChannel::ECC_GameTraceChannel1,
				FParams
			);

			FColor FDrawColor;
			if (FbResult)
				FDrawColor = FColor::Cyan;
			else
				FDrawColor = FColor::Black;

			if (FbResult)
			{
				// EndLocation Draw
				DrawDebugLine(GetWorld(), FHitResult.TraceStart, FHitResult.TraceEnd, DrawColor, false, LineTraceLifeTime, LineTracePriority, LineTraceThickness);

				TempEndLocation = FHitResult.Location;
				TempEndLocation.Z = TempEndLocation.Z - 4.f;
				

				EndingLocation = TempEndLocation;

				DrawDebugSphere(GetWorld(), EndingLocation, 8.f, 10, FColor::Cyan, false, 2.f, 2, 1);

				break;
			}
		}
	}

	DrawDebugSphere(GetWorld(), TempLoopLocation, 8.f, 10, FColor::Blue, false, 2.f, 2, 1);
}

void AAWMyCharacter::OnVaultMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	StartingLocation = FVector::ZeroVector;
	LoopingLocation = FVector::ZeroVector;
	EndingLocation = FVector::ZeroVector;

	ABLOG(Warning, TEXT("OnVaultMontageEnded!!"));
}

void AAWMyCharacter::SetControlMode(ControlMode Mode)
{
	CurrentControlMode = Mode;

	switch (Mode)
	{
		case ControlMode::GTA:
		{
			// SpringArm->TargetArmLength = 450.f;
			// SpringArm->SetRelativeRotation(FRotator::ZeroRotator);
			ArmLengthTo = 450.f;

			SpringArm->bUsePawnControlRotation = true;
			SpringArm->bInheritPitch = false;
			SpringArm->bInheritYaw = true;
			SpringArm->bInheritRoll = true;
			SpringArm->bDoCollisionTest = true;
			bUseControllerRotationYaw = false;

			// 캐릭터가 부드럽게 회전할 수 있도록
			GetCharacterMovement()->bOrientRotationToMovement = true;
			GetCharacterMovement()->bUseControllerDesiredRotation = false;
			GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);

			GetCharacterMovement()->MaxWalkSpeed = 200.f;
		}
		break;
	case ControlMode::DAIBLO:
		{
			// SpringArm->TargetArmLength = 800.f;
			// SpringArm->SetRelativeRotation(FRotator(-45, 0.f, 0.f));
			ArmLengthTo = 800.f;
			ArmRotationTo = FRotator(-45.f, 0.f, 0.f);

			SpringArm->bUsePawnControlRotation = false;
			SpringArm->bInheritPitch = false;
			SpringArm->bInheritYaw = false;
			SpringArm->bInheritRoll = false;
			SpringArm->bDoCollisionTest = false;

			bUseControllerRotationYaw = false;
			GetCharacterMovement()->bOrientRotationToMovement = false;
			GetCharacterMovement()->bUseControllerDesiredRotation = true;
			GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);

			GetCharacterMovement()->MaxWalkSpeed = 200.f;
		}
		break;
	}
}

