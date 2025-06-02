// Fill out your copyright notice in the Description page of Project Settings.

#include "HDGA_Knockback.h"
#include "Define/HDDefine.h"
#include "Tag/HDGameplayTag.h"
#include "Interface/HDCharacterRagdollInterface.h"

UHDGA_Knockback::UHDGA_Knockback()
    : RagdollInterface(nullptr)
    , World(nullptr)
    , bRecoveryFromRagdoll(false)
    , StateCheckTimerHandle(FTimerHandle())
	, RecoveryFromRagdollTimerHandle(FTimerHandle())
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
        RagdollInterface = ActorInfo->AvatarActor.Get();
        NULL_CHECK(RagdollInterface);
        
        const FVector& Direction = (TriggerEventData->Instigator->GetActorLocation() - ActorInfo->AvatarActor->GetActorLocation()).GetSafeNormal();
        const FVector Impulse = Direction * TriggerEventData->EventMagnitude;

        RagdollInterface->SetRagdoll(true, Impulse);

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
    NULL_CHECK(RagdollInterface);
    VALID_CHECK(World);

    const float Speed = RagdollInterface->GetRagdollPysicsLinearVelocity();

    if (Speed < 5.f && bRecoveryFromRagdoll == false)
    {
        bRecoveryFromRagdoll = true;

        World->GetTimerManager().ClearTimer(StateCheckTimerHandle);
        World->GetTimerManager().SetTimer(RecoveryFromRagdollTimerHandle, this, &UHDGA_Knockback::RecoveryFromRagdoll, 1.f, false);
    }
}

void UHDGA_Knockback::RecoveryFromRagdoll()
{
    NULL_CHECK(RagdollInterface);
    VALID_CHECK(World);

    RagdollInterface->SetRagdoll(false);
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
