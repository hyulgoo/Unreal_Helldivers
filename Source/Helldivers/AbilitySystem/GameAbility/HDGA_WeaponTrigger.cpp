// Fill out your copyright notice in the Description page of Project Settings.

#include "HDGA_WeaponTrigger.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Interface/HDWeaponInterface.h"

UHDGA_WeaponTrigger::UHDGA_WeaponTrigger(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
    : Super(ObjectInitializer)
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UHDGA_WeaponTrigger::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    const FGameplayTagContainer& CurrentTagContainer = GetAssetTags();
	CONDITION_CHECK(CurrentTagContainer.IsValid());

	WeaponInterface = ActorInfo->AvatarActor.Get();
	NULL_CHECK(WeaponInterface);

	if (CurrentTagContainer.HasTagExact(HDTAG_TRIGGER_ATTACK))
	{
		WeaponInterface->Attack(true);
        SavedDelay = WeaponInterface->GetWeaponFireDelay();
		SetAbilityTimer();
	}
	else if (CurrentTagContainer.HasTagExact(HDTAG_TRIGGER_RELOAD))
	{
        SavedDelay = WeaponInterface->Reload();
		SetAbilityTimer();
	}
}

void UHDGA_WeaponTrigger::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	const FGameplayTagContainer& CurrentTagContainer = GetAssetTags();
	CONDITION_CHECK(CurrentTagContainer.IsValid());

	NULL_CHECK(WeaponInterface);

	if (CurrentTagContainer.HasTagExact(HDTAG_TRIGGER_ATTACK))
	{
		WeaponInterface->Attack(false);
	}
	else if (CurrentTagContainer.HasTagExact(HDTAG_TRIGGER_RELOAD))
	{
		WeaponInterface->ReloadFinished();
	}

    WeaponInterface = nullptr;
}

void UHDGA_WeaponTrigger::OnDelayCompleted()
{
    const FGameplayTagContainer& CurrentTagContainer = GetAssetTags();
	CONDITION_CHECK(CurrentTagContainer.IsValid());

	NULL_CHECK(WeaponInterface);

	if (CurrentTagContainer.HasTagExact(HDTAG_TRIGGER_ATTACK))
	{
		if (WeaponInterface->FireFinished())
		{
			SetAbilityTimer();
		}
	}
	else if (CurrentTagContainer.HasTagExact(HDTAG_TRIGGER_RELOAD))
	{
		const bool bReplicatedEndAbility = true;
		const bool bWasCancelled = true;
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
	}
}

void UHDGA_WeaponTrigger::SetAbilityTimer()
{
	UAbilityTask_WaitDelay* DelayTask = UAbilityTask_WaitDelay::WaitDelay(this, SavedDelay);
	DelayTask->OnFinish.AddDynamic(this, &UHDGA_WeaponTrigger::OnDelayCompleted);
	DelayTask->ReadyForActivation();
}
