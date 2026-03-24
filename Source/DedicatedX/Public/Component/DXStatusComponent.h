// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DXStatusComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCurrentHPChangedDelegate, float /*InCurrentHP*/);
DECLARE_MULTICAST_DELEGATE(FOnOutOfCurrentHPDelegate);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMaxHPChangedDelegate, float /*InMaxHP*/)FOnCurrentHPChangedDelegate;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DEDICATEDX_API UDXStatusComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDXStatusComponent();

	float ApplyDamage(float InDamage);
	
	float GetCurrentHP() const { return CurrentHP; }
	void SetCurrentHP(float InCurrentHP);
	
	float GetMaxHP() const { return MaxHP; }
	void SetMaxHP(float InMaxHP);
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
public:
	UPROPERTY(ReplicatedUsing = OnRep_CurrentHP)
	float CurrentHP;
	
	UPROPERTY(ReplicatedUsing = OnRep_MaxHP)
	float MaxHP;
	
	FOnCurrentHPChangedDelegate OnCurrentHPChanged;
	FOnOutOfCurrentHPDelegate OnOutOfCurrentHP;
	FOnMaxHPChangedDelegate OnMaxHPChanged;

protected:
	UFUNCTION()
	void OnRep_CurrentHP();
	
	UFUNCTION()
	void OnRep_MaxHP();
};
