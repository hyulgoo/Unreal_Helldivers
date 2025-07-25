// Fill out your copyright notice in the Description page of Project Settings.

#include "HDGameMode.h"

AHDGameMode::AHDGameMode()
{
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
