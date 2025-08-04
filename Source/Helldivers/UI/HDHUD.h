// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "HDHUD.generated.h"

class UAbilitySystemComponent;
class UHDGASPlayerUserWidget;
class UHDReticleWidget;
struct FOnAttributeChangeData;

UCLASS()
class HELLDIVERS_API AHDHUD : public AHUD
{
	GENERATED_BODY()

public:
    void                                CreateDefaultWidget(UAbilitySystemComponent* ASC);

private:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UHDGASPlayerUserWidget>	PlayerHUDWidgetClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UHDReticleWidget>		ReticleWidgetClass;

	UPROPERTY()
	TObjectPtr<UHDGASPlayerUserWidget>	PlayerHUDWidget;
    	
	UPROPERTY()
	TObjectPtr<UHDReticleWidget>		ReticleWidget;
};
