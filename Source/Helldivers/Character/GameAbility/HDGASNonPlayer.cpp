// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/GameAbility/HDGASNonPlayer.h"
#include "Attribute/HDHealthAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "Define/HDDefine.h"

AHDGASNonPlayer::AHDGASNonPlayer()
    : AbilitySystemComponent(nullptr)
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

    // �׽�Ʈ�� �ʱ�ȭ. ���߿� GE�� DataTable ������� ������ ��
    AbilitySystemComponent->InitStats(UHDHealthAttributeSet::StaticClass(), nullptr);

    UHDHealthAttributeSet* HealthAttributeSet = const_cast<UHDHealthAttributeSet*>(AbilitySystemComponent->GetSet<UHDHealthAttributeSet>());
    NULL_CHECK(HealthAttributeSet);
    HealthAttributeSet->MaxHealth.SetBaseValue(150.f);
    HealthAttributeSet->MaxHealth.SetCurrentValue(150.f);
}
