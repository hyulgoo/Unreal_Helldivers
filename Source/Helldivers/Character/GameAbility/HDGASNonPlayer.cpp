// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/GameAbility/HDGASNonPlayer.h"
#include "Attribute/HDHealthAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "Define/HDDefine.h"

AHDGASNonPlayer::AHDGASNonPlayer()
    : AbilitySystemComponent(nullptr)
    , StartAbilities{}
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

    InitAbilitySystemComponent();
}

void AHDGASNonPlayer::InitAbilitySystemComponent()
{
    NULL_CHECK(AbilitySystemComponent);

	for (const TSubclassOf<UGameplayAbility>& StartAbility : StartAbilities)
	{
		AbilitySystemComponent->GiveAbility(StartAbility);
	}

    InitializeAttributeSet();
}

void AHDGASNonPlayer::InitializeAttributeSet()
{
    NULL_CHECK(AbilitySystemComponent);

    // 테스트용 초기화. 나중에 GE나 DataTable 기반으로 변경할 것
    AbilitySystemComponent->InitStats(UHDHealthAttributeSet::StaticClass(), nullptr);

    UHDHealthAttributeSet* HealthAttributeSet = const_cast<UHDHealthAttributeSet*>(AbilitySystemComponent->GetSet<UHDHealthAttributeSet>());
    NULL_CHECK(HealthAttributeSet);
    HealthAttributeSet->MaxHealth.SetBaseValue(150.f);
    HealthAttributeSet->MaxHealth.SetCurrentValue(150.f);
    HealthAttributeSet->CurrentHealth.SetBaseValue(150.f);
    HealthAttributeSet->CurrentHealth.SetCurrentValue(150.f);
}
