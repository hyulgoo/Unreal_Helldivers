// Fill out your copyright notice in the Description page of Project Settings.


#include "HDGA_Shoulder.h"
#include "GameFramework/Character.h"
#include "Interface/HDCharacterMovementInterface.h"

UHDGA_Shoulder::UHDGA_Shoulder()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UHDGA_Shoulder::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
    if (Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags) == false)
    {
        return false;
    }
    
    const ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
    return (Character && Character->CanJump());
}

void UHDGA_Shoulder::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    IHDCharacterMovementInterface* CharacterAimingInterface = Cast<IHDCharacterMovementInterface>(ActorInfo->AvatarActor.Get());
    if (CharacterAimingInterface)
    {
        CharacterAimingInterface->SetShouldering(true);
    }
}

void UHDGA_Shoulder::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
    IHDCharacterMovementInterface* CharacterAimingInterface = Cast<IHDCharacterMovementInterface>(ActorInfo->AvatarActor.Get());
    if (CharacterAimingInterface)
    {
        CharacterAimingInterface->SetShouldering(false);
    }

    const bool bReplicatedEndAbility = true;
    const bool bWasCancelled = true;
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}