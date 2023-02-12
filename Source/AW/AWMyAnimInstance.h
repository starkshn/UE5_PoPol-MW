// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AW.h"
#include "Animation/AnimInstance.h"
#include "AWMyAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class AW_API UAWMyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UAWMyAnimInstance();
	
public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	float CurrentPawnSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	bool IsInAir;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	bool IsDead;
};
