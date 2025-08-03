// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "HDAT_PlayMontageAndWaitForEvent.generated.h"

class UAnimMontage;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPlayMontageAndWaitGameplayEventDelegate, UAnimMontage*, Montage, FGameplayEventData, Payload);

UCLASS()
class HELLDIVERS_API UHDAT_PlayMontageAndWaitForEvent : public UAbilityTask
{
	GENERATED_BODY()
	
public:
    UHDAT_PlayMontageAndWaitForEvent(const FObjectInitializer& ObjectInitializer);

	UFUNCTION()
    static UHDAT_PlayMontageAndWaitForEvent*    PlayMontageAndWaitEvent(UGameplayAbility* OwningAbility, UAnimMontage* MontageToPlay, const FGameplayTagContainer EventTagContainer, float Rate = 1.f, FName StartSection = NAME_None, const bool OnlyTriggerOnce = false, bool OnlyMatchExact = true);

    UFUNCTION()
	void                                        OnMontageBlendedIn(UAnimMontage* Montage);

	UFUNCTION()
	void                                        OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void                                        OnGameplayAbilityCancelled();

	UFUNCTION()
	void                                        OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	virtual void                                ExternalCancel() override;

    virtual void                                Activate() override;

    virtual void                                GameplayEventCallback(const FGameplayEventData* Payload);
    virtual void                                GameplayEventContainerCallback(FGameplayTag MatchingTag, const FGameplayEventData* Payload);

protected:
    virtual void                                OnDestroy(bool AbilityEnded) override;

    bool                                        StopPlayingMontage();

public:
	UPROPERTY(BlueprintAssignable)
    FPlayMontageAndWaitGameplayEventDelegate	OnCompleted;

	UPROPERTY(BlueprintAssignable)
    FPlayMontageAndWaitGameplayEventDelegate	OnBlendedIn;

	UPROPERTY(BlueprintAssignable)
    FPlayMontageAndWaitGameplayEventDelegate	OnBlendOut;

	UPROPERTY(BlueprintAssignable)
    FPlayMontageAndWaitGameplayEventDelegate	OnCancelled;
    
	UPROPERTY(BlueprintAssignable)
    FPlayMontageAndWaitGameplayEventDelegate	OnEventReceived;

protected:
    FDelegateHandle                             EventHandle;
    FOnMontageBlendedInEnded                    BlendedInDelegate;
    FOnMontageBlendingOutStarted                BlendingOutDelegate;
    FOnMontageEnded                             MontageEndedDelegate;
    FDelegateHandle                             InterruptedHandle;

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

    uint8                                       bOnlyTriggerOnce : 1;
    uint8                                       bOnlyMatchExact : 1;

    FGameplayTagContainer                       EventTags;
};
