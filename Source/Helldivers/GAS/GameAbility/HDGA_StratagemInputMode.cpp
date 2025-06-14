// Fill out your copyright notice in the Description page of Project Settings.

#include "HDGA_StratagemInputMode.h"
#include "Interface/HDCharacterCommandInterface.h"
#include "AbilitySystemComponent.h"
#include "Define/HDDefine.h"
#include "Tag/HDGameplayTag.h"

UHDGA_StratagemInputMode::UHDGA_StratagemInputMode()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UHDGA_StratagemInputMode::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
    return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UHDGA_StratagemInputMode::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    FGameplayEventData EventData;
    EventData.EventTag = HDTAG_EVENT_STRATAGEMHUD_APPEAR;
    EventData.Instigator = GetAvatarActorFromActorInfo();
    EventData.Target = GetAvatarActorFromActorInfo();

    ActorInfo->AbilitySystemComponent->HandleGameplayEvent(EventData.EventTag, &EventData);
}

void UHDGA_StratagemInputMode::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
    TScriptInterface<IHDCharacterCommandInterface> CharacterCommandInterface = ActorInfo->AvatarActor.Get();
    NULL_CHECK(CharacterCommandInterface);

    CharacterCommandInterface->HoldStratagem();

    FGameplayEventData EventData;
    EventData.EventTag = HDTAG_EVENT_STRATAGEMHUD_DISAPPEAR;
    EventData.Instigator = GetAvatarActorFromActorInfo();
    EventData.Target = GetAvatarActorFromActorInfo();

    ActorInfo->AbilitySystemComponent->HandleGameplayEvent(EventData.EventTag, &EventData);

    const bool bReplicatedEndAbility = true;
    const bool bWasCancelled = true;
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicatedEndAbility, bWasCancelled);
}
