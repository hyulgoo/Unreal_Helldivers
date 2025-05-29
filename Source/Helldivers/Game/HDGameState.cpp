// Fill out your copyright notice in the Description page of Project Settings.

#include "Game/HDGameState.h"
#include "Stratagem/BattleShip/HDBattleShip.h"

AHDGameState::AHDGameState()
	: BattleShipClass(nullptr)
	, BattleShip(nullptr)
{
}

void AHDGameState::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (World && BattleShipClass)
	{
		const FVector SpawnLocation = FVector(0.f, 0.f, 80000.f);
		World->SpawnActor<AHDBattleShip>(BattleShipClass, SpawnLocation, FRotator::ZeroRotator);
	}
}
