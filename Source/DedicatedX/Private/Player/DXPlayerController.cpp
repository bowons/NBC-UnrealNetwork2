// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/Player/DXPlayerController.h"

#include "DedicatedX.h"

ADXPlayerController::ADXPlayerController()
{
	DX_LOG_NET(LogDXNet, Log, TEXT(""));
}

void ADXPlayerController::PostNetInit()
{
	DX_LOG_NET(LogDXNet, Log, TEXT("Begin"));

	Super::PostNetInit();
	
	if (IsLocalController() == true)
	{
		UNetDriver* ClientNetDriver = GetNetDriver();
		if (IsValid(ClientNetDriver) == true)
		{
			UNetConnection* ServerConnection = ClientNetDriver->ServerConnection;
			if (IsValid(ServerConnection) == true)
			{
				DX_LOG_NET(LogDXNet, Log, TEXT("Server Connection: %s"), *ClientNetDriver->ServerConnection->GetName());
			}
			else
			{
				DX_LOG_NET(LogDXNet, Log, TEXT("There is no server connection."));
			}
		}
		else
		{
			DX_LOG_NET(LogDXNet, Log, TEXT("ClientNetDriver is invalid."));
		}
	}
	
	DX_LOG_NET(LogDXNet, Log, TEXT("End"));
}

void ADXPlayerController::OnActorChannelOpen(FInBunch& InBunch, UNetConnection* Connection)
{
	DX_LOG_NET(LogDXNet, Log, TEXT("Begin"));

	Super::OnActorChannelOpen(InBunch, Connection);
	
	DX_LOG_NET(LogDXNet, Log, TEXT("End"));
}

void ADXPlayerController::PostInitializeComponents()
{
	DX_LOG_NET(LogDXNet, Log, TEXT("Begin"));

	Super::PostInitializeComponents();
	
	DX_LOG_NET(LogDXNet, Log, TEXT("End"));
}

void ADXPlayerController::BeginPlay()
{
	DX_LOG_NET(LogDXNet, Log, TEXT("Begin"));

	Super::BeginPlay();
	
	DX_LOG_NET(LogDXNet, Log, TEXT("End"));
}

void ADXPlayerController::OnPossess(APawn* InPawn)
{
	DX_LOG_NET(LogDXNet, Log, TEXT("Begin"));

	Super::OnPossess(InPawn);
	// 클라이언트에서는 Possess() 함수가 호출되지 않음에 주의.
	// 그렇다면 클라이언트에서 Owner는 어떻게 초기화 되는 걸까. 
	// AActor::Owner 속성은 ReplicatedUsing 키워드가 달린 속성임. 
	// Onwer가 초기화 되면 OnRep_Owner() 함수가 클라이언트에서 호출됨.
	
	DX_LOG_NET(LogDXNet, Log, TEXT("End"));
}
