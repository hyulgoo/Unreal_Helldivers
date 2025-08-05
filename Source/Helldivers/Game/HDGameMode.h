// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HDGameMode.generated.h"

UCLASS()
class HELLDIVERS_API AHDGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
    AHDGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void	StartPlay() override final;
};
