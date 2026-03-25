// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "DXPlayerCharacter.generated.h"

class UUW_HPText;
class UDXStatusComponent;
class UDXTextWidgetComponent;
class UCameraComponent;
class USpringArmComponent;
class UInputAction;
class UInputMappingContext;

UCLASS()
class DEDICATEDX_API ADXPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

#pragma region ACharacter Override
public:
	ADXPlayerCharacter();
	
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_Owner() override;
	virtual void PostNetInit() override;
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Tick(float DeltaTime) override;
	
#pragma endregion ACharacter Override
	
#pragma region DXPlayerCharacter Components
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DXPlayerCharacter|Components")
	TObjectPtr<USpringArmComponent> SpringArm;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DXPlayerCharacter|Components")
	TObjectPtr<UCameraComponent> Camera;
public:
	FORCEINLINE USpringArmComponent* GetSpringArm() const { return SpringArm; }
	FORCEINLINE UCameraComponent* GetCamera() const { return Camera; }
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DXPlayerCharacter|Components")
	TObjectPtr<UDXStatusComponent> StatusComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DXPlayerCharacter|Components")
	TObjectPtr<UDXTextWidgetComponent> HPTextWidgetComponent;
	
#pragma endregion DXPlayerCharacter Components
	
#pragma region Input
	
public:
	float GetCurrentAimPitch() const { return CurrentAimPitch; }
	
private:
	void HandleMoveInput(const FInputActionValue& InValue);
	void HandleLookInput(const FInputActionValue& InValue);
	void HandleLandMineInput(const FInputActionValue& InValue);
	void HandleMeleeAtackInput(const FInputActionValue& InValue);

	UFUNCTION(Server, Unreliable) // 한 두번 정도는 씹혀도 되기 때문.
	void ServerRPCUpdateAimValue(const float& InAimPitchValue);
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DXPlayerCharacter|Input")
	TObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DXPlayerCharacter|Input")
	TObjectPtr<UInputAction> MoveAction;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DXPlayerCharacter|Input")
	TObjectPtr<UInputAction> LookAction;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DXPlayerCharacter|Input")
	TObjectPtr<UInputAction> JumpAction;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DXPlayerCharacter|Input")
	TObjectPtr<UInputAction> LandMineAction;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DXPlayerCharacter|Input")
	TObjectPtr<UInputAction> MeleeAttackAction;
	
	UPROPERTY(Replicated)
	float CurrentAimPitch = 0.f;

	float PreviousAimPitch = 0.f;
	
#pragma endregion Input

#pragma region LandMine
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AActor> LandMineClass;
	
private:
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPCSpawnLandMine();
	
#pragma endregion LandMine

#pragma region Attack
	
public:
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	void CheckMeleeAttackHit();

private:
	void DrawDebugMeleeAttack(const FColor& DrawColor, FVector TraceStart, FVector TraceEnd, FVector Forward);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPCMeleeAttack(float InStartMeleeAttackTime);
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCMeleeAttack();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPCPerformMeleeHit(ACharacter* InDamagedCharacters, float InCheckTime);
	
	UFUNCTION(Client, Unreliable)
	void ClientRPCPlayMeleeAttackMontage(ADXPlayerCharacter* InTargetCharacter);
	
	UFUNCTION()
	void OnRep_CanAttack();
	
	void PlayMeleeAttackMontage() const;
	
	UFUNCTION()
	void OnDeath();
	
protected:
	//bool bCanAttack;
	UPROPERTY(ReplicatedUsing = OnRep_CanAttack)
	uint8 bCanAttack : 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> MeleeAttackMontage;

	float MeleeAttackMontagePlayTime;

	float LastStartMeleeAttackTime;
	float MeleeAttackTimeDifference;
	
	float MinAllowedTimeForMeleeAttack;
	
#pragma endregion Attack
	
#pragma region Widget
public:
	void SetHPTextWidget(UUW_HPText* InHPTextWidget);
	void TakeBuff(float InBuffValue);
	
#pragma endregion Widget
	
};
