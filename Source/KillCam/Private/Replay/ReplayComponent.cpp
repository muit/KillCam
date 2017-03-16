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

	// ...
    MaxBufferDuration = 6.0f;
    InWorld = nullptr;
}


// Called when the game starts
void UReplayComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
    InWorld = GetWorld();

    //StartRecording();
}

void UReplayComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
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

        /*InWorld->DestroyDemoNetDriver();

        FURL DemoURL;
        UE_LOG(ReplayLog, Log, TEXT("PlayReplay: Attempting to play demo %s"), *NAME_Replay.ToString());

        DemoURL.Map = NAME_Replay.ToString();


        const FName NAME_DemoNetDriver(TEXT("DemoNetDriver"));

        if (!GEngine->CreateNamedNetDriver(InWorld, NAME_DemoNetDriver, NAME_DemoNetDriver))
        {
            UE_LOG(ReplayLog, Warning, TEXT("PlayReplay: failed to create demo net driver!"));
            return;
        }

        InWorld->DemoNetDriver = Cast< UDemoNetDriver >(GEngine->FindNamedNetDriver(InWorld, NAME_DemoNetDriver));

        check(InWorld->DemoNetDriver != NULL);

        InWorld->DemoNetDriver->SetWorld(InWorld);

        FString Error;

        if (!InWorld->DemoNetDriver->InitConnect(InWorld, DemoURL, Error))
        {
            UE_LOG(ReplayLog, Warning, TEXT("Demo playback failed: %s"), *Error);
            InWorld->DestroyDemoNetDriver();
            return;
        }
        else
        {
            FCoreUObjectDelegates::PostDemoPlay.Broadcast();
        }*/


        InWorld->GetGameInstance()->PlayReplay(NAME_Replay.ToString());

        //Get Net Driver
        if (UDemoNetDriver* DemoNetDriver = InWorld->DemoNetDriver) {
            //Go to our desired record time.
            //Time can't be lees than 0. We would go to the past!
            const float ReturnTime = FMath::Clamp(DemoNetDriver->ReplayStreamer->GetTotalDemoTime() - SecondsBack, 0.0f, BIG_NUMBER);
            DemoNetDriver->GotoTimeInSeconds(ReturnTime);
        }
        
        //Duration can't be greater than Seconds back. We would go to the future!!
        //Duration = FMath::Clamp(Duration, 0.0f, SecondsBack);
        //GetWorld()->GetTimerManager().SetTimer(ReplayTimerHandle, this, &UReplayComponent::OnReplayFinished, Duration);

        //ReplayStarted.Broadcast();
    }
}

void UReplayComponent::OnReplayFinished() {
    if (InWorld) {
        InWorld->GetTimerManager().ClearTimer(ReplayTimerHandle);
        
        //Stop Replay
        if (UDemoNetDriver* DemoNetDriver = InWorld->DemoNetDriver) {
            //DemoNetDriver->StopDemo();
            //DemoNetDriver->play
        }
        
        ReplayFinished.Broadcast();
    }
}


