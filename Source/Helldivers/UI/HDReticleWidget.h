// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/HDGASUserWidget.h"
#include "HDReticleWidget.generated.h"

class UImage;

UCLASS()
class HELLDIVERS_API UHDReticleWidget : public UHDGASUserWidget
{
	GENERATED_BODY()

protected:
    virtual void        NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override final;

public:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage>  Img_Center;
    
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage>  Img_Left;
    
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage>  Img_Right;
    
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage>  Img_Top;
    
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage>  Img_Bottom;
};
