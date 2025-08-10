// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "HDPlayerController.generated.h"

class ACharacter;
class UInputAction;
class UHDInputData;
class UHDAbilitySystemComponent;
class UHDStratagemHUDUserWidget;
class UHDCharacterControlData;
struct FInputActionValue;

UENUM(BlueprintType)
enum class EHDCharacterInputAction : uint8
{
	ThirdLook,
	ThirdMove,
	FirstLook,
	FirstMove,
	Count
};

UCLASS()
class HELLDIVERS_API AHDPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
    AHDPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
		
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

    void									Look(const FInputActionValue& Value);
    void									Move(const FInputActionValue& Value);

private:
	UPROPERTY()
	TObjectPtr<UHDAbilitySystemComponent>   AbilitySystemComponent;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UHDInputData>			    AbilityInputData;
    
    EHDCharacterControlType			        CurrentCharacterControlType = EHDCharacterControlType::ThirdPerson;

	UPROPERTY(EditDefaultsOnly)
	TMap<EHDCharacterInputAction, TObjectPtr<UInputAction>> InputActionMap;

    UPROPERTY(EditAnywhere)
    TMap<EHDCharacterControlType, TObjectPtr<UHDCharacterControlData>> CharacterControlDataMap;	
};
