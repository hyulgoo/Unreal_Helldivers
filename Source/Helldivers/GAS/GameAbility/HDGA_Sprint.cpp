// Fill out your copyright notice in the Description page of Project Settings.

#include "HDGA_Sprint.h"
#include "Attribute/Player/HDPlayerSpeedAttributeSet.h"
#include "Interface/HDCharacterMovementInterface.h"
#include "Define/HDDefine.h"

UHDGA_Sprint::UHDGA_Sprint()
    : StaminaDrainPerSecounds(0.f)
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

	TScriptInterface<IHDCharacterMovementInterface> CharacterMovementInterface = ActorInfo->AvatarActor.Get();
	NULL_CHECK(CharacterMovementInterface);

	CharacterMovementInterface->SetSprint(true);
}

void UHDGA_Sprint::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
     TScriptInterface<IHDCharacterMovementInterface> CharacterMovementInterface = ActorInfo->AvatarActor.Get();
     NULL_CHECK(CharacterMovementInterface);

     CharacterMovementInterface->SetSprint(false);

    const bool bReplicatedEndAbility = true;
    const bool bWasCancelled = true;
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicatedEndAbility, bWasCancelled);
}
