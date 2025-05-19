// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HDStratagemInfoUserWidget.generated.h"

class UImage;
class UTextBlock;
class UHorizontalBox;
class UHDCommandIcon;
enum class EHDCommandInput : uint8;

UCLASS()
class HELLDIVERS_API UHDStratagemInfoUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void								InitializeStratagemInfoWidget(UTexture2D* IconTexture, const TArray<EHDCommandInput>& CommandList, const TMap<EHDCommandInput, UTexture2D*> CommandIconList);
	void								ActiveCommandIconByNum(const int32 InputNum);
	void								SetAllWidgetCapacity(const float Opacity);

protected:
    UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage>					Img_StratagemIcon;

    UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock>				TB_StratagemName;
	
    UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox>			HB_StratagemCommand;
	
    UPROPERTY(EditAnywhere)
	TSubclassOf<UHDCommandIcon>			CommandIconWidgetClass;
	
    UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UHDCommandIcon>>	CommandIconWidgetList;
};
