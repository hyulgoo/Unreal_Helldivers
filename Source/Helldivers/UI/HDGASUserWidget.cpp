// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/HDGASUserWidget.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Define/HDDefine.h"

void UHDGASUserWidget::SetAbilitySystemComponent(UAbilitySystemComponent* NewASC)
{
    NULL_CHECK(NewASC);

    AbilitySystemComponent = NewASC;
}

UAbilitySystemComponent* UHDGASUserWidget::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}
