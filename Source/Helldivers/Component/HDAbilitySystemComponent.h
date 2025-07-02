// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "HDAbilitySystemComponent.generated.h"

struct FHDCharacterStat;
enum class EHDCharacterPoseState : uint8;
enum class EHDCharacterMovementState : uint8;

UCLASS()
class HELLDIVERS_API UHDAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
public:
    explicit	                    UHDAbilitySystemComponent();

    void		                    AbilityInputTagTriggered(const FGameplayTag Tag);
    void		                    AbilityInputTagReleased(const FGameplayTag Tag);
    void		                    AbilityInputTagToggled(const FGameplayTag Tag);

    void                            SetAttributeSetStat(const FHDCharacterStat* StatData);

    const float                     GetSpeedAttribute(const EHDCharacterPoseState PoseState, const EHDCharacterMovementState MoveState);
private:
    void		                    InitAttributeSet();

private:
	TSubclassOf<UGameplayEffect>    InitAttributeStatEffect;
};
