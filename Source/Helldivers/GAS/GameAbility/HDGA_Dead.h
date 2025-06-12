// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/Tasks/AbilityTask_WaitAttributeChange.h"
#include "GameplayEffectTypes.h"
#include "HDGA_Dead.generated.h"

UCLASS()
class HELLDIVERS_API UHDGA_Dead : public UGameplayAbility
{
	GENERATED_BODY()

public:
	explicit UHDGA_Dead();

	virtual void	ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override final;

	void			OnHealthChanged(FOnAttributeChangeData Data);
};
