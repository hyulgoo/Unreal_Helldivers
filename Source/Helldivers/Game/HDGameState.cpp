// Fill out your copyright notice in the Description page of Project Settings.

#include "Game/HDGameState.h"
#include "Define/HDDefine.h"
#include "Stratagem/BattleShip/HDBattleShip.h"

AHDGameState::AHDGameState()
	: BattleShipClass(nullptr)
	, BattleShip(nullptr)
    , SpawnHeight(80000.f)
{
}

AHDBattleShip* AHDGameState::GetBattleShip() const
{
    return BattleShip;
}

void AHDGameState::SetBattleShip(AHDBattleShip* NewBattleShip)
{
    BattleShip = NewBattleShip;
}

void AHDGameState::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	VALID_CHECK(World);
	NULL_CHECK(BattleShipClass);

	const FVector SpawnLocation(0.f, 0.f, SpawnHeight);
	BattleShip = World->SpawnActor<AHDBattleShip>(BattleShipClass, SpawnLocation, FRotator::ZeroRotator);
	NULL_CHECK(BattleShip);
}
