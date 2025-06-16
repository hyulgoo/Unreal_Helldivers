// Fill out your copyright notice in the Description page of Project Settings.

#include "HDGA_DeadWatcher.h"
#include "Attribute/HDHealthAttributeSet.h"
#include "Define/HDDefine.h"
#include "Tag/HDGameplayTag.h"
#include "Interface//HDDeadInterface.h"

UHDGA_DeadWatcher::UHDGA_DeadWatcher()
{
}

void UHDGA_DeadWatcher::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
    VALID_CHECK(AbilitySystemComponent);

    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UHDHealthAttributeSet::GetCurrentHealthAttribute()).AddUObject(this, &UHDGA_DeadWatcher::OnHealthChanged);
}

void UHDGA_DeadWatcher::OnHealthChanged(const FOnAttributeChangeData& Data)
{
    if (Data.NewValue <= 0.f)
    {
        UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
        TArray<FGameplayAbilitySpec>& ActivatebleAbilities = AbilitySystemComponent->GetActivatableAbilities();
        for (FGameplayAbilitySpec& Spec : ActivatebleAbilities)
        {
            const FGameplayTagContainer& TagContainer = Spec.Ability->GetAssetTags();
            if (TagContainer.IsValid() && TagContainer.HasTagExact(GetAssetTags().First()))
            {
                AbilitySystemComponent->AbilitySpecInputReleased(Spec);
            }
        }

        TScriptInterface<IHDDeadInterface> DeadInterface = GetActorInfo().AvatarActor.Get();
        NULL_CHECK(DeadInterface);
        DeadInterface->SetDead();

        AbilitySystemComponent->AddLooseGameplayTag(HDTAG_CHARACTER_STATE_ISDEAD);

        const bool bReplicatedEndAbility = true;
        const bool bWasCancelled = false;
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
    }
}
