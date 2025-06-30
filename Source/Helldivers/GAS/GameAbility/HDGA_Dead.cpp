// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/GameAbility/HDGA_Dead.h"
#include "Interface/HDDeadInterface.h"

UHDGA_Dead::UHDGA_Dead()
{
}

void UHDGA_Dead::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    TScriptInterface<IHDDeadInterface> DeadInterface = ActorInfo->AvatarActor.Get();
    if (DeadInterface)
    {
        DeadInterface->SetDead();
    }
}
