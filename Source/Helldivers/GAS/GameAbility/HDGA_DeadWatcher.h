// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "HDGA_DeadWatcher.generated.h"

UCLASS()
class HELLDIVERS_API UHDGA_DeadWatcher : public UGameplayAbility
{
	GENERATED_BODY()

public:
	explicit UHDGA_DeadWatcher();

	virtual void	ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override final;

	void			OnHealthChanged(const FOnAttributeChangeData& Data);
};
