// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GameAbility/HDGA_Base.h"
#include "HDGA_Fire.generated.h"

class IHDWeaponInterface;
class UHDAT_PlayMontageAndWaitForEvent;

UCLASS()
class HELLDIVERS_API UHDGA_Fire : public UHDGA_Base
{
	GENERATED_BODY()

public:
	 UHDGA_Fire(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    virtual bool                            CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void							ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override final;
	virtual void							EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override final;

private:
    UFUNCTION()
    void 								    OnEventRecieved(FGameplayEventData Payload);
    
    UFUNCTION()
    void 								    OnFireDelayCompleted();

protected:
    UPROPERTY()
    TObjectPtr<UHDAT_PlayMontageAndWaitForEvent> PlayMontageAndWaitEventTask;
    
    UPROPERTY()
	TScriptInterface<IHDWeaponInterface>	WeaponInterface;

    float                                   AutoFireDelay   = 0.f;
    FName                                   SectionName     = NAME_None;
};
