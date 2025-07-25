// Fill out your copyright notice in the Description page of Project Settings.

#include "HDGA_Dead.h"
#include "Interface/HDDeadInterface.h"

void UHDGA_Dead::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    TScriptInterface<IHDDeadInterface> DeadInterface = ActorInfo->AvatarActor.Get();
    NULL_CHECK(DeadInterface);

    DeadInterface->SetDead();
}
