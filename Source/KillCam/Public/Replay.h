// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "ReplayComponent.h"
#include "Replay.generated.h"

UCLASS(Blueprintable)
class KILLCAM_API AReplay : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AReplay();

    UPROPERTY(Category = Replay, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    UReplayComponent* ReplayComponent;

    UPROPERTY(EditAnywhere, Category = Replay, meta=(DisplayName="Dynamic Level"))
    TAssetPtr<UWorld> DynamicLevelAsset;

    UPROPERTY(Transient)
    UWorld* DynamicLevel;

    UPROPERTY(Transient)
    UWorld* DuplicatedDynamicLevel;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
