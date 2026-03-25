// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/Core/DXGameModeBase.h"

#include "DedicatedX.h"
#include "Core/DXGameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Player/DXPlayerController.h"

ADXGameModeBase::ADXGameModeBase()
{
	// DX_LOG_NET(LogDXNet, Log, TEXT(""));
}

void ADXGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetTimerManager().SetTimer(MainTimerHandle, this, &ThisClass::OnMainTimerElapsed, 1.0f, true);
	
	RemainWaitingTimeForPlaying = WaitingTime;
}

void ADXGameModeBase::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId,
                               FString& ErrorMessage)
{
	// DX_LOG_NET(LogDXNet, Log, TEXT("Begin"));

	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
	
	//ErrorMessage = TEXT("The server is currently full. Please try again later.");
	// DX_LOG_NET(LogDXNet, Log, TEXT("End"));

}

APlayerController* ADXGameModeBase::Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal,
	const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	// DX_LOG_NET(LogDXNet, Log, TEXT("Begin"));
	
	APlayerController* LoginPlayerController = Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);

	// DX_LOG_NET(LogDXNet, Log, TEXT("End"));

	return LoginPlayerController;
}

void ADXGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	ADXPlayerController* NewPlayerController = Cast<ADXPlayerController>(NewPlayer);
	if (IsValid(NewPlayerController) == true)
	{
		AlivePlayerControllers.Add(NewPlayerController);
		
		NewPlayerController->NotificationText = FText::FromString(TEXT("Connected to the game server."));
	}
	
	ADXGameStateBase* DXGameState = GetGameState<ADXGameStateBase>();
	
	if (IsValid(DXGameState) == false) 	return;
	if (DXGameState->MatchState != EMatchState::Waiting)
	{
		//이 방법은 Hack 같은 방법입니다. 정석적인 방법은 아닙니다. 실 구현에서 일케 하지마십쇼
		NewPlayer->SetLifeSpan(0.1f);
		return;
	}
}

void ADXGameModeBase::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	
	ADXPlayerController* ExitingPlayerController = Cast<ADXPlayerController>(Exiting);
	if (IsValid(ExitingPlayerController) == true && AlivePlayerControllers.Find(ExitingPlayerController) != INDEX_NONE)
	{
		AlivePlayerControllers.Remove(ExitingPlayerController);
		DeadPlayerControllers.Add(ExitingPlayerController);
	}
}

void ADXGameModeBase::StartPlay()
{
	// DX_LOG_NET(LogDXNet, Log, TEXT("Begin"));

	Super::StartPlay();
	
	// DX_LOG_NET(LogDXNet, Log, TEXT("Begin"));
}

void ADXGameModeBase::OnMainTimerElapsed()
{
	ADXGameStateBase* DXGameState = GetGameState<ADXGameStateBase>();
	if (IsValid(DXGameState) == false)
	{
		return;
	}
	
	switch (DXGameState->MatchState)
	{
		case EMatchState::None:
			break;
		case EMatchState::Waiting:
			{
				FString NotificationString = FString::Printf(TEXT(""));

				if (AlivePlayerControllers.Num() < MinimumPlayerCountForPlaying)
				{
					NotificationString = FString::Printf(TEXT("Wait another players for playing."));

					RemainWaitingTimeForPlaying = WaitingTime; // 최소인원이 안된다면 대기 시간 초기화.
				}
				else
				{
					NotificationString = FString::Printf(TEXT("Wait %d seconds for playing."), RemainWaitingTimeForPlaying);

					--RemainWaitingTimeForPlaying;
				}

				if (RemainWaitingTimeForPlaying <= 0)
				{
					NotificationString = FString::Printf(TEXT(""));

					DXGameState->MatchState = EMatchState::Playing;
				}

				NotifyToAllPlayer(NotificationString);
			}
			break;
		case EMatchState::Playing:
			{
				DXGameState->AlivePlayerControllerCount = AlivePlayerControllers.Num();
			
				FString NotificationString = FString::Printf(TEXT("%d / %d"), DXGameState->AlivePlayerControllerCount, DXGameState->AlivePlayerControllerCount + DeadPlayerControllers.Num());
				NotifyToAllPlayer(NotificationString);
			
				if (DXGameState->AlivePlayerControllerCount <= 1)
				{
					DXGameState->MatchState = EMatchState::Ending;

					AlivePlayerControllers[0]->ClientRPCShowGameResultWidget(1);
				}
			}
			break;
		case EMatchState::Ending:
			{
				FString NotificationString = FString::Printf(TEXT("Waiting %d for returning to title."), RemainWaitingTimeForEnding);

				NotifyToAllPlayer(NotificationString);

				--RemainWaitingTimeForEnding;

				if (RemainWaitingTimeForEnding <= 0)
				{
					for (auto AliveController : AlivePlayerControllers)
					{
						AliveController->ClientRPCReturnToTitle();
					}
					for (auto DeadController : DeadPlayerControllers)
					{
						DeadController->ClientRPCReturnToTitle();
					}

					MainTimerHandle.Invalidate();
					
					FName CurrentLevelName = FName(UGameplayStatics::GetCurrentLevelName(this));
					UGameplayStatics::OpenLevel(this, CurrentLevelName, true, FString(TEXT("listen")));
					return;
				}

				break;
			}
		case EMatchState::End:
			break;
		default:
			break;
	}
}

void ADXGameModeBase::OnCharacterDead(ADXPlayerController* InController)
{
	if (IsValid(InController) == false || AlivePlayerControllers.Find(InController) == INDEX_NONE)
	{
		return;
	}
	
	InController->ClientRPCShowGameResultWidget(AlivePlayerControllers.Num());

	AlivePlayerControllers.Remove(InController);
	DeadPlayerControllers.Add(InController);
}

void ADXGameModeBase::NotifyToAllPlayer(const FString& NotificationString)
{
	for (auto AlivePlayerController : AlivePlayerControllers)
	{
		AlivePlayerController->NotificationText = FText::FromString(NotificationString);
	}

	for (auto DeadPlayerController : DeadPlayerControllers)
	{
		DeadPlayerController->NotificationText = FText::FromString(NotificationString);
	}
}
