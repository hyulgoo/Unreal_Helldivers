// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameAbility/HDGameplayAbility.h"
#include "HDGA_Sprint.generated.h"

UCLASS()
class HELLDIVERS_API UHDGA_Sprint : public UHDGameplayAbility
{
	GENERATED_BODY()
	
public:
	explicit UHDGA_Sprint();

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override final;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override final;
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override final;

private:


protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sprint")
	float StaminaDrainPerSecounds;
};
