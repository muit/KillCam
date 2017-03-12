// Fill out your copyright notice in the Description page of Project Settings.

#include "KillCam.h"
#include "Replay.h"


// Sets default values
AReplay::AReplay()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    ReplayComponent = CreateDefaultSubobject<UReplayComponent>(TEXT("Replay"));
}

// Called when the game starts or when spawned
void AReplay::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AReplay::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

