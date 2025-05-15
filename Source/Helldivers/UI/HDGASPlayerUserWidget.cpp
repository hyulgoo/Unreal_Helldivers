// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HDGASPlayerUserWidget.h"
#include "Attribute/HDHealthAttributeSet.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Define/HDDefine.h"
#include "Weapon/HDWeapon.h"

void UHDGASPlayerUserWidget::SetAbilitySystemComponent(UAbilitySystemComponent* NewAbilitySystemComponent)
{
    Super::SetAbilitySystemComponent(NewAbilitySystemComponent);

    NULL_CHECK(AbilitySystemComponent);
    
    //AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UHDCharacterPlayerSpeedAttributeSet::GetCurrentHealthAttribute()).AddUObject(this, &UHDGASPlayerUserWidget::OnHealthChangeds);
    //AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UHDCharacterPlayerSpeedAttributeSet::GetMaxHealthAttribute()).AddUObject(this, &UHDGASPlayerUserWidget::OnMaxHealthChangeds);
    //
    //const UHDCharacterPlayerSpeedAttributeSet* AttributeSet = AbilitySystemComponent->GetSet<UHDCharacterPlayerSpeedAttributeSet>();
    //NULL_CHECK(AttributeSet);
    //
    //CurrentHealth = AttributeSet->GetCurrentHealth();
    //CurrentMaxHealth = AttributeSet->GetMaxHealth();
    //if (CurrentMaxHealth <= 0.f)
    //{
    //    UE_LOG(LogTemp, Error, TEXT("MaxHealth is Invalid!"));
    //    return;
    //}
}

void UHDGASPlayerUserWidget::OnHealthChangeds(const FOnAttributeChangeData& ChangeData)
{
    CurrentHealth = ChangeData.NewValue;
    UpdateProgressbar(Pb_HPbar, CurrentHealth / CurrentMaxHealth);
}

void UHDGASPlayerUserWidget::OnMaxHealthChangeds(const FOnAttributeChangeData& ChangeData)
{
    CurrentMaxHealth = ChangeData.NewValue;
    UpdateProgressbar(Pb_HPbar, CurrentHealth / CurrentMaxHealth);
}

void UHDGASPlayerUserWidget::SetChangedWeaponAmmoCountInfo(const int32 NewAmmoCount, const int32 NewMaxAmmoCount)
{
    MaxAmmoCount = NewMaxAmmoCount; 
    OnAmmoCountChanged(NewAmmoCount);
}

void UHDGASPlayerUserWidget::SetChangedWeaponCapacityCountInfo(const int32 NewCapacityCount, const int32 NewMaxCapacityCount)
{
    MaxCapacityCount = NewMaxCapacityCount;
    OnCapacityCountChanged(NewCapacityCount);
}

void UHDGASPlayerUserWidget::SetGrenadeCountInfo(const int32 NewGrenadeCount, const int32 NewMaxGrenadeCount)
{
    MaxGrenadeCount = NewMaxGrenadeCount;
    OnGrenadeCountChanged(NewGrenadeCount);
}

void UHDGASPlayerUserWidget::OnAmmoCountChanged(const int32 NewAmmoCount)
{
    const float Ratio = static_cast<float>(NewAmmoCount) / static_cast<float>(MaxAmmoCount);
    UpdateProgressbar(Pb_Ammo, Ratio);
}

void UHDGASPlayerUserWidget::OnCapacityCountChanged(const int32 NewCapacityCount)
{
    FText CapacityCountText = FText::Format(NSLOCTEXT("Ammo", "AmmoForamt", "{0}/{1}"),
        FText::AsNumber(NewCapacityCount), FText::AsNumber(MaxCapacityCount));
    UpdateTextblock(Text_CapacityCount, CapacityCountText);
}

void UHDGASPlayerUserWidget::OnGrenadeCountChanged(const int32 NewGrenadeCount)
{
    FText GrenadeCountText = FText::Format(NSLOCTEXT("Grenade", "GrenadeForamt", "{0}/{1}"),
        FText::AsNumber(NewGrenadeCount), FText::AsNumber(MaxCapacityCount));
    UpdateTextblock(Text_CapacityCount, GrenadeCountText);
}

void UHDGASPlayerUserWidget::UpdateProgressbar(UProgressBar* Progressbar, const float Value)
{
    NULL_CHECK(Progressbar);

    if (Value < 0.f || Value > 1.f)
    {
        UE_LOG(LogTemp, Error, TEXT("Progressbar Value is Invalid!"));
        return;
    }

    Progressbar->SetPercent(Value);
}

void UHDGASPlayerUserWidget::UpdateTextblock(UTextBlock* TextBlock, const FText& Text)
{
    NULL_CHECK(TextBlock);

    TextBlock->SetText(Text);
}

void UHDGASPlayerUserWidget::UpdateImage(UImage* ImageWidget, UTexture2D* Texture)
{
    NULL_CHECK(ImageWidget);

    ImageWidget->SetBrushFromTexture(Texture);
}
