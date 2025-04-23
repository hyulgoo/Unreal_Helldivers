// Fill out your copyright notice in the Description page of Project Settings.

#include "Game/HDGameMode.h"

AHDGameMode::AHDGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> DefaultPawnClassRef(TEXT("/Game/Helldivers/Blueprint/Character/BP_HDGASCharacterPlayer.BP_HDGASCharacterPlayer_C"));
	if (DefaultPawnClassRef.Class)
	{
		DefaultPawnClass = DefaultPawnClassRef.Class;
	}

	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerClassRef(TEXT("/Script/Helldivers.HDPlayerController"));
	if (PlayerControllerClassRef.Class)
	{
		PlayerControllerClass = PlayerControllerClassRef.Class;
	}
}

void AHDGameMode::StartPlay()
{
	Super::StartPlay();
}
