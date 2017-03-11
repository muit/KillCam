// Fill out your copyright notice in the Description page of Project Settings.

#include "KillCam.h"
#include "Engine/DemoNetDriver.h"
#include "ReplayComponent.h"


// Sets default values for this component's properties
UReplayComponent::UReplayComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
    GameInstance = nullptr;
}


// Called when the game starts
void UReplayComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
    if (GEngine->GetWorld() && GEngine->GetWorld()->GetGameInstance())
    {
        GameInstance = GEngine->GetWorld()->GetGameInstance();
    }
    StartRecording();
}

void UReplayComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
    if (RecordDuration.IsValid()) {
        RecordDuration->Tick(DeltaTime);
    }
}

void UReplayComponent::StartRecording()
{
    if (GameInstance) {
        GameInstance->StartRecordingReplay(GetName(), TEXT("Kill Cam"), { TEXT("ReplayStreamerOverride=InMemoryNetworkReplayStreaming") });
        RecordDuration = MakeShareable(new FTimer());
    }
}

void UReplayComponent::StopRecording()
{
    if (GameInstance) {
        GameInstance->StopRecordingReplay();
    }
}

void UReplayComponent::Replay(float SecondsBack, float Duration)
{
    if (GameInstance && !IsInReplay()) {
        StopRecording();
        //Start replay
        GameInstance->PlayReplay(GetName());

        //Get Net Driver
        UDemoNetDriver* DemoNetDriver = GEngine->GetWorld()->DemoNetDriver;

        //Go to our desired record time.
        //Time can't be lees than 0. We would go to the past!
        const float ReturnTime = FMath::Clamp(RecordDuration->GetCurrentTime() - SecondsBack, 0.0f, BIG_NUMBER);
        DemoNetDriver->GotoTimeInSeconds(ReturnTime);

        //Reset record Timer
        RecordDuration = nullptr;
        
        //Duration can't be greater than Seconds back. We would go to the future!!
        Duration = FMath::Clamp(Duration, 0.0f, SecondsBack);
        GetWorld()->GetTimerManager().SetTimer(ReplayTimerHandle, this, &UReplayComponent::OnReplayFinished, Duration, false);

        ReplayStarted.Broadcast();
    }
}

void UReplayComponent::OnReplayFinished() {
    if (GameInstance) {
        GetWorld()->GetTimerManager().ClearTimer(ReplayTimerHandle);
        //GameInstance->StopReplay();
        ReplayFinished.Broadcast();
    }
}


