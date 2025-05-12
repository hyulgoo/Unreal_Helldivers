// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "HDGameInstance.generated.h"

struct FGameplayTag;
struct FHDProjectileData;

UCLASS()
class HELLDIVERS_API UHDGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
    virtual void                                        Init() override final;
    virtual void                                        Shutdown() override final;
};
