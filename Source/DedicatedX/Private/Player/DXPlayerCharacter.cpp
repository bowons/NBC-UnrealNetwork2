// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/Player/DXPlayerCharacter.h"

#include "DedicatedX.h"
#include "EngineUtils.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Component/DXStatusComponent.h"
#include "Component/DXTextWidgetComponent.h"
#include "Components/CapsuleComponent.h"
#include "Core/DXLandMine.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Player/DXPlayerController.h"
#include "UI/UW_HPText.h"


// Sets default values
ADXPlayerCharacter::ADXPlayerCharacter() : bCanAttack(true), MeleeAttackMontagePlayTime(0.f), LastStartMeleeAttackTime(0.f), MeleeAttackTimeDifference(0.f)
{
	PrimaryActorTick.bCanEverTick = true;
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->TargetArmLength = 400.f;
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->SetupAttachment(GetRootComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->bUsePawnControlRotation = false;
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	
	StatusComponent = CreateDefaultSubobject<UDXStatusComponent>(TEXT("StatusComponent"));
	HPTextWidgetComponent = CreateDefaultSubobject<UDXTextWidgetComponent>(TEXT("HPTextWidgetComponent"));
	HPTextWidgetComponent->SetupAttachment(GetRootComponent());
	HPTextWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
	// HPTextWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	// Billboard 방식으로 보이나, 주인공 캐릭터를 가리게됨. 또한 UI와 멀어져도 동일한 크기가 유지되는 문제도 있음.
	HPTextWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	HPTextWidgetComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	
	// DX_LOG_NET(LogDXNet, Log, TEXT(""));
}

void ADXPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	UEnhancedInputComponent* EIC = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::HandleMoveInput);

	EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::HandleLookInput);
	
	EIC->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
	EIC->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
	EIC->BindAction(LandMineAction, ETriggerEvent::Started, this, &ThisClass::HandleLandMineInput);
	EIC->BindAction(MeleeAttackAction, ETriggerEvent::Started, this, &ThisClass::HandleMeleeAtackInput);
}

void ADXPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsLocallyControlled() == true)
	{
		APlayerController* PC = Cast<APlayerController>(GetController());
		checkf(IsValid(PC) == true, TEXT("PlayerController is invalid."));
	
		UEnhancedInputLocalPlayerSubsystem* EILPS = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
		checkf(IsValid(EILPS) == true, TEXT("EnhancedInputLocalPlayerSubsystem is invalid."));
	
		EILPS->AddMappingContext(InputMappingContext, 0);
	}
	
	StatusComponent->OnOutOfCurrentHP.AddUObject(this, &ThisClass::OnDeath);
	
	if (IsValid(MeleeAttackMontage) == true)
	{
		MeleeAttackMontagePlayTime = MeleeAttackMontage->GetPlayLength();
	}
}

void ADXPlayerCharacter::PossessedBy(AController* NewController)
{
	// DX_LOG_NET(LogDXNet, Log, TEXT("Begin"));

	AActor* OwnerActor = GetOwner();
	if (IsValid(OwnerActor) == true)
	{
		// DX_LOG_NET(LogDXNet, Log, TEXT("OwnerActor Name: %s"), *OwnerActor->GetName());
	}
	else
	{
		// DX_LOG_NET(LogDXNet, Log, TEXT("There is no OwnerActor."));
	}
	
	Super::PossessedBy(NewController);
	
	OwnerActor = GetOwner();
	if (IsValid(OwnerActor) == true)
	{
		// DX_LOG_NET(LogDXNet, Log, TEXT("OwnerActor Name: %s"), *OwnerActor->GetName());
	}
	else
	{
		// DX_LOG_NET(LogDXNet, Log, TEXT("There is no OwnerActor."));
	}

	// DX_LOG_NET(LogDXNet, Log, TEXT("End"));
}

void ADXPlayerCharacter::OnRep_Owner()
{
	// DX_LOG_NET(LogDXNet, Log, TEXT("Begin"));

	Super::OnRep_Owner();
	
	AActor* OwnerActor = GetOwner();
	if (IsValid(OwnerActor) == true)
	{
		// DX_LOG_NET(LogDXNet, Log, TEXT("OwnerActor Name: %s"), *OwnerActor->GetName());
	}
	else
	{
		// DX_LOG_NET(LogDXNet, Log, TEXT("There is no OwnerActor."));
	}
	
	// DX_LOG_NET(LogDXNet, Log, TEXT("End"));
}

void ADXPlayerCharacter::PostNetInit()
{
	// DX_LOG_NET(LogDXNet, Log, TEXT("Begin"));
	
	Super::PostNetInit();
	
	// DX_LOG_NET(LogDXNet, Log, TEXT("End"));
}

void ADXPlayerCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, CurrentAimPitch);
	DOREPLIFETIME(ThisClass, bCanAttack);
}

void ADXPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (IsValid(GetController()) == true)
	{
		PreviousAimPitch = CurrentAimPitch;

		FRotator ControlRotation = GetController()->GetControlRotation();
		float NormalizedPitch = FRotator::NormalizeAxis(ControlRotation.Pitch);
		CurrentAimPitch = FMath::Clamp(NormalizedPitch, -90.0f, 90.0f);
	}
	
	if (IsLocallyControlled() == true && PreviousAimPitch != CurrentAimPitch)
	{
		ServerRPCUpdateAimValue(CurrentAimPitch);
	}
	
	if (IsValid(HPTextWidgetComponent) == true && HasAuthority() == false)
	{
		const FVector WidgetComponentLocation = HPTextWidgetComponent->GetComponentLocation();
		const FVector LocalPlayerCameraLocation = UGameplayStatics::GetPlayerCameraManager(this, 0)->GetCameraLocation();
		HPTextWidgetComponent->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(WidgetComponentLocation, LocalPlayerCameraLocation));
	}
}

void ADXPlayerCharacter::HandleMoveInput(const FInputActionValue& InValue)
{
	if (IsValid(Controller) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("Controller is invalid."));
		return;
	}
	
	const FVector2D InMovementVector = InValue.Get<FVector2D>();

	const FRotator ControlRotation = Controller->GetControlRotation();
	const FRotator ControlYawRotation(0.0f, ControlRotation.Yaw, 0.0f);

	const FVector ForwardDirection = FRotationMatrix(ControlYawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(ControlYawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, InMovementVector.X);
	AddMovementInput(RightDirection, InMovementVector.Y);
}

void ADXPlayerCharacter::HandleLookInput(const FInputActionValue& InValue)
{
	if (IsValid(Controller) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("Controller is invalid."));
		return;
	}
	
	const FVector2D InLookVector = InValue.Get<FVector2D>();

	AddControllerYawInput(InLookVector.X);
	AddControllerPitchInput(InLookVector.Y);
}

void ADXPlayerCharacter::HandleLandMineInput(const FInputActionValue& InValue)
{
	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("HandleLandMineInput()")), true, true, FLinearColor::Green, 5.f);
	
	// if (IsValid(LandMineClass) == true)
	// {
	// 	FVector SpawnedLocation = (GetActorLocation() + GetActorForwardVector() * 300.f) - FVector(0.f, 0.f, 90.f);
	// 	ADXLandMine* SpawnedLandMine = GetWorld()->SpawnActor<ADXLandMine>(LandMineClass, SpawnedLocation, FRotator::ZeroRotator);
	// 	SpawnedLandMine->SetOwner(GetController());
	// }
	
	if (IsLocallyControlled() == true)
	{
		ServerRPCSpawnLandMine();
	}
}

void ADXPlayerCharacter::HandleMeleeAtackInput(const FInputActionValue& InValue)
{
	if (true == bCanAttack && GetCharacterMovement()->IsFalling() == false)
	{
		ServerRPCMeleeAttack(GetWorld()->GetGameState()->GetServerWorldTimeSeconds());
		
		if (HasAuthority() == false && IsLocallyControlled() == true)
		{
			PlayMeleeAttackMontage();
		}
	}
}

float ADXPlayerCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("TakeDamage: %f"), DamageAmount), true, true, FLinearColor::Red, 5.f);
	
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	StatusComponent->ApplyDamage(ActualDamage);
	return ActualDamage;
}

void ADXPlayerCharacter::CheckMeleeAttackHit()
{
	//if (HasAuthority() == true) - 역할 구분
	if (IsLocallyControlled() == true)
	{
		TArray<FHitResult> OutHitResults;
		TSet<ACharacter*> DamagedCharacters;
		FCollisionQueryParams Params(NAME_None, false, this);
	
		const float MeleeAttackRange = 50.f;
		const float MeleeAttackRadius = 50.f;
		//const float MeleeAttackDamage = 10.f; - 데미지 처리는 서버로 이전
	
		const FVector Forward = GetActorForwardVector();
		const FVector Start = GetActorLocation() + Forward * GetCapsuleComponent()->GetScaledCapsuleRadius();
		const FVector End = Start + GetActorForwardVector() * MeleeAttackRange;
	
		bool bIsHitDetected = GetWorld()->SweepMultiByChannel(OutHitResults, Start, End, FQuat::Identity, ECC_Camera, FCollisionShape::MakeSphere(MeleeAttackRadius), Params);
	
		if (bIsHitDetected)
		{
			for (auto const& OutHitResult : OutHitResults)
			{
				ACharacter* DamagedCharacter = Cast<ACharacter>(OutHitResult.GetActor());
				if (IsValid(DamagedCharacter) == true)
				{
					DamagedCharacters.Add(DamagedCharacter);
				}
			}

			for (auto const& DamagedCharacter : DamagedCharacters)
			{
				// FDamageEvent DamageEvent;
				// DamagedCharacter->TakeDamage(MeleeAttackDamage, DamageEvent, GetController(), this);
				ServerRPCPerformMeleeHit(DamagedCharacter, GetWorld()->GetGameState()->GetServerWorldTimeSeconds());
			}
		}
	
		FColor DrawColor = bIsHitDetected ? FColor::Green : FColor::Red;
		DrawDebugMeleeAttack(DrawColor, Start, End, Forward);
	}
}

void ADXPlayerCharacter::DrawDebugMeleeAttack(const FColor& DrawColor, FVector TraceStart, FVector TraceEnd,
	FVector Forward)
{
	const float MeleeAttackRange = 50.f;
	const float MeleeAttackRadius = 50.f;
	FVector CapsuleOrigin = TraceStart + (TraceEnd - TraceStart) * 0.5f;
	float CapsuleHalfHeight = MeleeAttackRange * 0.5f;
	DrawDebugCapsule(GetWorld(), CapsuleOrigin, CapsuleHalfHeight, MeleeAttackRadius, FRotationMatrix::MakeFromZ(Forward).ToQuat(), DrawColor, false, 5.0f);
}

void ADXPlayerCharacter::ServerRPCMeleeAttack_Implementation(float InStartMeleeAttackTime)
{
	// 우리가 하고자 하는 것은, 너무 빠르게 공격 입력이 들어오는 것을 막아보고자 함.
	// 애님 몽타주가 끝나기 전에는 다시 공격 하지 못하게 하고 싶음.
	// 근데 공격 입력을 했다고해서 그 즉시 서버에서도 공격 입력을 확인 할 순 없음.
	// 유저 입장에선 억울하니까 서버로 "공격이 입력되었다"는 신호가 가는데까지 걸리는 서버딜레이는 빼줘야함.
	// 즉, 공격재개가능시간 = 몽타주재생시간 - 서버딜레이
	MeleeAttackTimeDifference = GetWorld()->GetTimeSeconds() - InStartMeleeAttackTime;
	// 서버딜레이 = 현재서버시간 - 공격 입력이 들어왔을때 서버시간
	MeleeAttackTimeDifference = FMath::Clamp(MeleeAttackTimeDifference, 0.f, MeleeAttackMontagePlayTime);;
	// 서버딜레이는 0초에서 밀리 공격 몽타주 재생시간 사이로 제한. 0초보다 작다는건 말이 안되는 자명한 사실이고,
	// 밀리 공격 몽타주 재생시간 보다 크다는건, 그럴 수는 있으나 공식(공격재개가능시간 = 몽타주재생시간 - 서버딜레이)에 의해 우리가 할 수 있는 최선이 곧바로 실행하는 것. 
	// 미리 실행할 순 없음.
	
	if (KINDA_SMALL_NUMBER < MeleeAttackMontagePlayTime - MeleeAttackTimeDifference)
	{
		// 대충 0초로 타이머를 걸면, ClearTimer() 함수를 호출하는 것과 똑같다는 뜻.
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([&]()
		{
			bCanAttack = true;
			OnRep_CanAttack();
		}), MeleeAttackMontagePlayTime - MeleeAttackTimeDifference, false, -1.f);
	}
	// else
	// {
	//    이 경우는 서버딜레이가 너무 심해버려서, 서버딜레이 == 밀리 공격 애님 몽타주 재생시간인 경우.
	//    우리가 할 수 있는건 bCanAttack을 그대로 둬서, 마치 아무일도 일어나지 않게끔함. 바로 공격할 수 있게함. - 불쾌감 최소화
	// }

	LastStartMeleeAttackTime = InStartMeleeAttackTime;
	PlayMeleeAttackMontage();

	//MulticastRPCMeleeAttack();
	
	// NetMulticast RPC의 경우에는 쓸데 없이 Server, OwningClient, OtherClient 모두에게 RPC 호출. 우리가 필요한 건 OtherClient만.
	for (APlayerController* PlayerController : TActorRange<APlayerController>(GetWorld()))
	{// 이 캐릭터는 공격한 플레이어의 캐릭터임. 공격한 플레이어의 컨트롤러 외의 컨트롤러들을 찾기 위한 조건문.
		if (IsValid(PlayerController) == true && GetController() != PlayerController)
		{
			ADXPlayerCharacter* OtherPlayerCharacter = Cast<ADXPlayerCharacter>(PlayerController->GetPawn());
			if (OtherPlayerCharacter)
			{
				OtherPlayerCharacter->ClientRPCPlayMeleeAttackMontage(this);
				// 다른 플레이어 컨트롤러의 캐릭터에 공격한 클라이언트의 캐릭터를 넘겨줘서, 애니메이션이 재생되게끔 ..
			}
		}
	}
}

bool ADXPlayerCharacter::ServerRPCMeleeAttack_Validate(float InStartMeleeAttackTime)
{
	if (LastStartMeleeAttackTime == 0.0f)
	{
		// 최초 공격은 일단 통과.
		return true;
	}
	
	return (MeleeAttackMontagePlayTime - 0.1f) < (InStartMeleeAttackTime - LastStartMeleeAttackTime);
	// InStartMeleeAttackTime - LastStartMeleeAttackTime은 "이전 공격 기준으로 얼마나 더 빠르게 공격시도 했는가"를 의미함.
	// 이 시간이 너무나도 짧아서 밀리 공격 애님 몽타주 재생시간 보다도 짧아버리면 문제가 됨. 그러니 ServerRPC 블락처리.
}

void ADXPlayerCharacter::MulticastRPCMeleeAttack_Implementation()
{
	// if (HasAuthority() == true)
	// {
	// 	bCanAttack = false;
	// 	
	// 	OnRep_CanAttack();
	// 	
	// 	FTimerHandle Handle;
	// 	GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&]()
	// 	{
	// 		bCanAttack = true;
	// 		OnRep_CanAttack();
	// 	}), MeleeAttackMontagePlayTime, false);
	// }
	
	if (HasAuthority() == false && IsLocallyControlled() == false)
	{
		PlayMeleeAttackMontage();
	}
}

void ADXPlayerCharacter::ServerRPCPerformMeleeHit_Implementation(ACharacter* InDamagedCharacters, float InCheckTime)
{
	if (IsValid(InDamagedCharacters) == true)
	{
		const float MeleeAttackDamage = 10.f;
		FDamageEvent DamageEvent;
		InDamagedCharacters->TakeDamage(MeleeAttackDamage, DamageEvent, GetController(), this);
	}
}

bool ADXPlayerCharacter::ServerRPCPerformMeleeHit_Validate(ACharacter* InDamagedCharacters, float InCheckTime)
{
	return MinAllowedTimeForMeleeAttack < (InCheckTime - LastStartMeleeAttackTime);
}

void ADXPlayerCharacter::ClientRPCPlayMeleeAttackMontage_Implementation(ADXPlayerCharacter* InTargetCharacter)
{
	if (IsValid(InTargetCharacter) == true)
	{
		InTargetCharacter->PlayMeleeAttackMontage();
	}
}

void ADXPlayerCharacter::OnRep_CanAttack()
{
	if (bCanAttack == true)
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
	else
	{
		GetCharacterMovement()->SetMovementMode(MOVE_None);
	}
}

void ADXPlayerCharacter::PlayMeleeAttackMontage() const
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (IsValid(AnimInstance) == true)
	{
		AnimInstance->StopAllMontages(0.f);
		AnimInstance->Montage_Play(MeleeAttackMontage);		
	}
}

void ADXPlayerCharacter::OnDeath()
{
	ADXPlayerController* PlayerController = GetController<ADXPlayerController>();
	if (IsValid(PlayerController) == true && HasAuthority() == true)
	{
		PlayerController->OnCharacterDead();
	}
}

void ADXPlayerCharacter::SetHPTextWidget(UUW_HPText* InHPTextWidget)
{
	UUW_HPText* HPWidget = Cast<UUW_HPText>(InHPTextWidget);
	if (IsValid(HPWidget) == true)
	{
		HPWidget->InitializeHPTextWidget(StatusComponent);
		StatusComponent->OnCurrentHPChanged.AddUObject(HPWidget, &UUW_HPText::OnCurrentHPChange);
		StatusComponent->OnMaxHPChanged.AddUObject(HPWidget, &UUW_HPText::OnMaxHPChange);
	}
}

void ADXPlayerCharacter::TakeBuff(float InBuffValue)
{
	if (IsValid(StatusComponent) == true)
	{
		StatusComponent->SetMaxHP(StatusComponent->GetMaxHP() + InBuffValue);
		StatusComponent->SetCurrentHP(StatusComponent->GetCurrentHP() + InBuffValue);
	}
}

void ADXPlayerCharacter::ServerRPCUpdateAimValue_Implementation(const float& InAimPitchValue)
{
	CurrentAimPitch = InAimPitchValue;
}

void ADXPlayerCharacter::ServerRPCSpawnLandMine_Implementation()
{
	if (IsValid(LandMineClass) == true)
	{
		FVector SpawnedLocation = (GetActorLocation() + GetActorForwardVector() * 300.f) - FVector(0.f, 0.f, 90.f);
		ADXLandMine* SpawnedLandMine = GetWorld()->SpawnActor<ADXLandMine>(LandMineClass, SpawnedLocation, FRotator::ZeroRotator);
		// SpawnedLandMine->SetOwner(GetController());
		// 내 컨트롤러는 서버와 내 PC에만 존재함. 
		// 다른 PC에는 내 컨트롤러가 존재하지 않음에 주의. 현재 로직으로는 Other와 Owning을 구분 불가.
		SpawnedLandMine->SetOwner(this);
		// 그러나 내 캐릭터는 서버와 내 PC 뿐만 아니라 다른 PC에도 존재함.
	}	
}

bool ADXPlayerCharacter::ServerRPCSpawnLandMine_Validate()
{
	return true;
}
