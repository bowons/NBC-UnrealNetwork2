// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "DXPlayerCharacter.generated.h"

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
	
#pragma endregion DXPlayerCharacter Components
	
#pragma region Input
	
public:
	float GetCurrentAimPitch() const { return CurrentAimPitch; }
	
private:
	void HandleMoveInput(const FInputActionValue& InValue);
	void HandleLookInput(const FInputActionValue& InValue);
	void HandleLandMineInput(const FInputActionValue& InValue);

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
	
};
