// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/Animation/DXAnimInstanceBase.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Player/DXPlayerCharacter.h"

void UDXAnimInstanceBase::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
	OwnerCharacter = Cast<ADXPlayerCharacter>(GetOwningActor());
	if (IsValid(OwnerCharacter) == true)
	{
		OwnerCharacterMovementComponent = OwnerCharacter->GetCharacterMovement();
	}
}

void UDXAnimInstanceBase::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	if (IsValid(OwnerCharacter) == false || IsValid(OwnerCharacterMovementComponent) == false)
	{
		return;
	}
	
	Velocity = OwnerCharacterMovementComponent->Velocity;
	GroundSpeed = FVector(Velocity.X, Velocity.Y, 0.f).Size();
	bShouldMove = ((OwnerCharacterMovementComponent->GetCurrentAcceleration().IsNearlyZero()) == false) && (3.f < GroundSpeed);
	bIsFalling = OwnerCharacterMovementComponent->IsFalling();
	
	AimPitch = OwnerCharacter->GetCurrentAimPitch();
}

void UDXAnimInstanceBase::AnimNotify_CheckMeleeAttack()
{
	if (IsValid(OwnerCharacter) == true)
	{
		OwnerCharacter->CheckMeleeAttackHit();
	}
}
