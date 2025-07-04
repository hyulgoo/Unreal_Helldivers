// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "HDAT_Trace.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTraceResultDelegate, const FGameplayAbilityTargetDataHandle&, TargetDataHandle);

class AHDTA_Trace;

UCLASS()
class HELLDIVERS_API UHDAT_Trace : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	explicit UHDAT_Trace() = default;

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (DisplayName = "WaitForTrace", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UHDAT_Trace* CreateTask(UGameplayAbility* OwningAbility, TSubclassOf<AHDTA_Trace> TargetActor);
	virtual void Activate() override;
	virtual void OnDestroy(bool AbilityEnded) override;

	void SpawnAndInitializeTargetActor();
	void FinalizeTargetActor();

protected:
	void OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& DataHandle);

public:
	UPROPERTY(BlueprintAssignable)
	FTraceResultDelegate OnComplete;

protected:
	UPROPERTY()
	TSubclassOf<AHDTA_Trace> TargetActorClass;

	UPROPERTY()
	TObjectPtr<AHDTA_Trace> SpawnedTargetActor;
};
