// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DXBox.generated.h"

class UPointLightComponent;

UCLASS()
class DEDICATEDX_API ADXBox : public AActor
{
	GENERATED_BODY()
	
public:	
	ADXBox();

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Tick(float DeltaSeconds) override;
	
	virtual bool IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const override;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USceneComponent> SceneRoot;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> Mesh;
	
	UPROPERTY(ReplicatedUsing = OnRep_ServerRotationYaw)
	float ServerRotationYaw;
	float RotationSpeed;
	
	float NetUpdatePeriod;
	float AccDeltaSecondSinceReplicated;
	
	float NetCullDistance;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UPointLightComponent> PointLight;

	UPROPERTY(ReplicatedUsing = OnRep_ServerLightColor)
	FLinearColor ServerLightColor;
	
private:
	UFUNCTION()
	void OnRep_ServerRotationYaw();

	UFUNCTION()
	void OnRep_ServerLightColor();
};
