// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GameAbility/HDGA_Base.h"
#include "HDGA_MovementTrigger.generated.h"

struct FTimerHandle;

UCLASS()
class HELLDIVERS_API UHDGA_MovementTrigger : public UHDGA_Base
{
	GENERATED_BODY()

public:
    UHDGA_MovementTrigger(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void    ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override final;
	virtual void    EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override final;
    
private:
	UFUNCTION()
	void		    OnCurrentStaminaChanged(bool bMatchesComparison, float CurrentValue);

private:
    float           MaxStamina = 0.f;
    FTimerHandle    RegenStaminaHandle = FTimerHandle();
};
