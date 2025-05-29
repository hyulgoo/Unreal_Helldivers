// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "HDAT_JumpAndWaitForLanding.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FJumpAndWaitForLandingDelegate);

UCLASS()
class HELLDIVERS_API UHDAT_JumpAndWaitForLanding : public UAbilityTask
{
    GENERATED_BODY()

public:
    explicit UHDAT_JumpAndWaitForLanding() = default;

    UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (DisplayName = "JumpAndWaitForLanding", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
    static UHDAT_JumpAndWaitForLanding* CreateTask(UGameplayAbility* OwningAbility);

    virtual void Activate() override;
    virtual void OnDestroy(bool AbilityEnded) override;

protected:
    UFUNCTION()
    void OnLandedCallback(const FHitResult& Hit);

public:
    UPROPERTY(BlueprintAssignable)
    FJumpAndWaitForLandingDelegate OnComplete;
};
