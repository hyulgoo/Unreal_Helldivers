// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/HDStratagemHUDUserWidget.h"
#include "Define/HDDefine.h"
#include "GameData/HDStratagemData.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/ScaleBox.h"
#include "UI/HDStratagemInfoUserWidget.h"

void UHDStratagemHUDUserWidget::SetStratagemListHUD(UDataTable* StratagemDataTable)
{
    FVector2D ViewportSize = FVector2D();
    if (GEngine && GEngine->GameViewport)
    {
        GEngine->GameViewport->GetViewportSize(ViewportSize);
    }

    WidgetAppearDistance = (ViewportSize.X / 2.f);

    if (StratagemIconList.IsEmpty() || RemainImageList.IsEmpty() || CommandIconList.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("StratagemIconList or RemainImageList or CommandIconList is Empty!"));
        return;
    }

    NULL_CHECK(StratagemDataTable);

    const TArray<FName>& StratagemDataNameList = StratagemDataTable->GetRowNames();
    CONDITION_CHECK(StratagemDataNameList.IsEmpty());

    const FString FindString(TEXT("Lookup"));
    const int32 StratagemDataNameListNum = StratagemDataNameList.Num();

    NULL_CHECK(VB_StratagemList);
    NULL_CHECK(StratagemInfoWidgetClass);

    for (int32 Index = 0; Index < StratagemDataNameListNum; ++Index)
    {
        FHDStratagemData* StratagemData = StratagemDataTable->FindRow<FHDStratagemData>(StratagemDataNameList[Index], FindString);
        NULL_CHECK(StratagemData);

        UHDStratagemInfoUserWidget* StratagemIndoWidget = CreateWidget<UHDStratagemInfoUserWidget>(this, StratagemInfoWidgetClass, StratagemDataNameList[Index]);
        NULL_CHECK(StratagemIndoWidget);
        StratagemIndoWidget->InitializeStratagemInfoWidget(StratagemIconList[StratagemDataNameList[Index]], StratagemData->CommandSequence, CommandIconList);
        StratagemInfoWidgetList.Add(StratagemIndoWidget);

        VB_StratagemList->AddChildToVerticalBox(StratagemIndoWidget);
    }

    FVector2D CurrentPos = GetRenderTransform().Translation;
    CurrentPos.X -= WidgetAppearDistance;
    SetRenderTranslation(CurrentPos);
}

void UHDStratagemHUDUserWidget::SetHUDActiveByCurrentInputMatchList(const TArray<FName>& MatchStratagemList, const int32 CurrentInputNum)
{
    for(UHDStratagemInfoUserWidget* StratagemInfoWidget : StratagemInfoWidgetList)
    {
        if(MatchStratagemList.Find(StratagemInfoWidget->GetFName()) != INDEX_NONE)
        {
            StratagemInfoWidget->ActiveCommandIconByNum(CurrentInputNum);
        }
        else
        {
            StratagemInfoWidget->SetAllWidgetCapacity(0.5f);
        }
    }
}

void UHDStratagemHUDUserWidget::SetAllWidgetOpacity(const float Opacity)
{
    for(UHDStratagemInfoUserWidget* StratagemInfoWidget : StratagemInfoWidgetList)
    {
        StratagemInfoWidget->SetAllWidgetCapacity(Opacity);
    }
}

void UHDStratagemHUDUserWidget::WidgetAppear(const bool bAppear)
{
    MoveState = bAppear ? EHDStratagemWidgetMoveState::APPEAR : EHDStratagemWidgetMoveState::DISAPPEAR;

    SetAllWidgetOpacity(bAppear ? 1.f : 0.5f);
}

void UHDStratagemHUDUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if(MoveState != EHDStratagemWidgetMoveState::NONE)
    {
        const FVector2D CurrentPos = GetRenderTransform().Translation;
        const FVector2D DesiredPos = MoveState == EHDStratagemWidgetMoveState::APPEAR ? FVector2D(0, 0) : FVector2D(-WidgetAppearDistance, 0);

        if (CurrentPos == DesiredPos)
        {
            MoveState = EHDStratagemWidgetMoveState::NONE;
            return;
        }

        const FVector2D NewPos = FMath::Vector2DInterpTo(CurrentPos, DesiredPos, InDeltaTime, 20.f);
        SetRenderTranslation(NewPos);
    }   
}
