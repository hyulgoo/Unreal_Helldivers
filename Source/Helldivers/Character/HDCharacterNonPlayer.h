// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/HDCharacterBase.h"
#include "Engine/StreamableManager.h"
#include "HDCharacterNonPlayer.generated.h"


UCLASS()
class HELLDIVERS_API AHDCharacterNonPlayer : public AHDCharacterBase
{
	GENERATED_BODY()
	
public:
    explicit AHDCharacterNonPlayer();

protected:
    virtual void PostInitializeComponents() override final;
    virtual void SetDead() override final;
};
