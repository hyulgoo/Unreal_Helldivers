// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GameAbility/HDGA_Base.h"
#include "HDGA_WeaponTrigger.generated.h"

class IHDWeaponInterface;

UCLASS()
class HELLDIVERS_API UHDGA_WeaponTrigger : public UHDGA_Base
{
	GENERATED_BODY()

public:
	explicit UHDGA_WeaponTrigger();

	virtual void							ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override final;
	virtual void							EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override final;

private:
	UFUNCTION()
	void									OnDelayCompleted();

	void									SetAbilityTimer();

protected:
	TScriptInterface<IHDWeaponInterface>	WeaponInterface;
	float									SavedDelay;
};
