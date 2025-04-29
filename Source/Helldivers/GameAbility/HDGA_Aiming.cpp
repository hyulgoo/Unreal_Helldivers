// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAbility/HDGA_Aiming.h"
#include "HDGA_Aiming.h"
#include "Interface/HDCharacterMovementInterface.h"
#include "Character/CharacterTypes/HDCharacterControlTypes.h"

UHDGA_Aiming::UHDGA_Aiming()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UHDGA_Aiming::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
    if (Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags) == false)
    {
        return false;
    }

    IHDCharacterMovementInterface* CharacterMovementInterface = Cast<IHDCharacterMovementInterface>(ActorInfo->AvatarActor.Get());
    return (CharacterMovementInterface != nullptr);
}

void UHDGA_Aiming::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    IHDCharacterMovementInterface* CharacterAimingInterface = Cast<IHDCharacterMovementInterface>(ActorInfo->AvatarActor.Get());
    if (CharacterAimingInterface)
    {
        CharacterAimingInterface->ChangeCharacterControlType();
    }
}
