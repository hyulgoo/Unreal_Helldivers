// Fill out your copyright notice in the Description page of Project Settings.

#include "HDHUD.h"
#include "UI/HDGASPlayerUserWidget.h"
#include "UI/HDReticleWidget.h"
#include "Define/HDDefine.h"

void AHDHUD::CreateDefaultWidget(UAbilitySystemComponent* ASC)
{
    NULL_CHECK(ASC);

    if (PlayerHUDWidgetClass)
    {
        PlayerHUDWidget = CreateWidget<UHDGASPlayerUserWidget>(GetWorld(), PlayerHUDWidgetClass);
        NULL_CHECK(PlayerHUDWidget);

        PlayerHUDWidget->AddToViewport();
        PlayerHUDWidget->SetAbilitySystemComponent(ASC);
    }

    if (ReticleWidgetClass)
    {
        ReticleWidget = CreateWidget<UHDReticleWidget>(GetWorld(), ReticleWidgetClass);
        NULL_CHECK(ReticleWidget);

        ReticleWidget->AddToViewport();
        ReticleWidget->SetAbilitySystemComponent(ASC);
    }
}
