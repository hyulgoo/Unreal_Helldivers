// Fill out your copyright notice in the Description page of Project Settings.

#include "HDGA_MovementTrigger.h"
#include "Abilities/Tasks/AbilityTask_WaitAttributeChangeThreshold.h"
#include "Interface/HDCharacterMovementInterface.h"
#include "Attribute/HDSpeedAttributeSet.h"
#include "AbilitySystem/GameplayAbilityHelper.h"
#include "Character/CharacterTypes/HDCharacterStateTypes.h"

UHDGA_MovementTrigger::UHDGA_MovementTrigger()
    : MaxStamina(0.f)
    , RegenStaminaHandle(FTimerHandle())
{ 
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	ActivationBlockedTags.AddTag(HDTAG_CHARACTER_STATE_ISDEAD);
}

void UHDGA_MovementTrigger::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    CONDITION_CHECK(Super::CommitAbility(Handle, ActorInfo, ActivationInfo))

	FGameplayTagContainer CurrentTagContainer = GetAssetTags();
	CONDITION_CHECK(CurrentTagContainer.IsValid());

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

        UAbilityTask_WaitAttributeChangeThreshold* ThresholdTask = UAbilityTask_WaitAttributeChangeThreshold::WaitForAttributeChangeThreshold(
            this,
            UHDSpeedAttributeSet::GetCurrentStaminaAttribute(),
            EWaitAttributeChangeComparison::LessThanOrEqualTo,
            0.f,
            true
        );
        NULL_CHECK(ThresholdTask);

        ThresholdTask->OnChange.AddDynamic(this, &UHDGA_MovementTrigger::OnCurrentStaminaChanged);
        ThresholdTask->ReadyForActivation();

        if(RegenStaminaHandle.IsValid())
        {
            GetWorld()->GetTimerManager().ClearTimer(RegenStaminaHandle);
        }
	}
	else if (CurrentTagContainer.HasTagExact(HDTAG_INPUT_CROUCH))
	{
		CharacterMovementInterface->SetCharacterStanceState(EHDCharacterStanceState::Crouch);
	}
	else if (CurrentTagContainer.HasTagExact(HDTAG_INPUT_PRONE))
	{
		CharacterMovementInterface->SetCharacterStanceState(EHDCharacterStanceState::Prone);
	}
    else if(CurrentTagContainer.HasTagExact(HDTAG_REGEN_STAMINA))
    {
        UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
        NULL_CHECK(ASC);

        MaxStamina = ASC->GetNumericAttribute(UHDSpeedAttributeSet::GetMaxStaminaAttribute());
        UAbilityTask_WaitAttributeChangeThreshold* ThresholdTask = UAbilityTask_WaitAttributeChangeThreshold::WaitForAttributeChangeThreshold(
            this,
            UHDSpeedAttributeSet::GetCurrentStaminaAttribute(),
            EWaitAttributeChangeComparison::GreaterThanOrEqualTo,
            MaxStamina,
            true
        );

        ThresholdTask->OnChange.AddDynamic(this, &UHDGA_MovementTrigger::OnCurrentStaminaChanged);
        ThresholdTask->ReadyForActivation();
    }
}

void UHDGA_MovementTrigger::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);

	TScriptInterface<IHDCharacterMovementInterface> CharacterMovementInterface = ActorInfo->AvatarActor.Get();
	NULL_CHECK(CharacterMovementInterface);

	FGameplayTagContainer CurrentTagContainer = GetAssetTags();
	CONDITION_CHECK(CurrentTagContainer.IsValid());

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
        NULL_CHECK(ASC);

        FGameplayAbilityHelper::RemoveActiveEffectByGrantedTag(HDTAG_COST_STAMINA, ASC);

        GetWorld()->GetTimerManager().SetTimer(RegenStaminaHandle, FTimerDelegate::CreateLambda([this, ASC]() {
            FGameplayAbilityHelper::SendGameplayEventToSelf(HDTAG_REGEN_STAMINA, ASC);}), 1.f, false);
	}
	else if (CurrentTagContainer.HasTagExact(HDTAG_INPUT_CROUCH) || CurrentTagContainer.HasTagExact(HDTAG_INPUT_PRONE))
	{
		CharacterMovementInterface->RestoreStanceState();
	}
    else if(CurrentTagContainer.HasTagExact(HDTAG_REGEN_STAMINA))
    {
        UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
        NULL_CHECK(ASC);

        FGameplayAbilityHelper::RemoveActiveEffectByGrantedTag(HDTAG_REGEN_STAMINA, ASC);
    }
}

void UHDGA_MovementTrigger::OnCurrentStaminaChanged(bool bMatchesComparison, float CurrentValue)
{
    if (bMatchesComparison)
    {
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
    }
}
