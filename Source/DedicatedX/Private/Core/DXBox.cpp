// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/DXBox.h"

#include "DedicatedX.h"
#include "Components/PointLightComponent.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ADXBox::ADXBox() : ServerRotationYaw(0.0f), RotationSpeed(30.0f), NetCullDistance(1000.f)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(SceneRoot);
	Mesh->SetRelativeLocation(FVector(-50.f, -50.f, 50.f));

	static constexpr float BoxActorNetUpdateFrequency = 1.f;
	SetNetUpdateFrequency(BoxActorNetUpdateFrequency);
	// 1초에 1번씩 액터 레플리케이션 시도. 즉, 서버 성능이 아무리 좋아도 1초에 1번씩만 레플리케이션함.
	NetUpdatePeriod = 1 / GetNetUpdateFrequency();
	// 주기 = 1 / 주파수
	
	SetNetCullDistanceSquared(NetCullDistance * NetCullDistance); // 제곱 값을 입력으로 받음
	
	PointLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLight"));
	PointLight->SetupAttachment(SceneRoot);
	
	//SetNetDormancy(DORM_Initial);
}

// Called when the game starts or when spawned
void ADXBox::BeginPlay()
{
	Super::BeginPlay();
	
	DX_LOG_ROLE(LogDXNet, Log, TEXT(""));
	
	if (HasAuthority() == true)
	{
		FTimerHandle TimerHandle01;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle01, FTimerDelegate::CreateLambda([&]()-> void
		{
			float RandomR = FMath::RandRange(0.f, 1.f);
			float RandomG = FMath::RandRange(0.f, 1.f);
			float RandomB = FMath::RandRange(0.f, 1.f);
			ServerLightColor = FLinearColor(RandomR, RandomG, RandomB, 1.f);
			OnRep_ServerLightColor();
				// 서버에서도 로직이 수행될 수 있게끔 OnRep_() 함수를 명시적으로 호출.
		}), 1.f, true);
		
		// FTimerHandle TimerHandle02;
		// GetWorld()->GetTimerManager().SetTimer(TimerHandle02, FTimerDelegate::CreateLambda(
		// 	[&]()->void
		// 	{
		// 		FlushNetDormancy();
		// 	}), 5.f, false
		// );
	}
}

void ADXBox::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, ServerRotationYaw);
	//DOREPLIFETIME(ThisClass, ServerLightColor);
	DOREPLIFETIME_CONDITION(ThisClass, ServerLightColor, COND_InitialOnly);
}

void ADXBox::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (HasAuthority() == true)
	{
		AddActorLocalRotation(FRotator(0.f, RotationSpeed * DeltaSeconds, 0.f));
		ServerRotationYaw = RootComponent->GetComponentRotation().Yaw;
	}
	else
	{
	//	SetActorRotation(FRotator(0.f, ServerRotationYaw, 0.f));
		if (NetUpdatePeriod < KINDA_SMALL_NUMBER)
		{
			return;			
		}
		
		AccDeltaSecondSinceReplicated += DeltaSeconds;
		const float LerpRatio = FMath::Clamp(AccDeltaSecondSinceReplicated / NetUpdatePeriod, 0.f, 1.f);
		const float NextServerRotationYaw = ServerRotationYaw + RotationSpeed * NetUpdatePeriod;
		const float EstimatedClientRotationYaw = FMath::Lerp(ServerRotationYaw, NextServerRotationYaw, LerpRatio);
		SetActorRotation(FRotator(0.f, EstimatedClientRotationYaw, 0.f));

	}
	
	DrawDebugSphere(GetWorld(), GetActorLocation(), NetCullDistance / 2.f, 16, FColor::Green, false, -1.f);
	// NetCullDistanceSquared를 시각화 하기 위한 디버그 드로잉
}

bool ADXBox::IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const
{
	bool bIsNetRelevant = Super::IsNetRelevantFor(RealViewer, ViewTarget, SrcLocation);
	
	if (false == bIsNetRelevant)
	{
		DX_LOG_NET(LogDXNet, Log, TEXT("%s is not relevant for(%s, %s)"), *GetName(), *RealViewer->GetName(), *ViewTarget->GetName());
	}
	
	return bIsNetRelevant;
}

void ADXBox::OnRep_ServerRotationYaw()
{
	//DX_LOG_NET(LogDXNet, Log, TEXT("OnRep_ServerRotationYaw(): %f"), ServerRotationYaw);
	//SetActorRotation(FRotator(0.f, ServerRotationYaw, 0.f));
	
	// 지금은 큰 차이 없긴함. Tick() 함수에서 매 틱 마다 ServerRotationYaw 값이 수정되기 때문.
	// 하지만 추후에 매 틱마다 수정하지 않거나, 레플리케이션 주기를 낮춘다면 상당한 효과를 거둘 수 있음.
	
	AccDeltaSecondSinceReplicated = 0.f;
}

void ADXBox::OnRep_ServerLightColor()
{
	if (HasAuthority() == false)
	{
		DX_LOG_NET(LogDXNet, Log, TEXT("OnRep_ServerLightColor(): %s"), *ServerLightColor.ToString());
	}

	PointLight->SetLightColor(ServerLightColor);
}
