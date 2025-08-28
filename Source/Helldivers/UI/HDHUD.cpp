// Fill out your copyright notice in the Description page of Project Settings.

#include "HDHUD.h"
#include "UI/HDGASPlayerUserWidget.h"
#include "AbilitySystemComponent.h"
#include "Attribute/HDHealthAttributeSet.h"
#include "Attribute/HDWeaponAttributeSet.h"
#include "UI/HDReticleWidget.h"
#include "Weapon/HDWeapon.h"
#include "Define/HDDefine.h"
#include "Define//HDGameplayTag.h"
#include "Interface/HDWeaponInterface.h"
#include "AbilitySystem/GameplayAbilityHelper.h"
#include "Component\HDStratagemComponent.h"
#include "HDStratagemHUDUserWidget.h"

void AHDHUD::SetAbilitySystemComponent(UAbilitySystemComponent* ASC)
{
    NULL_CHECK(ASC);
    AbilitySystemComponent = ASC;
}

void AHDHUD::CreateDefaultWidget()
{
    NULL_CHECK_WITHOUT_LOG(AbilitySystemComponent);

    if(PlayerHUDWidgetClass)
    {
        PlayerHUDWidget = CreateWidget<UHDGASPlayerUserWidget>(GetWorld(), PlayerHUDWidgetClass);
        NULL_CHECK(PlayerHUDWidget);

        AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UHDHealthAttributeSet::GetCurrentHealthAttribute()).AddUObject(PlayerHUDWidget, &UHDGASPlayerUserWidget::OnHealthAttributeChangeds);
        AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UHDHealthAttributeSet::GetMaxHealthAttribute()).AddUObject(PlayerHUDWidget, &UHDGASPlayerUserWidget::OnHealthAttributeChangeds);
        AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UHDWeaponAttributeSet::GetAmmoAttribute()).AddUObject(PlayerHUDWidget, &UHDGASPlayerUserWidget::OnWeaponAttributeChangeds);
        AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UHDWeaponAttributeSet::GetMaxAmmoAttribute()).AddUObject(PlayerHUDWidget, &UHDGASPlayerUserWidget::OnWeaponAttributeChangeds);
        AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UHDWeaponAttributeSet::GetCapacityAttribute()).AddUObject(PlayerHUDWidget, &UHDGASPlayerUserWidget::OnWeaponAttributeChangeds);
        AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UHDWeaponAttributeSet::GetMaxCapacityAttribute()).AddUObject(PlayerHUDWidget, &UHDGASPlayerUserWidget::OnWeaponAttributeChangeds);
        AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UHDWeaponAttributeSet::GetGrenadeAttribute()).AddUObject(PlayerHUDWidget, &UHDGASPlayerUserWidget::OnWeaponAttributeChangeds);
        AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UHDWeaponAttributeSet::GetMaxGrenadeAttribute()).AddUObject(PlayerHUDWidget, &UHDGASPlayerUserWidget::OnWeaponAttributeChangeds);

        PlayerHUDWidget->AddToViewport();
    }

    if (StratagemWidgetClass)
    {
        StratagemWidget = CreateWidget<UHDStratagemHUDUserWidget>(GetWorld(), StratagemWidgetClass, FName("StratagemHUDWidget"));
        NULL_CHECK(StratagemWidget);

        const FGameplayTagContainer& StratagemEventTags = FGameplayAbilityHelper::GetAllChildTag(Tag_Event_StratagemHUD);
        for (const FGameplayTag& StratagemEventTag : StratagemEventTags)
        {
            AbilitySystemComponent->GenericGameplayEventCallbacks.FindOrAdd(StratagemEventTag).AddUObject(StratagemWidget, &UHDStratagemHUDUserWidget::OnStratagemEventReceived);
        }

        UHDStratagemComponent* StratagemComponent = PlayerOwner->GetPawn()->GetComponentByClass<UHDStratagemComponent>();
        NULL_CHECK(StratagemComponent);

        StratagemWidget->InitializeStratagemHUD(StratagemComponent);
        StratagemWidget->AddToViewport();
    }
}

void AHDHUD::OnEquipWeaponUIEventRecieved(const FGameplayEventData* Payload)
{
    NULL_CHECK_WITHOUT_LOG(AbilitySystemComponent);
    NULL_CHECK(ReticleWidgetClass);

    if (ReticleWidget)
    {
        NULL_CHECK(CurrentWeapon);
        CurrentWeapon->OnSpreadChanged.Remove(SpreadChangedHandle);
    }
    else
    {
        ReticleWidget = CreateWidget<UHDReticleWidget>(GetWorld(), ReticleWidgetClass);
        NULL_CHECK(ReticleWidget);

        ReticleWidget->AddToViewport();
    }

    TScriptInterface<IHDWeaponInterface> WeaponInterfae = PlayerOwner->GetPawn();
    NULL_CHECK(WeaponInterfae);

    //CurrentWeapon = WeaponInterfae->GetWeapon();
    //if (CurrentWeapon)
    //{
    //    SpreadChangedHandle = CurrentWeapon->OnSpreadChanged.AddUObject(ReticleWidget, &UHDReticleWidget::UpdateCrosshair);
    //}
}
