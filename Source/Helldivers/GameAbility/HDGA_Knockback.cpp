// Fill out your copyright notice in the Description page of Project Settings.

#include "GameAbility/HDGA_Knockback.h"
#include "Tag/HDGameplayTag.h"
#include "Define/HDDefine.h"

UHDGA_Knockback::UHDGA_Knockback()
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
        UE_LOG(LogTemp, Error, TEXT("Ragdoll Tag Called!!"));
        USkeletalMeshComponent* MeshComponent = ActorInfo->AvatarActor->GetComponentByClass<USkeletalMeshComponent>();
        NULL_CHECK(MeshComponent);

        MeshComponent->SetSimulatePhysics(true);

        // TODO 가해진 힘이 적어지면 해제되게
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("EventTag in Knockback Ability is Invalid"));
    }
}

void UHDGA_Knockback::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
    Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}
