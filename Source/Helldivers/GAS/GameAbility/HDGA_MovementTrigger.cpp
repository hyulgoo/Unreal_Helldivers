// Fill out your copyright notice in the Description page of Project Settings.

#include "HDGA_MovementTrigger.h"
#include "Tag/HDGameplayTag.h"
#include "Define/HDDefine.h"
#include "Interface/HDCharacterMovementInterface.h"

UHDGA_MovementTrigger::UHDGA_MovementTrigger()
	: CurrentTagContainer(FGameplayTagContainer())
{ 
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UHDGA_MovementTrigger::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UHDGA_MovementTrigger::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	CurrentTagContainer = GetAssetInputTags();
	CONDITION_CHECK(CurrentTagContainer.IsValid() == false);

	TScriptInterface<IHDCharacterMovementInterface> CharacterMovementInterface = ActorInfo->AvatarActor.Get();
	NULL_CHECK(CharacterMovementInterface);

	if (CurrentTagContainer.HasTagExact(HDTAG_INPUT_AIMING))
	{
	   //CharacterMovementInterface->ChangeCharacterControlType();
	}

	if (CurrentTagContainer.HasTagExact(HDTAG_INPUT_SHOULDER))
	{
		CharacterMovementInterface->SetShouldering(true);
	}

	if (CurrentTagContainer.HasTagExact(HDTAG_INPUT_SPRINT))
	{
		CharacterMovementInterface->SetSprint(true);
	}

	if (CurrentTagContainer.HasTagExact(HDTAG_INPUT_CROUCH))
	{
		if (CharacterMovementInterface->IsCrouch())
		{
			CharacterMovementInterface->SetCrouch(false);

			const bool bReplicatedEndAbility = true;
			const bool bWasCancelled = true;
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
		}
		else
		{
			CharacterMovementInterface->SetCrouch(true);
		}
	}

	if (CurrentTagContainer.HasTagExact(HDTAG_INPUT_PRONE))
	{
		if (CharacterMovementInterface->IsProne())
		{
			CharacterMovementInterface->SetProne(false);

			const bool bReplicatedEndAbility = true;
			const bool bWasCancelled = true;
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
		}
		else
		{
			CharacterMovementInterface->SetProne(true);
		}
	}
}

void UHDGA_MovementTrigger::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	TScriptInterface<IHDCharacterMovementInterface> CharacterMovementInterface = ActorInfo->AvatarActor.Get();
	NULL_CHECK(CharacterMovementInterface);

	if (CurrentTagContainer.HasTagExact(HDTAG_INPUT_AIMING))
	{
		//CharacterMovementInterface->ChangeCharacterControlType();
	}

	if (CurrentTagContainer.HasTagExact(HDTAG_INPUT_SHOULDER))
	{
		CharacterMovementInterface->SetShouldering(false);
	}

	if (CurrentTagContainer.HasTagExact(HDTAG_INPUT_SPRINT))
	{
		CharacterMovementInterface->SetSprint(false);
	}

	const bool bReplicatedEndAbility = true;
	const bool bWasCancelled = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}
