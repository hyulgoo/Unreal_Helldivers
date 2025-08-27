// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "HDAT_PlayMontageAndWaitForEvent.generated.h"

class UAnimMontage;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayMontageAndWaitGameplayEventDelegate, FGameplayEventData, Payload);

UCLASS()
class HELLDIVERS_API UHDAT_PlayMontageAndWaitForEvent : public UAbilityTask
{
	GENERATED_BODY()
	
public:
    virtual void                                Activate() override;
    virtual void                                OnDestroy(bool AbilityEnded) override;

	UFUNCTION(BlueprintCallable)
    static UHDAT_PlayMontageAndWaitForEvent*    PlayMontageAndWaitEvent(UGameplayAbility* OwningAbility, UAnimMontage* MontageToPlay, const FGameplayTagContainer EventTagContainer, float Rate = 1.f, FName StartSection = NAME_None, const bool EndTaskOnMontageEnded = true, const bool OnlyTriggerOnce = false, bool OnlyMatchExact = true);

	void                                        OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);
    virtual void                                GameplayEventCallback(const FGameplayEventData* Payload);
    virtual void                                GameplayEventContainerCallback(FGameplayTag MatchingTag, const FGameplayEventData* Payload);

    void                                        JumpToSection(const FName SectionName);
    void                                        ResetMontage();
    bool                                        StopPlayingMontage();

public:
	UPROPERTY(BlueprintAssignable)
    FPlayMontageAndWaitGameplayEventDelegate	OnCompleted;

	UPROPERTY(BlueprintAssignable)
    FPlayMontageAndWaitGameplayEventDelegate	OnEventReceived;

protected:
    FDelegateHandle                             EventHandle;
    FOnMontageEnded                             MontageEndedDelegate;

	UPROPERTY()
	TObjectPtr<UAnimMontage>                    MontageToPlay;

	UPROPERTY()
	float                                       Rate;

	UPROPERTY()
	FName                                       StartSection;

	UPROPERTY()
	float                                       AnimRootMotionTranslationScale;

	UPROPERTY()
	uint8                                       bStopWhenAbilityEnds : 1;

	UPROPERTY()
	uint8                                       bAllowInterruptAfterBlendOut : 1;

    uint8                                       bOnlyTriggerOnce : 1 = false;
    uint8                                       bOnlyMatchExact : 1 = false;
    uint8                                       bEndTaskOnMontageEnded : 1 = false;

    FGameplayTagContainer                       EventTags;
};
