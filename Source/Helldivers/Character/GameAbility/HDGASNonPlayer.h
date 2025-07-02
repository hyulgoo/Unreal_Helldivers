// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/HDCharacterNonPlayer.h"
#include "AbilitySystemInterface.h"
#include "HDGASNonPlayer.generated.h"

class UGameplayEffect;
class UGameplayAbility;

UCLASS()
class HELLDIVERS_API AHDGASNonPlayer : public AHDCharacterNonPlayer
{
	GENERATED_BODY()
	
public:
	explicit								AHDGASNonPlayer();

protected:
	virtual void							BeginPlay() override final;

private:
	void									InitAbilitySystemComponent();
	void									InitializeAttributeSet();

protected:
};
