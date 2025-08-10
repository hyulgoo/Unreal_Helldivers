// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayEffectTypes.h"
#include "HDGASPlayerUserWidget.generated.h"

class UProgressBar;
class UImage;
class UTextBlock;
struct FGameplayEventData;

UCLASS()
class HELLDIVERS_API UHDGASPlayerUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    void                        OnHealthAttributeChangeds(const FOnAttributeChangeData& ChangeData);
    void                        OnWeaponInfoChanged(const FGameplayEventData* Payload);
private:

    void                        SetChangedWeaponAmmoCountInfo(const int32 NewAmmoCount, const int32 NewMaxAmmoCount);
    void                        SetChangedWeaponCapacityCountInfo(const int32 NewCapacityCount, const int32 NewMaxCapacityCount);
    void                        SetGrenadeCountInfo(const int32 NewGrenadeCount, const int32 NewMaxGrenadeCount);
    void                        OnCapacityCountChanged(const int32 NewCapacityCount);
    void                        OnGrenadeCountChanged(const int32 NewGrenadeCount);

    void                        UpdateProgressbar(UProgressBar* Progressbar, const float Value);
    void                        UpdateTextblock(UTextBlock* TextBlock, const FText& Text);
    void                        UpdateImage(UImage* ImageWidget, UTexture2D* Texture);

protected:
    // HP Section
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage>          Img_HPbarBackGround;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UProgressBar>    Pb_HPbar;
    
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage>          Img_Steam;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock>      Text_SteamCount;

    float                       CurrentHealth = 0.f;
    float                       CurrentMaxHealth = 0.f;

    // Equip Section
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage>          Img_EquipBackground;
    
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage>          Img_Grenade;
    
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock>      Text_GrenadeCount;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage>          Img_Backpack;
    
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UProgressBar>    Pb_Backpack;
    
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UProgressBar>    Pb_Ammo;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock>      Text_CapacityCount;
    
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage>          Img_Weapon;

    int32                       MaxAmmoCount = 1;
    int32                       MaxCapacityCount = 1;
    int32                       MaxGrenadeCount = 1;
};
