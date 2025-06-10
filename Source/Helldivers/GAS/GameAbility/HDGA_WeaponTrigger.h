// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HDGameplayAbility.h"
#include "HDGA_WeaponTrigger.generated.h"

class IHDWeaponInterface;

UCLASS()
class HELLDIVERS_API UHDGA_WeaponTrigger : public UHDGameplayAbility
{
	GENERATED_BODY()

public:
	explicit UHDGA_WeaponTrigger();

	virtual bool							CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override final;
	virtual void							ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override final;
	virtual void							InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override final;
	virtual void							EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override final;

protected:
	UFUNCTION()
	void									OnDelayCompleted();

private:
	void									SetAbilityTimer(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const float Delay);

protected:
	TScriptInterface<IHDWeaponInterface>	WeaponInterface;
	FGameplayAbilitySpecHandle				SavedHandle;
	FGameplayAbilityActorInfo const*		SavedActorInfo;
	FGameplayAbilityActivationInfo			SavedActivationInfo;
	float									SavedDelay;
};
