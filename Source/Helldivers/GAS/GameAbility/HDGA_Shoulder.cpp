// Fill out your copyright notice in the Description page of Project Settings.

#include "HDGA_Shoulder.h"
#include "Define/HDDefine.h"
#include "GameFramework/Character.h"
#include "Interface/HDCharacterMovementInterface.h"

UHDGA_Shoulder::UHDGA_Shoulder()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UHDGA_Shoulder::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
    return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UHDGA_Shoulder::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    TScriptInterface<IHDCharacterMovementInterface> CharacterMovementInterface = ActorInfo->AvatarActor.Get();
    NULL_CHECK(CharacterMovementInterface);
    CharacterMovementInterface->SetShouldering(true);
}

void UHDGA_Shoulder::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
    TScriptInterface<IHDCharacterMovementInterface> CharacterMovementInterface = ActorInfo->AvatarActor.Get();
    NULL_CHECK(CharacterMovementInterface);
    CharacterMovementInterface->SetShouldering(false);

    const bool bReplicatedEndAbility = true;
    const bool bWasCancelled = true;
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}