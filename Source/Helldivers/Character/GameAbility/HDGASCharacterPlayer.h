// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/HDCharacterPlayer.h"
#include "AbilitySystemInterface.h"
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
	void										GASInputPressed(const int32 InputId);
    void										GASInputReleased(const int32 InputId);

	void										InputStratagemCommand(const FInputActionValue& Value);
	void										HandleGameplayEvent(const FGameplayEventData* EventData);

private:
	UPROPERTY(EditAnywhere, Category = GAS)
    TObjectPtr<UAbilitySystemComponent>			AbilitySystemComponent;
	
	UPROPERTY(EditAnywhere, Category = GAS)
	TSubclassOf<UGameplayEffect>				InitStatEffect;
	
	UPROPERTY(EditAnywhere, Category = GAS)
	TArray<TSubclassOf<UGameplayAbility>>		StartAbilities;
	
	UPROPERTY(EditAnywhere, Category = GAS)
	TMap<int32, TSubclassOf<UGameplayAbility>>	StartInputAbilities;	
};
