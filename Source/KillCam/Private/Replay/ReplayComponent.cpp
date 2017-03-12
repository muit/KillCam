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
    InWorld = nullptr;
}


// Called when the game starts
void UReplayComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
    InWorld = GetWorld();

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
    if (!InWorld || !InWorld->GetGameInstance())
        return;

    InWorld->GetGameInstance()->StartRecordingReplay(GetName(), GetName()/*, { TEXT("ReplayStreamerOverride=InMemoryNetworkReplayStreaming") }*/);
    RecordDuration = MakeShareable(new FTimer());
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
        InWorld->GetGameInstance()->PlayReplay(GetName());

        //Get Net Driver
        UDemoNetDriver* DemoNetDriver = InWorld->DemoNetDriver;
        //Go to our desired record time.
        //Time can't be lees than 0. We would go to the past!
        //const float ReturnTime = FMath::Clamp(RecordDuration->GetCurrentTime() - SecondsBack, 0.0f, BIG_NUMBER);
        //DemoNetDriver->GotoTimeInSeconds(ReturnTime);

        //Reset record Timer
        //RecordDuration = nullptr;
        
        //Duration can't be greater than Seconds back. We would go to the future!!
        Duration = FMath::Clamp(Duration, 0.0f, SecondsBack);
        GetWorld()->GetTimerManager().SetTimer(ReplayTimerHandle, this, &UReplayComponent::OnReplayFinished, Duration);

        ReplayStarted.Broadcast();
    }
}

void UReplayComponent::OnReplayFinished() {
    if (InWorld) {
        InWorld->GetTimerManager().ClearTimer(ReplayTimerHandle);
        
        //Stop Replay
        if (UDemoNetDriver* DemoNetDriver = InWorld->DemoNetDriver) {
            DemoNetDriver->StopDemo();
            //DemoNetDriver->play
        }
        
        ReplayFinished.Broadcast();
    }
}


