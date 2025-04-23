// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/HDStratagemInfoUserWidget.h"
#include "Define/HDDefine.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/HorizontalBox.h"
#include "UI/HDCommandIcon.h"

void UHDStratagemInfoUserWidget::InitializeStratagemInfoWidget(UTexture2D* IconTexture, const TArray<EHDCommandInput>& CommandList, const TMap<EHDCommandInput, UTexture2D*> CommandIconList)
{
    NULL_CHECK(Img_StratagemIcon);
    Img_StratagemIcon->SetBrushFromTexture(IconTexture);

    NULL_CHECK(TB_StratagemName);
    FString WidgetName = GetName();
    if(WidgetName.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("HDStratagemInfoUserWidget Name is Empty!"));
        return;
    }

    TB_StratagemName->SetText(FText::FromString(WidgetName));

    NULL_CHECK(HB_StratagemCommand);
    NULL_CHECK(CommandIconWidgetClass);

    for(const EHDCommandInput CommandInput : CommandList)
    {
        UHDCommandIcon* CommandIconWidget = CreateWidget<UHDCommandIcon>(this, CommandIconWidgetClass);
        NULL_CHECK(CommandIconWidget);
        CommandIconWidget->SetCommandIcon(CommandIconList[CommandInput]);
        CommandIconWidgetList.Add(CommandIconWidget);

        HB_StratagemCommand->AddChildToHorizontalBox(CommandIconWidget);
    }
}

void UHDStratagemInfoUserWidget::ActiveCommandIconByNum(const int32 InputNum)
{
    const int32 CommandIconWidgetListNum = CommandIconWidgetList.Num();
    if(InputNum > CommandIconWidgetListNum)
    {
        UE_LOG(LogTemp, Error, TEXT("InputNum is Exceed StratagemCommandNum!"));
        return;
    }

    NULL_CHECK(Img_StratagemIcon);
    Img_StratagemIcon->SetRenderOpacity(1.f);

    for(int32 Index = 0; Index < CommandIconWidgetListNum; ++Index)
    {
        CommandIconWidgetList[Index]->SetRenderOpacity(Index < InputNum ? 0.5f : 1.f);
    }
}

void UHDStratagemInfoUserWidget::SetAllWidgetCapacity(const float Opacity)
{
    Img_StratagemIcon->SetRenderOpacity(Opacity);
    TB_StratagemName->SetRenderOpacity(Opacity);
    const int32 CommandIconWidgetListNum = CommandIconWidgetList.Num(); 
    for (int32 Index = 0; Index < CommandIconWidgetListNum; ++Index)
    {
        CommandIconWidgetList[Index]->SetRenderOpacity(Opacity);
    }
}
