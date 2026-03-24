// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/DXStatusComponent.h"

#include "DedicatedX.h"
#include "Net/UnrealNetwork.h"


UDXStatusComponent::UDXStatusComponent() : CurrentHP(100.f), MaxHP(100.f)
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

float UDXStatusComponent::ApplyDamage(float InDamage)
{
	if (IsValid(GetOwner()) == false || GetOwner()->HasAuthority() == false)
	{
		return 0.f;
	}
	
	const float PreviousHP = CurrentHP;
	const float ActualDamage = FMath::Clamp<float>(InDamage, 0, PreviousHP);
	
	SetCurrentHP(PreviousHP - ActualDamage);
	
	return ActualDamage;
}

void UDXStatusComponent::SetCurrentHP(float InCurrentHP)
{
	if (IsValid(GetOwner()) == false || GetOwner()->HasAuthority() == false)
	{
		return;
	}
	
	CurrentHP = InCurrentHP;
	if (CurrentHP <= KINDA_SMALL_NUMBER)
	{
		CurrentHP = 0.f;
		OnOutOfCurrentHP.Broadcast();
	}
	OnCurrentHPChanged.Broadcast(CurrentHP);
}

void UDXStatusComponent::SetMaxHP(float InMaxHP)
{
	if (IsValid(GetOwner()) == false || GetOwner()->HasAuthority() == false)
	{
		return;
	}
	
	MaxHP = InMaxHP;
	
	if (MaxHP < KINDA_SMALL_NUMBER)
	{
		MaxHP = 0.f;
	}
	OnMaxHPChanged.Broadcast(MaxHP);
}

void UDXStatusComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	//DOREPLIFETIME(ThisClass, CurrentHP);
	DOREPLIFETIME_CONDITION(ThisClass, MaxHP, ELifetimeCondition::COND_OwnerOnly);
}

void UDXStatusComponent::OnRep_CurrentHP()
{
	DX_LOG_SUBOBJECT_ROLE(LogDXNet, Log, TEXT("CurrentHP: %.1f"), CurrentHP);
	OnCurrentHPChanged.Broadcast(CurrentHP);
}

void UDXStatusComponent::OnRep_MaxHP()
{
	DX_LOG_SUBOBJECT_ROLE(LogDXNet, Log, TEXT("MaxHP: %.1f"), MaxHP);
	OnMaxHPChanged.Broadcast(MaxHP);
}
