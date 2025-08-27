// Fill out your copyright notice in the Description page of Project Settings.

#include "HDGA_WeaponTrigger.h"
#include "Interface/HDWeaponInterface.h"
#include "Character/CharacterTypes/HDCharacterStateTypes.h"
#include "AbilitySystem/AbilityTask/HDAT_PlayMontageAndWaitForEvent.h"
#include "Define/HDMontageSectionNames.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Define/HDDefine.h"

UHDGA_WeaponTrigger::UHDGA_WeaponTrigger(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
    : Super(ObjectInitializer)
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UHDGA_WeaponTrigger::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags /*= nullptr*/, const FGameplayTagContainer* TargetTags /*= nullptr*/, OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr*/) const
{
    CONDITION_CHECK_WITH_RETURNTYPE_WITHOUT_LOG(Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags), false);

    return CheckCost(Handle, ActorInfo);
}

void UHDGA_WeaponTrigger::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    const FGameplayTagContainer& CurrentTagContainer = GetAssetTags();
    CONDITION_CHECK(CurrentTagContainer.IsValid());

    WeaponInterface = ActorInfo->AvatarActor.Get();
    NULL_CHECK(WeaponInterface);

    AutoFireDelay = WeaponInterface->GetWeaponFireDelay();
    UAnimMontage* CombatMontage = nullptr;
    bool bTaskEndOnMontageEnded = true;

    if (CurrentTagContainer.HasTagExact(HDTAG_TRIGGER_ATTACK))
    {
        CombatMontage = WeaponInterface->GetCombatMontage(EHDCombatMontage::Fire);
        WeaponInterface->PlayWeaponMontage(EHDCombatMontage::Fire);
        SectionName = WeaponInterface->IsShoulder() ? HDMONTAGE_SECTIONNAME_RIFLE_AIM : HDMONTAGE_SECTIONNAME_RIFLE_HIP;
        bTaskEndOnMontageEnded = false;
    }
    else if (CurrentTagContainer.HasTagExact(HDTAG_TRIGGER_RELOAD))
    {
        CombatMontage = WeaponInterface->GetCombatMontage(EHDCombatMontage::Reload);
        WeaponInterface->PlayWeaponMontage(EHDCombatMontage::Reload);
    }

    NULL_CHECK(CombatMontage);
    PlayMontageAndWaitEventTask = UHDAT_PlayMontageAndWaitForEvent::PlayMontageAndWaitEvent(
        this,
        CombatMontage,
        EventTags,
        1.f,
        SectionName,
        bTaskEndOnMontageEnded
    );

    PlayMontageAndWaitEventTask->OnEventReceived.AddDynamic(this, &UHDGA_WeaponTrigger::OnEventRecieved);
    PlayMontageAndWaitEventTask->ReadyForActivation();
}

void UHDGA_WeaponTrigger::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

    WeaponInterface = nullptr;
    AutoFireDelay = 0.f;
}

void UHDGA_WeaponTrigger::OnEventRecieved(FGameplayEventData Payload)
{
    NULL_CHECK(WeaponInterface);

    if (Payload.EventTag == HDTAG_EVENT_SPAWN_PROJECTILE)
    {
        if(CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo))
        {
            WeaponInterface->SpawnProjectile();

            if (WeaponInterface->IsWeaponAutoFire())
            {
                UAbilityTask_WaitDelay* Task = UAbilityTask_WaitDelay::WaitDelay(this, AutoFireDelay);
                Task->OnFinish.AddDynamic(this, &UHDGA_WeaponTrigger::OnFireDelayCompleted);
                Task->ReadyForActivation();

                PlayMontageAndWaitEventTask->JumpToSection(SectionName);
            }
            else
            {
                EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, true);
            }
        }
        else
        {
            EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, true);
        }
    }
    else if (Payload.EventTag == HDTAG_EVENT_RELOAD)
    {
        WeaponInterface->Reload();
    }
}

void UHDGA_WeaponTrigger::OnFireDelayCompleted()
{
    NULL_CHECK(WeaponInterface);
    WeaponInterface->PlayWeaponMontage(EHDCombatMontage::Fire);

    NULL_CHECK(PlayMontageAndWaitEventTask);
    PlayMontageAndWaitEventTask->ResetMontage();
}
