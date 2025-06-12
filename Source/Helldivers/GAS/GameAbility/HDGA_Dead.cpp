// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/GameAbility/HDGA_Dead.h"
#include "Attribute/HDHealthAttributeSet.h"

UHDGA_Dead::UHDGA_Dead()
{
}

void UHDGA_Dead::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    UAbilityTask_WaitAttributeChange* WaitHealthChangeTask = UAbilityTask_WaitAttributeChange::WaitForAttributeChange(this, 
        FGameplayAttribute(UHDHealthAttributeSet::GetCurrentHealthAttribute()),
        FGameplayTag::EmptyTag, FGameplayTag::EmptyTag);
    WaitHealthChangeTask->OnChange.AddDynamic(this, &UHDGA_Dead::OnHealthChanged);
    //WaitHealthChangeTask->ReadyForActivation();
}

void UHDGA_Dead::OnHealthChanged(FOnAttributeChangeData Data)
{
    if (Data.NewValue <= 0.f)
    {
        //GetAbilitySystemComponentFromActorInfo()->TryActivateAbility(FGameplayTag::EmptyTag);

        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
    }
}
