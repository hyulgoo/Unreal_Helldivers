// Fill out your copyright notice in the Description page of Project Settings.

#include "HDGA_MovementTrigger.h"
#include "Define/HDGameplayTag.h"
#include "Define/HDDefine.h"
#include "Interface/HDCharacterMovementInterface.h"
#include "Abilities/Tasks/AbilityTask_WaitAttributeChangeThreshold.h"
#include "Attribute/HDSpeedAttributeSet.h"
#include "Character/CharacterTypes/HDCharacterStateTypes.h"

UHDGA_MovementTrigger::UHDGA_MovementTrigger()
{ 
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	ActivationBlockedTags.AddTag(HDTAG_CHARACTER_STATE_ISDEAD);
}

void UHDGA_MovementTrigger::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    if (Super::CommitAbility(Handle, ActorInfo, ActivationInfo) == false)
    {
        return;
    }

	FGameplayTagContainer CurrentTagContainer = GetAssetTags();
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
		CharacterMovementInterface->SetMovementState(EHDCharacterMovementState::Sprint);

        ThresholdTask = UAbilityTask_WaitAttributeChangeThreshold::WaitForAttributeChangeThreshold(
            this,
            UHDSpeedAttributeSet::GetCurrentStaminaAttribute(),
            EWaitAttributeChangeComparison::LessThanOrEqualTo,
            0.f,
            true
        );
        FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CostGameplayEffectClass);
        SpecHandle.Data->DynamicGrantedTags.AddTag(HDTAG_COST_STAMINA);
        
        GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
	else if (CurrentTagContainer.HasTagExact(HDTAG_INPUT_CROUCH))
	{
		CharacterMovementInterface->SetCharacterStanceState(EHDCharacterStanceState::Crouch);
	}
	else if (CurrentTagContainer.HasTagExact(HDTAG_INPUT_PRONE))
	{
		CharacterMovementInterface->SetCharacterStanceState(EHDCharacterStanceState::Prone);
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

	FGameplayTagContainer CurrentTagContainer = GetAssetTags();
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
		CharacterMovementInterface->SetMovementState(EHDCharacterMovementState::Walk);

        UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();

        FGameplayTagContainer GrantedTags(HDTAG_COST_STAMINA);
        FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(GrantedTags);

        ASC->RemoveActiveEffects(Query);
	}
	else if (CurrentTagContainer.HasTagExact(HDTAG_INPUT_CROUCH) || CurrentTagContainer.HasTagExact(HDTAG_INPUT_PRONE))
	{
		CharacterMovementInterface->RestoreStanceState();
	}
}

void UHDGA_MovementTrigger::OnCurrentStaminaChanged(bool bMatchesComparison, float CurrentValue)
{
    if (bMatchesComparison && CurrentValue <= 0.f)
    {
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
    }
}
