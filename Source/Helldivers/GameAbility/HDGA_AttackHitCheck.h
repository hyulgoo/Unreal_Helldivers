// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "HDGA_AttackHitCheck.generated.h"

class UGameplayEffect;
class AHDTA_Trace;

UCLASS()
class HELLDIVERS_API UHDGA_AttackHitCheck : public UGameplayAbility
{
    GENERATED_BODY()

public:
    explicit        UHDGA_AttackHitCheck();

    virtual void    ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override final;

protected:
    UFUNCTION()
    void            OnTraceResultCallback(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

protected:    
	UPROPERTY(EditAnywhere, Category = "GAS")
	TSubclassOf<UGameplayEffect> AttackDamageEffect;

	UPROPERTY(EditAnywhere, Category = "GAS")
	TSubclassOf<AHDTA_Trace> TargetActorClass;
};
