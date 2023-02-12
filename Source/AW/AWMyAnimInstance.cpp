// Fill out your copyright notice in the Description page of Project Settings.


#include "AWMyAnimInstance.h"
#include "AWMyCharacter.h"

UAWMyAnimInstance::UAWMyAnimInstance()
{
	CurrentPawnSpeed = 0.f;
	IsInAir = false;
}

void UAWMyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto Pawn = TryGetPawnOwner();
	if (!::IsValid(Pawn)) return;

	if (!IsDead)
	{
		if (::IsValid(Pawn))
		{
			CurrentPawnSpeed = Pawn->GetVelocity().Size();
			auto Character = Cast<AAWMyCharacter>(Pawn);
			if (Character)
				IsInAir = Character->GetMovementComponent()->IsFalling();
		}
	}
}
