// Fill out your copyright notice in the Description page of Project Settings.

#include "GameAbility/HDGA_Sprint.h"
#include "Attribute/Player/HDPlayerSpeedAttributeSet.h"
#include "Interface/HDCharacterMovementInterface.h"
#include "Define/HDDefine.h"

UHDGA_Sprint::UHDGA_Sprint()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UHDGA_Sprint::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
    return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UHDGA_Sprint::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    if(ActorInfo->AvatarActor.IsValid())
    {
        IHDCharacterMovementInterface* CharacterMovementInterface = Cast<IHDCharacterMovementInterface>(ActorInfo->AvatarActor.Get());
        NULL_CHECK(CharacterMovementInterface);

        CharacterMovementInterface->SetSprint(true);
    }   
}

void UHDGA_Sprint::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    if (ActorInfo->AvatarActor.IsValid())
    {
        IHDCharacterMovementInterface* CharacterMovementInterface = Cast<IHDCharacterMovementInterface>(ActorInfo->AvatarActor.Get());
        NULL_CHECK(CharacterMovementInterface);

        CharacterMovementInterface->SetSprint(false);
    }

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
