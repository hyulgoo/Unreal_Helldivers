// Fill out your copyright notice in the Description page of Project Settings.

#include "HDGA_Fire.h"
#include "Define/HDDefine.h"
#include "Interface/HDWeaponInterface.h"
#include "GameFramework/CharacterMovementComponent.h"

UHDGA_Fire::UHDGA_Fire()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UHDGA_Fire::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UHDGA_Fire::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	TScriptInterface<IHDWeaponInterface> WeaponInterface = ActorInfo->AvatarActor.Get();
	NULL_CHECK(WeaponInterface);

	WeaponInterface->Fire(true);
}

void UHDGA_Fire::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	TScriptInterface<IHDWeaponInterface> WeaponInterface = ActorInfo->AvatarActor.Get();
	NULL_CHECK(WeaponInterface);

	WeaponInterface->Fire(false);

	const bool bReplicatedEndAbility = true;
	const bool bWasCancelled = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}
