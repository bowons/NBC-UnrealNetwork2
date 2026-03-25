// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DXGameModeBase.generated.h"

class ADXPlayerController;
/**
 * 
 */
UCLASS()
class DEDICATEDX_API ADXGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
#pragma region AGameModeBase Override
public:
	ADXGameModeBase();
	
	virtual void BeginPlay() override;
	
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	
	virtual APlayerController* Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void Logout(AController* Exiting) override;

	virtual void StartPlay() override;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<ADXPlayerController>> AlivePlayerControllers;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<ADXPlayerController>> DeadPlayerControllers;	
	
#pragma endregion AGameModeBase Override
	
public:
	FTimerHandle MainTimerHandle;
	
	UFUNCTION()
	void OnMainTimerElapsed();
	void OnCharacterDead(ADXPlayerController* InController);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 WaitingTime = 15;

	int32 RemainWaitingTimeForPlaying = 15;

	int32 MinimumPlayerCountForPlaying = 2;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 EndingTime = 15;

	int32 RemainWaitingTimeForEnding = 15;
	
private:
	void NotifyToAllPlayer(const FString& NotificationString);
};
