// Fill out your copyright notice in the Description page of Project Settings.

#include "HDGA_Reload.h"
#include "AbilitySystemComponent.h"
#include "Interface/HDWeaponInterface.h"
#include "AbilitySystem/AbilityTask/HDAT_PlayMontageAndWaitForEvent.h"
#include "Character/CharacterTypes/HDCharacterStateTypes.h"
#include "Attribute/HDWeaponAttributeSet.h"
#include "Define/HDMontageSectionNames.h"

bool UHDGA_Reload::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags /*= nullptr*/, const FGameplayTagContainer* TargetTags /*= nullptr*/, OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr*/) const
{
    CONDITION_CHECK_WITH_RETURNTYPE_WITHOUT_LOG(Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags), false);

    UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
    NULL_CHECK_WITH_RETURNTYPE(ASC, false);

    const UHDWeaponAttributeSet* WeaponAttributeSet = ASC->GetSet<UHDWeaponAttributeSet>();
    NULL_CHECK_WITH_RETURNTYPE(WeaponAttributeSet, false);

    return (WeaponAttributeSet->GetAmmo() < WeaponAttributeSet->GetMaxAmmo() && WeaponAttributeSet->GetCapacity() > 0.f);
}

void UHDGA_Reload::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    TScriptInterface<IHDWeaponInterface> WeaponInterface = ActorInfo->AvatarActor.Get();
    NULL_CHECK(WeaponInterface);

    WeaponInterface->PlayWeaponMontage(EHDCombatMontage::Reload);

    UAnimMontage* CombatMontage = WeaponInterface->GetCombatMontage(EHDCombatMontage::Reload);
    NULL_CHECK(CombatMontage);

    UHDAT_PlayMontageAndWaitForEvent* PlayMontageAndWaitEventTask = UHDAT_PlayMontageAndWaitForEvent::PlayMontageAndWaitEvent(
        this,
        CombatMontage,
        FGameplayTagContainer(Tag_Event_Reload)
    );

    PlayMontageAndWaitEventTask->OnEventReceived.AddDynamic(this, &UHDGA_Reload::OnEventRecieved);
    PlayMontageAndWaitEventTask->ReadyForActivation();
}               

void UHDGA_Reload::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UHDGA_Reload::OnEventRecieved(FGameplayEventData Payload)
{
    NULL_CHECK(ReloadEffectClass);

    UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
    NULL_CHECK(ASC);

    const FGameplayEffectSpecHandle& SpecHandle = ASC->MakeOutgoingSpec(ReloadEffectClass, 1.f, ASC->MakeEffectContext());
    CONDITION_CHECK(SpecHandle.IsValid());

    ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, true);
}
