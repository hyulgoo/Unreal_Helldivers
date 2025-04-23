// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AbilitySystemInterface.h"
#include "HDGASUserWidget.generated.h"

class UAbilitySystemComponent;

UCLASS()
class HELLDIVERS_API UHDGASUserWidget : public UUserWidget, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
    virtual void SetAbilitySystemComponent(UAbilitySystemComponent* NewAbilitySystemComponent);
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override final;

protected:
    UPROPERTY(EditAnywhere, Category = GAS)
    TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
};
