// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "HDGASPlayerState.generated.h"

class UAbilitySystemComponent;

UCLASS()
class HELLDIVERS_API AHDGASPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
    explicit                            AHDGASPlayerState();

    virtual UAbilitySystemComponent*    GetAbilitySystemComponent() const override final;

protected:
    UPROPERTY(EditAnywhere, Category = GAS)
    TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
};
