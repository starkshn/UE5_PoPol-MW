// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AW.h"
#include "Components/ActorComponent.h"
#include "ClimbSystem.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AW_API UClimbSystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UClimbSystem();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UFUNCTION()
	void ClimbForwardTrace();

	UFUNCTION()
	void ClimbHeightTrace(FVector HitLocation);

	UFUNCTION()
	bool CanPlayerGrabLedge(FHitResult HitResult, FVector FirstHitLocation);

	UFUNCTION()
	void SetCharacterForward(FVector HitLocation, FVector HitresultNormal);


public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character, Meta = (AllowPrivateAccess = true))
	class AAWMyCharacter* CurrentCharacterREF;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CanClimb, Meta = (AllowPrivateAccess = true))
	bool CanClimb;
	
};
