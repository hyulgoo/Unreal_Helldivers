// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/HDCharacterPlayer.h"
#include "AbilitySystemInterface.h"
#include "GameAbility/Struct/HDTaggedInputAction.h"
#include "HDGASCharacterPlayer.generated.h"

class UAbilitySystemComponent;
class UEnhancedInputComponent;
class UGameplayAbility;
class UGameplayEffect;
struct FGameplayTag;
struct FGameplayEventData;

USTRUCT(BlueprintType)
struct FTagEventBindInfo
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly)
    FName						BindFunctionName;

    UPROPERTY(EditDefaultsOnly)
    TObjectPtr<UInputAction>	InputAction;

    UPROPERTY(EditDefaultsOnly)
    FGameplayTag				EventConditionTag;
};

UCLASS()
class HELLDIVERS_API AHDGASCharacterPlayer : public AHDCharacterPlayer, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	explicit AHDGASCharacterPlayer();

public:
    virtual UAbilitySystemComponent*			GetAbilitySystemComponent() const override final;

protected:
	virtual void								PossessedBy(AController* NewController) override final;
    virtual void								SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override final;
	void		                                CreateGASWidget(AController* PlayerController);
    void										SetupGASInputComponent(UEnhancedInputComponent* EnhancedInputComponent);
	void										SetGASEventInputComponent(UEnhancedInputComponent* EnhancedInputComponent);
    void										GASInputPressed(const FGameplayTag Tag);
    void										GASInputReleased(const FGameplayTag Tag);

    UFUNCTION()
	void										InputStratagemCommand(const FInputActionValue& Value);

	void										HandleGameplayEvent(const FGameplayEventData* Payload);

private:
	void										SetStratagemHUDAppear(const bool bAppear);

private:
	UPROPERTY(EditAnywhere, Category = GAS)
    TObjectPtr<UAbilitySystemComponent>			AbilitySystemComponent;
	
	UPROPERTY(EditAnywhere, Category = GAS)
	TSubclassOf<UGameplayEffect>				InitStatEffect;
	
	UPROPERTY(EditAnywhere, Category = GAS)
	TArray<TSubclassOf<UGameplayAbility>>		StartAbilities;
	
	UPROPERTY(EditAnywhere, Category = GAS)
	TArray<FTaggedInputAction>					TaggedInputActions;
	
	UPROPERTY(EditAnywhere, Category = GAS)
	FGameplayTagContainer						EventCallTags;
		
	UPROPERTY(EditAnywhere, Category = GAS)
	TArray<FTagEventBindInfo>					TagEventBindInfoList;
};
