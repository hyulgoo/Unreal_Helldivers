// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "HDPlayerController.generated.h"

class UHDGASPlayerUserWidget;
class UHDStratagemHUDUserWidget;
class UAbilitySystemComponent;
class ACharacter;

UCLASS()
class HELLDIVERS_API AHDPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	explicit								AHDPlayerController();
	void									CreateHUDWidget(APawn* aPawn);

	void									ChangeAmmoHUDInfo(const int32 NewAmmoCount);
	void									ChangeCapacityHUDInfo(const int32 NewCapacityCount);

	void									SetHUDActiveByCurrentInputMatchList(const TArray<FName>& MatchStratagemList, const int32 CurrentInputNum);
		
protected:
    virtual void							BeginPlay() override final;
	virtual void							OnPossess(APawn* Pawn) override final;

private:
	void									SetPossessAbilitySystemComponentBindEventCall(UAbilitySystemComponent* ASC);
	void									OnPlayerHUDInfoChanged(const FGameplayEventData* Payload);
	void									OnStratagemHUDInfoChanged(const FGameplayEventData* Payload);
	void									StratagemHUDAppear(const FGameplayEventData* Payload);

private:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TObjectPtr<UAbilitySystemComponent>		PossessPawnASC;

	// Widget Section
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UHDGASPlayerUserWidget>		PlayerHUDWidgetClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TObjectPtr<UHDGASPlayerUserWidget>		PlayerHUDWidget;
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UHDStratagemHUDUserWidget>	StratagemHUDWidgetClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TObjectPtr<UHDStratagemHUDUserWidget>	StratagemHUDWidget;
};
