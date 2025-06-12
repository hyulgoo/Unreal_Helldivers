// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/GameAbility/HDGA_WeaponTrigger.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Define/HDDefine.h"
#include "Tag/HDGameplayTag.h"
#include "Interface/HDWeaponInterface.h"
#include "Weapon/HDWeapon.h"

UHDGA_WeaponTrigger::UHDGA_WeaponTrigger()
	: WeaponInterface(nullptr)
	, SavedHandle(FGameplayAbilitySpecHandle())
	, SavedActorInfo(nullptr)
	, SavedActivationInfo(EGameplayAbilityActivationMode::Rejected)
	, SavedDelay(0.f)
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

	FGameplayTagContainer CurrentTagContainer = GetAssetTags();
	CONDITION_CHECK(CurrentTagContainer.IsValid() == false);

	WeaponInterface = ActorInfo->AvatarActor.Get();
	NULL_CHECK(WeaponInterface);

	if (CurrentTagContainer.HasTagExact(HDTAG_INPUT_FIRE))
	{
		WeaponInterface->Attack(true);

		AHDWeapon* Weapon = WeaponInterface->GetWeapon();
		const float FireDelay = Weapon->GetFireDelay();
		SetAbilityTimer(Handle, ActorInfo, ActivationInfo, FireDelay);
	}
	else if (CurrentTagContainer.HasTagExact(HDTAG_INPUT_RELOAD))
	{
		const float ReloadDelay = WeaponInterface->Reload();
		SetAbilityTimer(Handle, ActorInfo, ActivationInfo, ReloadDelay);
	}
}

void UHDGA_WeaponTrigger::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	const bool bReplicatedEndAbility = true;
	const bool bWasCancelled = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}

void UHDGA_WeaponTrigger::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	FGameplayTagContainer CurrentTagContainer = GetAssetTags();
	CONDITION_CHECK(CurrentTagContainer.IsValid() == false);

	NULL_CHECK(WeaponInterface);

	if (CurrentTagContainer.HasTagExact(HDTAG_INPUT_FIRE))
	{
		WeaponInterface->Attack(false);
	}
	else if (CurrentTagContainer.HasTagExact(HDTAG_INPUT_RELOAD))
	{
		WeaponInterface->ReloadFinished();
	}
}

void UHDGA_WeaponTrigger::OnDelayCompleted()
{
	FGameplayTagContainer CurrentTagContainer = GetAssetTags();
	CONDITION_CHECK(CurrentTagContainer.IsValid() == false);

	NULL_CHECK(WeaponInterface);

	if (CurrentTagContainer.HasTagExact(HDTAG_INPUT_FIRE))
	{
		if (WeaponInterface->FireFinished())
		{
			SetAbilityTimer(SavedHandle, SavedActorInfo, SavedActivationInfo, SavedDelay);
		}
	}
	else if (CurrentTagContainer.HasTagExact(HDTAG_INPUT_RELOAD))
	{
		const bool bReplicatedEndAbility = true;
		const bool bWasCancelled = true;
		EndAbility(SavedHandle, SavedActorInfo, SavedActivationInfo, bReplicatedEndAbility, bWasCancelled);
	}
}

void UHDGA_WeaponTrigger::SetAbilityTimer(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const float Delay)
{
	SavedHandle			= Handle;
	SavedActorInfo		= ActorInfo;
	SavedActivationInfo = ActivationInfo;
	SavedDelay			= Delay;

	UAbilityTask_WaitDelay* DelayTask = UAbilityTask_WaitDelay::WaitDelay(this, Delay);
	DelayTask->OnFinish.AddDynamic(this, &UHDGA_WeaponTrigger::OnDelayCompleted);
	DelayTask->ReadyForActivation();
}
