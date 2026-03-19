// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/Core/DXGameModeBase.h"

#include "DedicatedX.h"

ADXGameModeBase::ADXGameModeBase()
{
	DX_LOG_NET(LogDXNet, Log, TEXT(""));
}

void ADXGameModeBase::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId,
	FString& ErrorMessage)
{
	DX_LOG_NET(LogDXNet, Log, TEXT("Begin"));

	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
	
	//ErrorMessage = TEXT("The server is currently full. Please try again later.");
	DX_LOG_NET(LogDXNet, Log, TEXT("End"));

}

APlayerController* ADXGameModeBase::Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal,
	const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	DX_LOG_NET(LogDXNet, Log, TEXT("Begin"));
	
	APlayerController* LoginPlayerController = Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);

	DX_LOG_NET(LogDXNet, Log, TEXT("End"));

	return LoginPlayerController;
}

void ADXGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	DX_LOG_NET(LogDXNet, Log, TEXT("Begin"));
	
	Super::PostLogin(NewPlayer);
	UNetDriver* ServerNetDriver = GetNetDriver();
	if (IsValid(ServerNetDriver) == true)
	{
		if (ServerNetDriver->ClientConnections.Num() == 0)
		{
			DX_LOG_NET(LogDXNet, Log, TEXT("There is no client connection."));
		}
		else
		{
			for (const auto& ClientConnection : ServerNetDriver->ClientConnections)
			{
				if (IsValid(ClientConnection) == true)
				{
					DX_LOG_NET(LogDXNet, Log, TEXT("Client Connection: %s"), *ClientConnection->GetName());
				}
			}
		}
	}
	else
	{
		DX_LOG_NET(LogDXNet, Log, TEXT("ServerNetDriver is invalid."));
	}
	
	DX_LOG_NET(LogDXNet, Log, TEXT("End"));

}

void ADXGameModeBase::StartPlay()
{
	DX_LOG_NET(LogDXNet, Log, TEXT("Begin"));

	Super::StartPlay();
	
	DX_LOG_NET(LogDXNet, Log, TEXT("Begin"));
}
