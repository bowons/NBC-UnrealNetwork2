// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DXPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class DEDICATEDX_API ADXPlayerController : public APlayerController
{
	GENERATED_BODY()
	
#pragma region APlayerController Override
public:
	ADXPlayerController();
	
	virtual void PostNetInit() override;
	virtual void OnActorChannelOpen(FInBunch& InBunch, UNetConnection* Connection) override;
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	
	virtual void OnPossess(APawn* InPawn) override;
	
#pragma endregion APlayerController Override
};
