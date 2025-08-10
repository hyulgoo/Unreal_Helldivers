// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/HDGASPlayerUserWidget.h"
#include "Attribute/HDHealthAttributeSet.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Define/HDDefine.h"
#include "Define//HDGameplayTag.h"

void UHDGASPlayerUserWidget::OnHealthAttributeChangeds(const FOnAttributeChangeData& ChangeData)
{
    if(ChangeData.Attribute == UHDHealthAttributeSet::GetCurrentHealthAttribute())
    {
        CurrentHealth = ChangeData.NewValue;
    }
    else if(ChangeData.Attribute == UHDHealthAttributeSet::GetMaxHealthAttribute())
    {
        CurrentMaxHealth = ChangeData.NewValue;
    }

    UpdateProgressbar(Pb_HPbar, CurrentHealth / CurrentMaxHealth);
}

void UHDGASPlayerUserWidget::SetChangedWeaponAmmoCountInfo(const int32 NewAmmoCount, const int32 NewMaxAmmoCount)
{
    MaxAmmoCount = NewMaxAmmoCount;
    const float Ratio = static_cast<float>(NewAmmoCount) / static_cast<float>(MaxAmmoCount);
    UpdateProgressbar(Pb_Ammo, Ratio);
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

void UHDGASPlayerUserWidget::OnWeaponInfoChanged(const FGameplayEventData* Payload)
{
    if (Payload->EventTag == HDTAG_EVENT_PLAYERHUD_CURRENTAMMOCHANGE)
    {
        const float CurrentAmmoCount = Payload->EventMagnitude;
        const float Ratio = CurrentAmmoCount / static_cast<float>(MaxAmmoCount);
        UpdateProgressbar(Pb_Ammo, Ratio);
    }
    else if(Payload->EventTag == HDTAG_EVENT_PLAYERHUD_CURRENTCAPACITYCHANGE)
    {
        OnCapacityCountChanged(static_cast<int32>(Payload->EventMagnitude));
    }
    else if (Payload->EventTag == HDTAG_EVENT_PLAYERHUD_MAXAMMOCHANGE)
    {
        MaxAmmoCount = static_cast<int32>(Payload->EventMagnitude);
        UpdateProgressbar(Pb_Ammo, 1.f);
    }
    else if (Payload->EventTag == HDTAG_EVENT_PLAYERHUD_MAXCAPACITYCHANGE)
    {
        MaxCapacityCount = static_cast<int32>(Payload->EventMagnitude);
        OnCapacityCountChanged(MaxCapacityCount);
    }
}

void UHDGASPlayerUserWidget::OnCapacityCountChanged(const int32 NewCapacityCount)
{
    const FText CapacityCountText = FText::Format(NSLOCTEXT("Ammo", "AmmoForamt", "{0}/{1}"),
        FText::AsNumber(NewCapacityCount), FText::AsNumber(MaxCapacityCount));
    UpdateTextblock(Text_CapacityCount, CapacityCountText);
}

void UHDGASPlayerUserWidget::OnGrenadeCountChanged(const int32 NewGrenadeCount)
{
    const FText GrenadeCountText = FText::Format(NSLOCTEXT("Grenade", "GrenadeForamt", "{0}/{1}"),
        FText::AsNumber(NewGrenadeCount), FText::AsNumber(MaxCapacityCount));
    UpdateTextblock(Text_CapacityCount, GrenadeCountText);
}

void UHDGASPlayerUserWidget::UpdateProgressbar(UProgressBar* Progressbar, const float Value)
{
    NULL_CHECK(Progressbar);
    CONDITION_CHECK(Value <= 0.f && Value <= 1.f);

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
