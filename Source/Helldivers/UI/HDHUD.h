// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "HDHUD.generated.h"

class UAbilitySystemComponent;
class UHDGASPlayerUserWidget;
class UHDReticleWidget;
class UHDStratagemHUDUserWidget;
class AHDWeapon;
struct FOnAttributeChangeData;
struct FGameplayEventData;

UCLASS()
class HELLDIVERS_API AHDHUD : public AHUD
{
	GENERATED_BODY()

public:
    void                                SetAbilitySystemComponent(UAbilitySystemComponent* ASC);
    void                                CreateDefaultWidget();
    void                                OnEquipWeaponUIEventRecieved(const FGameplayEventData* Payload);

private:
    UPROPERTY()
    TObjectPtr<UAbilitySystemComponent>     AbilitySystemComponent;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UHDGASPlayerUserWidget>	    PlayerHUDWidgetClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UHDReticleWidget>		    ReticleWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UHDStratagemHUDUserWidget>	StratagemWidgetClass;
	
	UPROPERTY()
	TObjectPtr<UHDGASPlayerUserWidget>	    PlayerHUDWidget;
    	
	UPROPERTY()
	TObjectPtr<UHDReticleWidget>		    ReticleWidget;
    
	UPROPERTY()
	TObjectPtr<UHDStratagemHUDUserWidget>	StratagemWidget;

	UPROPERTY()
    TObjectPtr<AHDWeapon>                   CurrentWeapon;

    FDelegateHandle                         SpreadChangedHandle;  
};
