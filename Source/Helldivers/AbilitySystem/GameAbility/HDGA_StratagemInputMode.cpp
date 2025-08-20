// Fill out your copyright notice in the Description page of Project Settings.

#include "HDGA_StratagemInputMode.h"
#include "AbilitySystem/GameplayAbilityHelper.h"

UHDGA_StratagemInputMode::UHDGA_StratagemInputMode(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UHDGA_StratagemInputMode::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    FGameplayAbilityHelper::SendGameplayEventToTarget(HDTAG_EVENT_STRATAGEMHUD_APPEAR, CurrentActorInfo, ActorInfo);
}

void UHDGA_StratagemInputMode::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

    FGameplayAbilityHelper::SendGameplayEventToTarget(HDTAG_CHARACTER_ACTION_HOLDSTRATAGEM, CurrentActorInfo, ActorInfo);
    FGameplayAbilityHelper::SendGameplayEventToTarget(HDTAG_EVENT_STRATAGEMHUD_DISAPPEAR, CurrentActorInfo, ActorInfo);
}
