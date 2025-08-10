// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HDReticleWidget.generated.h"

class UImage;
struct FHDWeaponCrosshairPair;

UCLASS()
class HELLDIVERS_API UHDReticleWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    void SetReticlaImage(const TArray<FHDWeaponCrosshairPair>& Images);
    void UpdateCrosshair(const float Spread);
    
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
