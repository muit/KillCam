// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "ReplayComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(ReplayLog, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReplayStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReplayFinished);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KILLCAM_API UReplayComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UReplayComponent();

    UPROPERTY(BlueprintAssignable, Category = Replay)
    FOnReplayStarted ReplayStarted;

    UPROPERTY(BlueprintAssignable, Category = Replay)
    FOnReplayFinished ReplayFinished;

private:

    TSharedPtr<FTimer> RecordDuration;

    UPROPERTY()
    FTimerHandle ReplayTimerHandle;

    UPROPERTY()
    UWorld* InWorld;


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

    virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

public:
    UFUNCTION(BlueprintCallable, Category = Replay)
    void StartRecording();

    UFUNCTION(BlueprintCallable, Category = Replay)
    void StopRecording();


    UFUNCTION(BlueprintCallable, Category = Replay)
    void Replay(float SecondsBack = 3.0f, float Duration = 3.0f);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = Replay)
    bool IsInReplay() const {
        if (!InWorld)
            return false;

        return InWorld->GetTimerManager().IsTimerActive(ReplayTimerHandle);
    }

    UFUNCTION()
    void OnReplayFinished();


    void CreateDuplicateLevel();
};
