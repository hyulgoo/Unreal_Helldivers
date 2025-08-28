// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GameAbility/HDGA_Base.h"
#include "HDGA_Reload.generated.h"

class UGameplayEffect;

UCLASS()
class HELLDIVERS_API UHDGA_Reload : public UHDGA_Base
{
	GENERATED_BODY()
	
public:
    virtual bool                    CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
    virtual void                    ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override final;
    virtual void                    EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override final;
    
private:
    UFUNCTION()
    void 	                        OnEventRecieved(FGameplayEventData Payload);

private:
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UGameplayEffect>    ReloadEffectClass; 
};
