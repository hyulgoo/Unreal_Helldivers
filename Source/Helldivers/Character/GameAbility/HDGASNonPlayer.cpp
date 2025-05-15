// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/GameAbility/HDGASNonPlayer.h"
#include "Attribute/HDHealthAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "Define/HDDefine.h"

AHDGASNonPlayer::AHDGASNonPlayer()
{
    AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
}

UAbilitySystemComponent* AHDGASNonPlayer::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

void AHDGASNonPlayer::BeginPlay()
{
    Super::BeginPlay();

    NULL_CHECK(AbilitySystemComponent);
    NULL_CHECK(InitAttributeSetGameEffect);

    FGameplayEffectContextHandle Context = AbilitySystemComponent->MakeEffectContext();
    FGameplayEffectSpecHandle Spec = AbilitySystemComponent->MakeOutgoingSpec(InitAttributeSetGameEffect, 1.f, Context);

    CONDITION_CHECK(Spec.IsValid() == false);
    AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
}
