// Fill out your copyright notice in the Description page of Project Settings.

#include "HDGA_MovementTrigger.h"
#include "Tag/HDGameplayTag.h"
#include "Define/HDDefine.h"
#include "Interface/HDCharacterMovementInterface.h"
#include "Character/CharacterTypes/HDCharacterStateTypes.h"

UHDGA_MovementTrigger::UHDGA_MovementTrigger()
{ 
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UHDGA_MovementTrigger::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	FGameplayTagContainer CurrentTagContainer = GetAssetInputTags();
	CONDITION_CHECK(CurrentTagContainer.IsValid() == false);

	TScriptInterface<IHDCharacterMovementInterface> CharacterMovementInterface = ActorInfo->AvatarActor.Get();
	NULL_CHECK(CharacterMovementInterface);

	if (CurrentTagContainer.HasTagExact(HDTAG_INPUT_AIMING))
	{
	   //CharacterMovementInterface->ChangeCharacterControlType();
	}
	else if (CurrentTagContainer.HasTagExact(HDTAG_INPUT_SHOULDER))
	{
		CharacterMovementInterface->SetShouldering(true);
	}
	else if (CurrentTagContainer.HasTagExact(HDTAG_INPUT_SPRINT))
	{
		CharacterMovementInterface->SetSprint(true);
	}
	else if (CurrentTagContainer.HasTagExact(HDTAG_INPUT_CROUCH))
	{
		CharacterMovementInterface->SetCharacterMovementState(EHDCharacterMovementState::Crouch);
	}
	else if (CurrentTagContainer.HasTagExact(HDTAG_INPUT_PRONE))
	{
		CharacterMovementInterface->SetCharacterMovementState(EHDCharacterMovementState::Prone);
	}
}

void UHDGA_MovementTrigger::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	const bool bReplicatedEndAbility = true;
	const bool bWasCancelled = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}

void UHDGA_MovementTrigger::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);

	TScriptInterface<IHDCharacterMovementInterface> CharacterMovementInterface = ActorInfo->AvatarActor.Get();
	NULL_CHECK(CharacterMovementInterface);

	FGameplayTagContainer CurrentTagContainer = GetAssetInputTags();
	CONDITION_CHECK(CurrentTagContainer.IsValid() == false);

	if (CurrentTagContainer.HasTagExact(HDTAG_INPUT_AIMING))
	{
		//CharacterMovementInterface->ChangeCharacterControlType();
	}
	else if (CurrentTagContainer.HasTagExact(HDTAG_INPUT_SHOULDER))
	{
		CharacterMovementInterface->SetShouldering(false);
	}
	else if (CurrentTagContainer.HasTagExact(HDTAG_INPUT_SPRINT))
	{
		CharacterMovementInterface->SetSprint(false);
	}
	else if (CurrentTagContainer.HasTagExact(HDTAG_INPUT_CROUCH) || CurrentTagContainer.HasTagExact(HDTAG_INPUT_PRONE))
	{
		CharacterMovementInterface->RestoreMovementState();
	}
}
