// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/Player/DXPlayerCharacter.h"

#include "DedicatedX.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"


// Sets default values
ADXPlayerCharacter::ADXPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	
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
	
	DX_LOG_NET(LogDXNet, Log, TEXT(""));
}

void ADXPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	UEnhancedInputComponent* EIC = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::HandleMoveInput);

	EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::HandleLookInput);
	
	EIC->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
	EIC->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
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
}

void ADXPlayerCharacter::PossessedBy(AController* NewController)
{
	DX_LOG_NET(LogDXNet, Log, TEXT("Begin"));

	AActor* OwnerActor = GetOwner();
	if (IsValid(OwnerActor) == true)
	{
		DX_LOG_NET(LogDXNet, Log, TEXT("OwnerActor Name: %s"), *OwnerActor->GetName());
	}
	else
	{
		DX_LOG_NET(LogDXNet, Log, TEXT("There is no OwnerActor."));
	}
	
	Super::PossessedBy(NewController);
	
	OwnerActor = GetOwner();
	if (IsValid(OwnerActor) == true)
	{
		DX_LOG_NET(LogDXNet, Log, TEXT("OwnerActor Name: %s"), *OwnerActor->GetName());
	}
	else
	{
		DX_LOG_NET(LogDXNet, Log, TEXT("There is no OwnerActor."));
	}

	DX_LOG_NET(LogDXNet, Log, TEXT("End"));
}

void ADXPlayerCharacter::OnRep_Owner()
{
	DX_LOG_NET(LogDXNet, Log, TEXT("Begin"));

	Super::OnRep_Owner();
	
	AActor* OwnerActor = GetOwner();
	if (IsValid(OwnerActor) == true)
	{
		DX_LOG_NET(LogDXNet, Log, TEXT("OwnerActor Name: %s"), *OwnerActor->GetName());
	}
	else
	{
		DX_LOG_NET(LogDXNet, Log, TEXT("There is no OwnerActor."));
	}
	
	DX_LOG_NET(LogDXNet, Log, TEXT("End"));
}

void ADXPlayerCharacter::PostNetInit()
{
	DX_LOG_NET(LogDXNet, Log, TEXT("Begin"));
	
	Super::PostNetInit();
	
	DX_LOG_NET(LogDXNet, Log, TEXT("End"));
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
