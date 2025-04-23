// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HDCommandIcon.generated.h"

class UScaleBox;
class UImage;

UCLASS()
class HELLDIVERS_API UHDCommandIcon : public UUserWidget
{
	GENERATED_BODY()

public:
    void SetCommandIcon(UTexture2D* IconTexture);
	
protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UScaleBox> SB_Command;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage>    Img_CommandIcon;
};
