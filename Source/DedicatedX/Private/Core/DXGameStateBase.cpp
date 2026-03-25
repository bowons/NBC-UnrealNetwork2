// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/Core/DXGameStateBase.h"

#include "DedicatedX.h"
#include "Net/UnrealNetwork.h"

ADXGameStateBase::ADXGameStateBase()
{
	// DX_LOG_NET(LogDXNet, Log, TEXT(""));
}

void ADXGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, AlivePlayerControllerCount);
	DOREPLIFETIME(ThisClass, MatchState);
}
