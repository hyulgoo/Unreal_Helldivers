// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "HDAbilitySystemComponent.generated.h"

struct FHDCharacterAttributeData;
enum class EHDCharacterType : uint8;

UCLASS()
class HELLDIVERS_API UHDAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
public:
    explicit    UHDAbilitySystemComponent();

    void	    AbilityInputTagTriggered(const FGameplayTag Tag);
    void	    AbilityInputTagReleased(const FGameplayTag Tag);
    void	    AbilityInputTagToggled(const FGameplayTag Tag);

    void	    InitAttributeSet();
    void        SetAttributeSetStat(FHDCharacterAttributeData* StatData, TSubclassOf<UGameplayEffect> InitAttributeStatEffect);
};
