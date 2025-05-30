// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/GameAbility/HDGA_CommonTrigger.h"
#include "Tag/HDGameplayTag.h"

UHDGA_CommonTrigger::UHDGA_CommonTrigger()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UHDGA_CommonTrigger::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UHDGA_CommonTrigger::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (TriggerEventData->EventTag == HDTAG_CHARACTER_STATE_ISAIMING)
	{
	
	}
	else if (TriggerEventData->EventTag == HDTAG_CHARACTER_STATE_ISFIRE)
	{
		
	}
	else if (TriggerEventData->EventTag == HDTAG_CHARACTER_STATE_ISSHOULDER)
	{

	}
	else if (TriggerEventData->EventTag == HDTAG_CHARACTER_STATE_ISSPRINT)
	{

	}
}

void UHDGA_CommonTrigger::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{

	const bool bReplicatedEndAbility = true;
	const bool bWasCancelled = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}
