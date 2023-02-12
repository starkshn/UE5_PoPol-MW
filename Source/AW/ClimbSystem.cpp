// Fill out your copyright notice in the Description page of Project Settings.


#include "ClimbSystem.h"
#include "AWMyCharacter.h"

// Sets default values for this component's properties
UClimbSystem::UClimbSystem()
{
	PrimaryComponentTick.bCanEverTick = true;

	CanClimb = false;
}


// Called when the game starts
void UClimbSystem::BeginPlay()
{
	Super::BeginPlay();

	auto Pawn = GetOwner();
	CurrentCharacterREF = Cast<AAWMyCharacter>(Pawn);
}


// Called every frame
void UClimbSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (CurrentCharacterREF->GetMovementComponent()->IsFalling())
	{
		ClimbForwardTrace();
	}
	else
	{
		CanClimb = false;
	}
}

void UClimbSystem::ClimbForwardTrace()
{
	if (CanClimb) return;

	FHitResult HitResult;
	FCollisionQueryParams Params(NAME_None, false, CurrentCharacterREF);

	float Range = 40.f;
	float Radius = 10.f;

	FVector Start = CurrentCharacterREF->GetActorLocation();
	Start.Z = Start.Z + 70.f;
	FVector End = Start + CurrentCharacterREF->GetActorForwardVector() * Range;

	bool bResult = GetWorld()->SweepSingleByChannel
	(
		OUT HitResult,
		Start,
		End,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(Radius),
		Params
	);

	FVector Vec = CurrentCharacterREF->GetActorForwardVector() * Range;
	FVector Center = Start + Vec * 0.5f;
	float HalfHeight = Range * 0.5f + Radius;
	FQuat Rotation = FRotationMatrix::MakeFromZ(Vec).ToQuat();
	FColor DrawColor;
	if (bResult)
		DrawColor = FColor::Green;
	else
		DrawColor = FColor::Red;

	/*DrawDebugCapsule
	(
		GetWorld(),
		Center, HalfHeight, Radius,
		Rotation, DrawColor, false, 2.f
	);*/

	if (bResult)
	{
		ClimbHeightTrace(HitResult.Location);
		CanClimb = true;
	}
}

void UClimbSystem::ClimbHeightTrace(FVector HitLocation)
{
	float Dis = 40.f;
	bool flag = false;
	FHitResult CanClimbHitResult;

	FVector Vec;
	FVector Center;
	float HalfHeight;
	FQuat Rotation;
	FColor DrawColor;

	for (int i = 0; i < 5; ++i)
	{
		FHitResult HitResult;
		FCollisionQueryParams Params(NAME_None, false, CurrentCharacterREF);

		float Range = 50.f;
		float Radius = 10.f;

		FVector Start = HitLocation;
		Start.Z = Start.Z + i * Dis;
		
		FVector End = Start + CurrentCharacterREF->GetActorForwardVector() * Range;

		bool bResult = GetWorld()->SweepSingleByChannel
		(
			OUT HitResult,
			Start,
			End,
			FQuat::Identity,
			ECollisionChannel::ECC_GameTraceChannel2,
			FCollisionShape::MakeSphere(Radius),
			Params
		);

		Vec = CurrentCharacterREF->GetActorForwardVector() * Range;
		Center = Start + Vec * 0.5f;
		HalfHeight = Range * 0.5f + Radius;
		Rotation = FRotationMatrix::MakeFromZ(Vec).ToQuat();

		if (bResult)
			DrawColor = FColor::Cyan;
		else
			DrawColor = FColor::Orange;

		/*DrawDebugCapsule
		(
			GetWorld(),
			Center, HalfHeight, Radius,
			Rotation, DrawColor, false, 2.f
		);*/

		if (bResult)
		{
			if (i <= 2)
			{
				flag = true;
				CanClimbHitResult = HitResult;
			}

			if (i > 2)
			{
				flag = false;
				break;
			}
			
		}
	}

	if (flag)
	{
		ABLOG(Warning, TEXT("Can Climb"));
	
		CanPlayerGrabLedge(CanClimbHitResult, HitLocation);
	}
	else
	{
		ABLOG(Warning, TEXT("Too Height! Can't Climb"));
	}
}

bool UClimbSystem::CanPlayerGrabLedge(FHitResult HitResult, FVector FirstHitLocation)
{
	float Range = 150.f;
	float Radius = 10.f;

	FVector Start = HitResult.Location;
	Start.Z = Start.Z + 100.f;
	FVector End = Start + CurrentCharacterREF->GetActorUpVector() * -1 * Range;

	FHitResult InHitResult;
	FCollisionQueryParams Params(NAME_None, false, CurrentCharacterREF);

	bool bResult = GetWorld()->SweepSingleByChannel
	(
		OUT InHitResult,
		Start,
		End,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(Radius),
		Params
	);

	FColor DrawColor;

	if (bResult)
	{
		DrawDebugSphere
		(
			GetWorld(),
			InHitResult.Location, 15.f,
			12, FColor::White, false, 3.f, 3, 1.f
		);
		
		
		// SetCharacterForward(HitResult.Location, HitResult.Normal);
	}
	
	return false;
}

void UClimbSystem::SetCharacterForward(FVector HitLocation, FVector HitresultNormal)
{
	FHitResult HitResult;
	FCollisionQueryParams Params(NAME_None, false, CurrentCharacterREF);

	float Range = 200.f;
	float Radius = 10.f;

	FVector Start = HitLocation;
	FVector End = Start + HitresultNormal * Range;

	bool bResult = GetWorld()->SweepSingleByChannel
	(
		OUT HitResult,
		Start,
		End,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(Radius),
		Params
	);

	FVector Vec = HitresultNormal * Range;
	FVector Center = Start + Vec * 0.5f;
	float HalfHeight = Range * 0.5f + Radius;
	FQuat Rotation = FRotationMatrix::MakeFromZ(Vec).ToQuat();
	FColor DrawColor = FColor::White;

	DrawDebugCapsule
	(
		GetWorld(), Center, HalfHeight, Radius, Rotation, DrawColor, false, 10.f, 3, 1.f
	);

	if (bResult)
	{
		/*CurrentCharacterREF->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
		CurrentCharacterREF->GetMovementComponent()->StopMovementImmediately();*/
	}
}


