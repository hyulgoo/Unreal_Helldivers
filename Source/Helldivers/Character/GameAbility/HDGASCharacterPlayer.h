// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/HDCharacterPlayer.h"
#include "AbilitySystemInterface.h"
#include "GameAbility/Struct/HDTaggedInputAction.h"
#include "HDGASCharacterPlayer.generated.h"

class UAbilitySystemComponent;
class UInputComponent;
class UGameplayAbility;
class UGameplayEffect;
struct FGameplayTag;
struct FGameplayEventData;

UCLASS()
class HELLDIVERS_API AHDGASCharacterPlayer : public AHDCharacterPlayer, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	explicit AHDGASCharacterPlayer();

public:
    virtual UAbilitySystemComponent*			GetAbilitySystemComponent() const override final;
    virtual void								PossessedBy(AController* NewController) override final;
	virtual void								SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override final;

protected:
    void										SetupGASInputComponent();
    void										GASInputPressed(FGameplayTag Tag);
    void										GASInputReleased(FGameplayTag Tag);

	void										InputStratagemCommand(const FInputActionValue& Value);
	void										HandleGameplayEvent(const FGameplayEventData* Payload);

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
	TArray<TSubclassOf<UInputAction>>			EventActions;
};
