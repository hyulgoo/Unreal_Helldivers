// Fill out your copyright notice in the Description page of Project Settings.

#include "HDGA_MovementTrigger.h"
#include "Abilities/Tasks/AbilityTask_WaitAttributeChangeThreshold.h"
#include "Interface/HDCharacterMovementInterface.h"
#include "Attribute/HDSpeedAttributeSet.h"
#include "Character/CharacterTypes/HDCharacterStateTypes.h"
#include "AbilitySystem/GameplayAbilityHelper.h"

UHDGA_MovementTrigger::UHDGA_MovementTrigger(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UHDGA_MovementTrigger::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    CONDITION_CHECK(Super::CommitAbility(Handle, ActorInfo, ActivationInfo))

	FGameplayTagContainer CurrentTagContainer = GetAssetTags();
	CONDITION_CHECK(CurrentTagContainer.IsValid());

	TScriptInterface<IHDCharacterMovementInterface> CharacterMovementInterface = ActorInfo->AvatarActor.Get();
	NULL_CHECK(CharacterMovementInterface);

	if (CurrentTagContainer.HasTagExact(Tag_Trigger_Aiming))
	{
	   //CharacterMovementInterface->ChangeCharacterControlType();
	}
	else if (CurrentTagContainer.HasTagExact(Tag_Trigger_Shoulder))
	{
		CharacterMovementInterface->SetShouldering(true);
	}
	else if (CurrentTagContainer.HasTagExact(Tag_Trigger_Sprint))
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
	else if (CurrentTagContainer.HasTagExact(Tag_Trigger_Crouch))
	{
		CharacterMovementInterface->SetCharacterStanceState(EHDCharacterStanceState::Crouch);
	}
	else if (CurrentTagContainer.HasTagExact(Tag_Trigger_Prone))
	{
		CharacterMovementInterface->SetCharacterStanceState(EHDCharacterStanceState::Prone);
	}
    else if(CurrentTagContainer.HasTagExact(Tag_Trigger_RegenStamina))
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

	if (CurrentTagContainer.HasTagExact(Tag_Trigger_Shoulder))
	{
		CharacterMovementInterface->SetShouldering(false);
	}
	else if (CurrentTagContainer.HasTagExact(Tag_Trigger_Sprint))
	{
		CharacterMovementInterface->SetMovementState(EHDCharacterMovementState::Walk);

        UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
        NULL_CHECK(ASC);

        FGameplayAbilityHelper::RemoveActiveEffectByGrantedTag(Tag_Cost_Stamina, ASC);

        GetWorld()->GetTimerManager().SetTimer(RegenStaminaHandle, FTimerDelegate::CreateLambda([this, ASC]() {
            FGameplayAbilityHelper::SendGameplayEventToSelf(Tag_Trigger_RegenStamina, ASC);}), 1.f, false);
	}
	else if (CurrentTagContainer.HasTagExact(Tag_Trigger_Crouch) || CurrentTagContainer.HasTagExact(Tag_Trigger_Prone))
	{
		CharacterMovementInterface->RestoreStanceState();
	}
    else if(CurrentTagContainer.HasTagExact(Tag_Trigger_RegenStamina))
    {
        UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
        NULL_CHECK(ASC);

        FGameplayAbilityHelper::RemoveActiveEffectByGrantedTag(Tag_Regen_Stamina, ASC);
    }
}

void UHDGA_MovementTrigger::OnCurrentStaminaChanged(bool bMatchesComparison, float CurrentValue)
{
    if (bMatchesComparison)
    {
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
    }
}
