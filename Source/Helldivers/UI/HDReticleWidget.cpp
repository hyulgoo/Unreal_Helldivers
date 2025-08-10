// Fill out your copyright notice in the Description page of Project Settings.

#include "HDReticleWidget.h"
#include "Components/Image.h"
#include "GameData/HDWeaponData.h"
#include "Define/HDDefine.h"

void UHDReticleWidget::SetReticlaImage(const TArray<FHDWeaponCrosshairPair>& Images)
{
    CONDITION_CHECK(Images.Num() == static_cast<int32>(EHDCrosshair::Count));

    Img_Center->SetBrushFromTexture(Images[static_cast<int32>(EHDCrosshair::Center)].Texture);
    Img_Left->SetBrushFromTexture(Images[static_cast<int32>(EHDCrosshair::Left)].Texture);
    Img_Right->SetBrushFromTexture(Images[static_cast<int32>(EHDCrosshair::Right)].Texture);
    Img_Top->SetBrushFromTexture(Images[static_cast<int32>(EHDCrosshair::Top)].Texture);
    Img_Bottom->SetBrushFromTexture(Images[static_cast<int32>(EHDCrosshair::Bottom)].Texture);
}

void UHDReticleWidget::UpdateCrosshair(const float Spread)
{
    CONDITION_CHECK(Img_Left && Img_Right && Img_Top && Img_Bottom);

    Img_Left->SetRenderTranslation(FVector2D(0.f, -Spread));
    Img_Right->SetRenderTranslation(FVector2D(0.f, Spread));
    Img_Top->SetRenderTranslation(FVector2D(-Spread, 0.f));
    Img_Bottom->SetRenderTranslation(FVector2D(Spread, 0.f));
}
