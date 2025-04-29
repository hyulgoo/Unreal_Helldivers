// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "HDGameplayAbility.generated.h"

UCLASS()
class HELLDIVERS_API UHDGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	explicit UHDGameplayAbility() = default;
	const FGameplayTagContainer& GetAssetInputTags() const;

private:
	UPROPERTY(EditDefaultsOnly, Category = Tags, DisplayName="AssetTags (Input AbilityTags)", meta=(Categories="AbilityTagCategory"))
	FGameplayTagContainer AbilityInputTags;
};
