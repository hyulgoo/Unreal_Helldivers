// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/GameAbility/HDGA_WeaponTrigger.h"
#include "Define/HDDefine.h"
#include "Tag/HDGameplayTag.h"
#include "Interface/HDWeaponInterface.h"

UHDGA_WeaponTrigger::UHDGA_WeaponTrigger()
	: CurrentTagContainer(FGameplayTagContainer())
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UHDGA_WeaponTrigger::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UHDGA_WeaponTrigger::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	CurrentTagContainer = GetAssetInputTags();
	CONDITION_CHECK(CurrentTagContainer.IsValid() == false);

	TScriptInterface<IHDWeaponInterface> WeaponInterface = ActorInfo->AvatarActor.Get();
	NULL_CHECK(WeaponInterface);

	if (CurrentTagContainer.HasTagExact(HDTAG_INPUT_FIRE))
	{
		WeaponInterface->Fire(true);
	}
	if (CurrentTagContainer.HasTagExact(HDTAG_INPUT_RELOAD))
	{
		WeaponInterface->Reload();
	}
}

void UHDGA_WeaponTrigger::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	TScriptInterface<IHDWeaponInterface> WeaponInterface = ActorInfo->AvatarActor.Get();
	NULL_CHECK(WeaponInterface);

	if (CurrentTagContainer.HasTagExact(HDTAG_INPUT_FIRE))
	{
		WeaponInterface->Fire(false);
	}

	const bool bReplicatedEndAbility = true;
	const bool bWasCancelled = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}
