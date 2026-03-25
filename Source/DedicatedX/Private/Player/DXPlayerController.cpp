// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/Player/DXPlayerController.h"

#include "DedicatedX.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Core/DXGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "UI/UW_GameResult.h"

ADXPlayerController::ADXPlayerController()
{
	// DX_LOG_NET(LogDXNet, Log, TEXT(""));
}

void ADXPlayerController::PostNetInit()
{
	// DX_LOG_NET(LogDXNet, Log, TEXT("Begin"));

	Super::PostNetInit();
	
	// if (IsLocalController() == true)
	// {
	// 	UNetDriver* ClientNetDriver = GetNetDriver();
	// 	if (IsValid(ClientNetDriver) == true)
	// 	{
	// 		UNetConnection* ServerConnection = ClientNetDriver->ServerConnection;
	// 		if (IsValid(ServerConnection) == true)
	// 		{
	// 			// DX_LOG_NET(LogDXNet, Log, TEXT("Server Connection: %s"), *ClientNetDriver->ServerConnection->GetName());
	// 		}
	// 		else
	// 		{
	// 			// DX_LOG_NET(LogDXNet, Log, TEXT("There is no server connection."));
	// 		}
	// 	}
	// 	else
	// 	{
	// 		// DX_LOG_NET(LogDXNet, Log, TEXT("ClientNetDriver is invalid."));
	// 	}
	// }
	
	// DX_LOG_NET(LogDXNet, Log, TEXT("End"));
}

void ADXPlayerController::OnActorChannelOpen(FInBunch& InBunch, UNetConnection* Connection)
{
	// DX_LOG_NET(LogDXNet, Log, TEXT("Begin"));

	Super::OnActorChannelOpen(InBunch, Connection);
	
	// DX_LOG_NET(LogDXNet, Log, TEXT("End"));
}

void ADXPlayerController::PostInitializeComponents()
{
	// DX_LOG_NET(LogDXNet, Log, TEXT("Begin"));

	Super::PostInitializeComponents();
	
	// DX_LOG_NET(LogDXNet, Log, TEXT("End"));
}

void ADXPlayerController::BeginPlay()
{
	// DX_LOG_NET(LogDXNet, Log, TEXT("Begin"));
	Super::BeginPlay();
	
	if (IsLocalController() == false)
	{
		return;
	}
	
	FInputModeGameOnly GameOnly;
	SetInputMode(GameOnly);
	
	if (IsValid(NotificationTextUIClass) == true)
	{
		UUserWidget* NotificationTextUI = CreateWidget<UUserWidget>(this, NotificationTextUIClass);
		if (IsValid(NotificationTextUI) == true)
		{
			NotificationTextUI->AddToViewport(1);
			NotificationTextUI->SetVisibility(ESlateVisibility::Visible);
		}
	}
	
	// DX_LOG_NET(LogDXNet, Log, TEXT("End"));
}

void ADXPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, NotificationText);
}

void ADXPlayerController::OnPossess(APawn* InPawn)
{
	// DX_LOG_NET(LogDXNet, Log, TEXT("Begin"));

	Super::OnPossess(InPawn);
	// 클라이언트에서는 Possess() 함수가 호출되지 않음에 주의.
	// 그렇다면 클라이언트에서 Owner는 어떻게 초기화 되는 걸까. 
	// AActor::Owner 속성은 ReplicatedUsing 키워드가 달린 속성임. 
	// Onwer가 초기화 되면 OnRep_Owner() 함수가 클라이언트에서 호출됨.
	
	// DX_LOG_NET(LogDXNet, Log, TEXT("End"));
}

void ADXPlayerController::OnCharacterDead()
{
	ADXGameModeBase* GameMode = Cast<ADXGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (HasAuthority() == true && IsValid(GameMode) == true)
	{
		GameMode->OnCharacterDead(this);
	}
}

void ADXPlayerController::ClientRPCReturnToTitle_Implementation()
{
	if (IsLocalController() == true)
	{ // 서버의 레벨이 변경되는걸 원치 않음. 클라이언트가 이동해야하므로 if() 처리.
		UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("Title")), true);
	}
}

void ADXPlayerController::ClientRPCShowGameResultWidget_Implementation(int32 InRanking)
{
	if (IsLocalController() == true)
	{
		UUW_GameResult* GameResultUI = CreateWidget<UUW_GameResult>(this, GameResultUIClass);
		if (IsValid(GameResultUI) == true)
		{
			GameResultUI->AddToViewport(3);
			
			FString GameResultString = FString::Printf(TEXT("%s"), InRanking == 1 ? TEXT("Winner Winner!") : TEXT("Looser..."));
			GameResultUI->ResultText->SetText(FText::FromString(GameResultString));
				
			FString RankingString = FString::Printf(TEXT("#%02d"), InRanking);
			GameResultUI->RankingText->SetText(FText::FromString(RankingString));
			
			FInputModeUIOnly Mode;
			Mode.SetWidgetToFocus(GameResultUI->GetCachedWidget());
			SetInputMode(Mode);

			bShowMouseCursor = true;
		}
	}
}
