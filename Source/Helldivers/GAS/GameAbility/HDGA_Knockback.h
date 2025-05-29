// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "HDGA_Knockback.generated.h"

class IHDCharacterMovementInterface;

UCLASS()
class HELLDIVERS_API UHDGA_Knockback : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	explicit UHDGA_Knockback();
	
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override final;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override final;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override final;

private:
	void		 CheckCharacterRagdollState();
	void		 RecoveryFromRagdoll();

private:
	UPROPERTY()
	TScriptInterface<IHDCharacterMovementInterface>	MovementInterface;
	
	UPROPERTY()
	TObjectPtr<UWorld>								World;

	bool											bRecoveryFromRagdoll;
	FTimerHandle									StateCheckTimerHandle;
	FTimerHandle									RecoveryFromRagdollTimerHandle;
};
