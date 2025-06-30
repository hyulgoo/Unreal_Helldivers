// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/GameAbility/HDGASNonPlayer.h"
#include "Attribute/HDHealthAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "Define/HDDefine.h"
#include "Tag/HDGameplayTag.h"

AHDGASNonPlayer::AHDGASNonPlayer()
    : AbilitySystemComponent(nullptr)
    , StartAbilities{}
    , InitStatEffect(nullptr)
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

    // 테스트용 초기화. 나중에 DataTable 기반으로 변경할 것

    UHDHealthAttributeSet* HealthAttributeSet = NewObject<UHDHealthAttributeSet>(this);
    AbilitySystemComponent->AddAttributeSetSubobject(HealthAttributeSet);

    FGameplayEffectContextHandle Context = AbilitySystemComponent->MakeEffectContext();
    FGameplayEffectSpecHandle InitStatusSpec = AbilitySystemComponent->MakeOutgoingSpec(InitStatEffect, 1.f, Context);
    CONDITION_CHECK(InitStatusSpec.IsValid() == false);

    InitStatusSpec.Data->SetSetByCallerMagnitude(HDTAG_DATA_STATUS_MAXHEALTH, 150.f);

    AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*InitStatusSpec.Data.Get());
}
