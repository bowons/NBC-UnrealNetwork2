// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "DXAnimInstanceBase.generated.h"

class UCharacterMovementComponent;
class ADXPlayerCharacter;
/**
 * 
 */
UCLASS()
class DEDICATEDX_API UDXAnimInstanceBase : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
protected:
	UPROPERTY()
	TObjectPtr<ADXPlayerCharacter> OwnerCharacter;
	
	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> OwnerCharacterMovementComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector Velocity;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float GroundSpeed;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 bShouldMove : 1;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 bIsFalling : 1;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float AimPitch;
};
