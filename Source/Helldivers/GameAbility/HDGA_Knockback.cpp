// Fill out your copyright notice in the Description page of Project Settings.

#include "GameAbility/HDGA_Knockback.h"
#include "Tag/HDGameplayTag.h"
#include "Define/HDDefine.h"
#include "Interface/HDCharacterMovementInterface.h"

UHDGA_Knockback::UHDGA_Knockback()
    : MovementInterface(nullptr)
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UHDGA_Knockback::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
    return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UHDGA_Knockback::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    if(TriggerEventData->EventTag == HDTAG_DATA_KNOCKBACK_HIT)
    {
        // Character Hit
        UE_LOG(LogTemp, Error, TEXT("Hit Tag Called!!"));
    }
    else if (TriggerEventData->EventTag == HDTAG_DATA_KNOCKBACK_RAGDOLL)
    {
        // Character Ragdoll
        MovementInterface = ActorInfo->AvatarActor.Get();
        NULL_CHECK(MovementInterface);
        
        const FVector Impulse = (TriggerEventData->Instigator->GetActorLocation() - ActorInfo->AvatarActor->GetActorLocation()).GetSafeNormal();

        MovementInterface->SetRagdoll(true, Impulse);

        World = ActorInfo->AvatarActor->GetWorld();
        VALID_CHECK(World);
        World->GetTimerManager().SetTimer(StateCheckTimerHandle, this, &UHDGA_Knockback::CheckCharacterRagdollState, 0.1f, true);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("EventTag in Knockback Ability is Invalid"));
    }
}

void UHDGA_Knockback::CheckCharacterRagdollState()
{
    NULL_CHECK(MovementInterface);
    VALID_CHECK(World);

    const float Speed = MovementInterface->GetRagdollPysicsLinearVelocity();

    if (Speed < 5.f && bRecoveryFromRagdoll == false)
    {
        bRecoveryFromRagdoll = true;

        World->GetTimerManager().ClearTimer(StateCheckTimerHandle);
        World->GetTimerManager().SetTimer(RecoveryFromRagdollTimerHandle, this, &UHDGA_Knockback::RecoveryFromRagdoll, 1.f, false);
    }
}

void UHDGA_Knockback::RecoveryFromRagdoll()
{
    NULL_CHECK(MovementInterface);
    VALID_CHECK(World);

    MovementInterface->SetRagdoll(false);
    World->GetTimerManager().ClearTimer(RecoveryFromRagdollTimerHandle);

    const bool bReplicatedEndAbility = true;
    const bool bWasCancelled = true;
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);

    bRecoveryFromRagdoll = false;
}

void UHDGA_Knockback::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
    Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}
