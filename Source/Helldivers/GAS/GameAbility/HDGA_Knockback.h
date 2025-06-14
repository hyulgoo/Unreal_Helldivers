// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "HDGA_Knockback.generated.h"

class IHDCharacterRagdollInterface;

UCLASS()
class HELLDIVERS_API UHDGA_Knockback : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	explicit UHDGA_Knockback();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override final;

private:
	void		 CheckCharacterRagdollState();
	void		 RecoveryFromRagdoll();

private:
	UPROPERTY()
	TScriptInterface<IHDCharacterRagdollInterface>	RagdollInterface;
	
	UPROPERTY()
	TObjectPtr<UWorld>								World;

	bool											bRecoveryFromRagdoll;
	FTimerHandle									StateCheckTimerHandle;
	FTimerHandle									RecoveryFromRagdollTimerHandle;
};
