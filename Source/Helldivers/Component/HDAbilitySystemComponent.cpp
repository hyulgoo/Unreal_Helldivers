// Fill out your copyright notice in the Description page of Project Settings.

#include "Component/HDAbilitySystemComponent.h"
#include "Attribute/HDHealthAttributeSet.h"
#include "Attribute/Player/HDPlayerSpeedAttributeSet.h"
#include "GameData/HDCharacterStat.h"

UHDAbilitySystemComponent::UHDAbilitySystemComponent()
    : InitAttributeStatEffect(nullptr)
{
    static ConstructorHelpers::FClassFinder<UGameplayEffect> InitStatGameplayEffectRef(TEXT("/Game/GB/GAS/Effect/GE_InitStats.GE_InitStats_C"));
    if (InitStatGameplayEffectRef.Succeeded())
    {
        InitAttributeStatEffect = InitStatGameplayEffectRef.Class;
    }
}

void UHDAbilitySystemComponent::AbilityInputTagTriggered(const FGameplayTag Tag)
{
    TArray<FGameplayAbilitySpec>& ActivatebleAbilities = GetActivatableAbilities();
    for (FGameplayAbilitySpec& Spec : ActivatebleAbilities)
    {
        const FGameplayTagContainer& TagContainer = Spec.Ability->GetAssetTags();
        if (TagContainer.IsValid() == false || TagContainer.HasTagExact(Tag) == false)
        {
            continue;
        }

        if (Spec.IsActive())
        {
            AbilitySpecInputPressed(Spec);
        }
        else
        {
            TryActivateAbility(Spec.Handle);
        }
    }
}

void UHDAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag Tag)
{
    TArray<FGameplayAbilitySpec>& ActivatebleAbilities = GetActivatableAbilities();
    for (FGameplayAbilitySpec& Spec : ActivatebleAbilities)
    {
        const FGameplayTagContainer& TagContainer = Spec.Ability->GetAssetTags();
        if (TagContainer.IsValid() == false || TagContainer.HasTagExact(Tag) == false)
        {
            continue;
        }

        if (Spec.IsActive())
        {
            AbilitySpecInputReleased(Spec);
        }
    }
}

void UHDAbilitySystemComponent::AbilityInputTagToggled(const FGameplayTag Tag)
{
    TArray<FGameplayAbilitySpec>& ActivatebleAbilities = GetActivatableAbilities();
    for (FGameplayAbilitySpec& Spec : ActivatebleAbilities)
    {
        const FGameplayTagContainer& TagContainer = Spec.Ability->GetAssetTags();
        if (TagContainer.IsValid() == false || TagContainer.HasTagExact(Tag) == false)
        {
            continue;
        }

        if (Spec.IsActive())
        {
            AbilitySpecInputReleased(Spec);
        }
        else
        {
            TryActivateAbility(Spec.Handle);
        }
    }
}

void UHDAbilitySystemComponent::InitAttributeSet()
{
	UHDHealthAttributeSet* HealthAttributeSet = NewObject<UHDHealthAttributeSet>(this);
	UHDPlayerSpeedAttributeSet* ApeedAttributeSet = NewObject<UHDPlayerSpeedAttributeSet>(this);
	AddAttributeSetSubobject(HealthAttributeSet);
	AddAttributeSetSubobject(ApeedAttributeSet);
}

void UHDAbilitySystemComponent::SetAttributeSetStat(FHDCharacterStat* StatData)
{
    NULL_CHECK(StatData);
    NULL_CHECK(InitAttributeStatEffect);

    FGameplayEffectContextHandle Context = MakeEffectContext();
    FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(InitAttributeStatEffect, 1.f, Context);
    VALID_CHECK(SpecHandle);

    SpecHandle.Data->SetSetByCallerMagnitude(HDTAG_DATA_STATUS_MAXHEALTH,       StatData->MaxHealth);
    SpecHandle.Data->SetSetByCallerMagnitude(HDTAG_DATA_STATUS_WALKSPEED,       StatData->WalkSpeed);
    SpecHandle.Data->SetSetByCallerMagnitude(HDTAG_DATA_STATUS_CRAWLINGSPEED,   StatData->CrawlingSpeed);
    SpecHandle.Data->SetSetByCallerMagnitude(HDTAG_DATA_STATUS_CROUCHSPEED,     StatData->CrouchSpeed);
    SpecHandle.Data->SetSetByCallerMagnitude(HDTAG_DATA_STATUS_SPRINTSPEED,     StatData->SprintSpeed);
    SpecHandle.Data->SetSetByCallerMagnitude(HDTAG_DATA_STATUS_MAXSTAMINA,      StatData->MaxStamina);

    ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}
