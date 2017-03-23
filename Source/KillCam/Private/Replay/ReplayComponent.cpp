// Fill out your copyright notice in the Description page of Project Settings.

#include "KillCam.h"
#include "Engine/DemoNetDriver.h"
#include "ReplayComponent.h"

DEFINE_LOG_CATEGORY(ReplayLog);

// Sets default values for this component's properties
UReplayComponent::UReplayComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = true;

    // Default Values
    MaxBufferDuration = 6.0f;
    InWorld = nullptr;
}


// Called when the game starts
void UReplayComponent::BeginPlay()
{
    Super::BeginPlay();

    //Cache world
    InWorld = GetWorld();

    // Do we want to record from the start? 
    // We may want external control over it.
    StartRecording();
}

void UReplayComponent::StartRecording()
{
    if (!InWorld || !InWorld->GetGameInstance())
        return;

    InWorld->GetGameInstance()->StartRecordingReplay("KillCam", "KillCam", { TEXT("ReplayStreamerOverride=InMemoryNetworkReplayStreaming") });

    //Get Net Driver
    if (UDemoNetDriver* DemoNetDriver = InWorld->DemoNetDriver) {
        //Limit recording duration to MaxBufferDuration seconds
        DemoNetDriver->ReplayStreamer->SetTimeBufferHintSeconds(MaxBufferDuration);
    }
}

void UReplayComponent::StopRecording()
{
    if (InWorld && InWorld->GetGameInstance()) {
        InWorld->GetGameInstance()->StopRecordingReplay();
    }
}

void UReplayComponent::Replay(float SecondsBack, float Duration)
{
    if (InWorld && InWorld->GetGameInstance() && !IsInReplay()) {
        //Start replay
        StopRecording();

        const FName NAME_Replay(TEXT("KillCam"));

        InWorld->GetGameInstance()->PlayReplay(NAME_Replay.ToString());

        //Get Net Driver
        if (UDemoNetDriver* DemoNetDriver = InWorld->DemoNetDriver) {
            //Go to our desired record time.
            //Time can't be lees than 0. We would go to the past!
            const float ReturnTime = FMath::Clamp(DemoNetDriver->ReplayStreamer->GetTotalDemoTime() - SecondsBack, 0.0f, BIG_NUMBER);
            DemoNetDriver->GotoTimeInSeconds(ReturnTime);
        }
        
        //Duration can't be greater than Seconds back. We would go to the future!!
        Duration = FMath::Clamp(Duration, 0.0f, SecondsBack);
        //Set timer for replay duration
        GetWorld()->GetTimerManager().SetTimer(ReplayTimerHandle, this, &UReplayComponent::OnReplayFinished, Duration);

        ReplayStarted.Broadcast();
    }
}

void UReplayComponent::OnReplayFinished() {
    if (InWorld) {
        //Reset replay timer
        InWorld->GetTimerManager().ClearTimer(ReplayTimerHandle);
        
        //Stop Replay and go back to the main game
        if (UDemoNetDriver* DemoNetDriver = InWorld->DemoNetDriver) {
            DemoNetDriver->StopDemo();
        }
        
        ReplayFinished.Broadcast();
    }
}


