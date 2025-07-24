// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "HDPlayerController.generated.h"

class ACharacter;
class UHDInputData;
class UHDAbilitySystemComponent;
class UHDGASPlayerUserWidget;
class UHDStratagemHUDUserWidget;
class UHDCharacterControlData;
struct FInputActionValue;
enum class EHDCharacterControlType : uint8;

UENUM(BlueprintType)
enum class EHDCharacterInputAction : uint8
{
	ThirdLook,
	ThirdMove,
	FirstLook,
	FirstMove,
	ChangeControl,
	Count
};

UCLASS()
class HELLDIVERS_API AHDPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	explicit								AHDPlayerController();
		
    UHDAbilitySystemComponent*              GetAbilitySystemComponent();

protected:
    virtual void							BeginPlay() override final;
	virtual void							OnPossess(APawn* Pawn) override final;
    virtual void                            OnUnPossess() override final;
    virtual void                            SetupInputComponent() override final;

private:
    void		                            AbilityInputTriggered(const FGameplayTag Tag);
    void		                            AbilityInputReleased(const FGameplayTag Tag);
    void		                            AbilityInputToggled(const FGameplayTag Tag);

    void                                    ChangeCharacterControlType();
    void                                    SetCharacterControl(const EHDCharacterControlType NewCharacterControlType);


    void									CreateHUDWidget(APawn* aPawn);
	void									SetPossessAbilitySystemComponentBindEventCall(UHDAbilitySystemComponent* ASC);
    void                                    OnPlayerHUDChanged(const FGameplayEventData* Payload);
    void                                    OnStratagemHUDChanged(const FGameplayEventData* Payload);

    void									Look(const FInputActionValue& Value);
    void									Move(const FInputActionValue& Value);

private:
	UPROPERTY()
	TObjectPtr<UHDAbilitySystemComponent>   AbilitySystemComponent;

	// Widget Section
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UHDGASPlayerUserWidget>		PlayerHUDWidgetClass;
	
	UPROPERTY()
	TObjectPtr<UHDGASPlayerUserWidget>		PlayerHUDWidget;
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UHDStratagemHUDUserWidget>	StratagemHUDWidgetClass;
	
	UPROPERTY()
	TObjectPtr<UHDStratagemHUDUserWidget>	StratagemHUDWidget;

    //Input
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UHDInputData>			    AbilityInputData;
    
    EHDCharacterControlType			        CurrentCharacterControlType;

	UPROPERTY(EditDefaultsOnly)
	TMap<EHDCharacterInputAction, TObjectPtr<UInputAction>> InputActionMap;

    UPROPERTY(EditAnywhere)
    TMap<EHDCharacterControlType, TObjectPtr<UHDCharacterControlData>> CharacterControlDataMap;	
};
