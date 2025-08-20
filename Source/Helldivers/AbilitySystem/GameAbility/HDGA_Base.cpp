// Fill out your copyright notice in the Description page of Project Settings.

#include "HDGA_Base.h"

UHDGA_Base::UHDGA_Base(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    TargetBlockedTags.AddTag(HDTAG_CHARACTER_STATE_DEAD);
}

bool UHDGA_Base::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags /*= nullptr*/, const FGameplayTagContainer* TargetTags /*= nullptr*/, OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr*/) const
{
    CONDITION_CHECK_WITH_RETURNTYPE_WITHOUT_LOG(Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags), false);

    return true;
}

void UHDGA_Base::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
    const bool bReplicatedEndAbility = false;
    const bool bWasCancelled = true;
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}
