// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "HDPlayerController.generated.h"

class UHDGASPlayerUserWidget;
class UHDStratagemHUDUserWidget;
class UAbilitySystemComponent;
class AHDWeapon;
class ACharacter;

UCLASS()
class HELLDIVERS_API AHDPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	explicit								AHDPlayerController();
	void									SetPossessPawnAbilitySystemComponent(UAbilitySystemComponent* ASC);
	void									CreateHUDWidget(ACharacter* PlayerCharacter);
	void									SetWeaponHUDInfo(AHDWeapon* NewWeapon);

	void									ChangeAmmoHUDInfo(const int32 NewAmmoCount);
	void									ChangeCapacityHUDInfo(const int32 NewCapacityCount);

	void									SetHUDActiveByCurrentInputMatchList(const TArray<FName>& MatchStratagemList, const int32 CurrentInputNum);
	void									SetStratagemHUDAppear(const bool bAppear);
		
protected:
    virtual void							BeginPlay() override final;
	virtual void							OnPossess(APawn* Pawn) override;

private:
	void									OnPlayerHUDInfoChanged(const FGameplayEventData& Data);
	void									OnStratagemHUDInfoChanged(const FGameplayEventData& Data);

protected:
	UPROPERTY(EditAnywhere, Category = "UI")
	TObjectPtr<UAbilitySystemComponent>		PossessPawnAbilitySystemComponent;

	// Widget Section
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UHDGASPlayerUserWidget>		PlayerHUDWidgetClass;
	
	UPROPERTY(EditAnywhere, Category = "UI")
	TObjectPtr<UHDGASPlayerUserWidget>		PlayerHUDWidget;
	
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UHDStratagemHUDUserWidget>	StratagemHUDWidgetClass;
	
	UPROPERTY(EditAnywhere, Category = "UI")
	TObjectPtr<UHDStratagemHUDUserWidget>	StratagemHUDWidget;
};
