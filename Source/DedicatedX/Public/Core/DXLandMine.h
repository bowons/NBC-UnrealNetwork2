// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DXLandMine.generated.h"

class UBoxComponent;

UCLASS()
class DEDICATEDX_API ADXLandMine : public AActor
{
	GENERATED_BODY()

public:
	ADXLandMine();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TObjectPtr<USceneComponent> SceneRoot;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TObjectPtr<UBoxComponent> BoxCollision;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TObjectPtr<UParticleSystemComponent> Particle;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess))
	TObjectPtr<UMaterial> ExplodedMaterial;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess))
	float NetCullDistance;
	
	UPROPERTY(ReplicatedUsing = OnRep_bIsExploded)
	uint8 bIsExploded : 1;
	
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCSpawnEffect();

	UFUNCTION()
	void OnLandMineBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);
	
	UFUNCTION()
	void OnRep_bIsExploded();
};
