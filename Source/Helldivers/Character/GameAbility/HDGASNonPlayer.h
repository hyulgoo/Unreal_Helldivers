// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/HDCharacterNonPlayer.h"
#include "AbilitySystemInterface.h"
#include "HDGASNonPlayer.generated.h"

class UGameplayEffect;
class UGameplayAbility;

UCLASS()
class HELLDIVERS_API AHDGASNonPlayer : public AHDCharacterNonPlayer, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	explicit								AHDGASNonPlayer();

	virtual UAbilitySystemComponent*		GetAbilitySystemComponent() const override final;

protected:
	virtual void							BeginPlay() override final;

private:
	void									InitAbilitySystemComponent();
	void									InitializeAttributeSet();

protected:
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent>		AbilitySystemComponent;

	UPROPERTY(EditAnywhere, Category = "GASNonPlayer|Input")
	TArray<TSubclassOf<UGameplayAbility>>	StartAbilities;

	UPROPERTY(EditAnywhere, Category = "GASNonPlayer|Input")
	TArray<TSubclassOf<UGameplayAbility>>	PassiveAbilities;
};
