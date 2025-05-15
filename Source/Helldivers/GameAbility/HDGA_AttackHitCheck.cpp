// Fill out your copyright notice in the Description page of Project Settings.

#include "GameAbility/HDGA_AttackHitCheck.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameAbility/AbilityTask/HDAT_Trace.h"
#include "GameAbility/TargetActor/HDTA_Trace.h"
#include "Attribute/HDHealthAttributeSet.h"
#include "Define/HDDefine.h"

UHDGA_AttackHitCheck::UHDGA_AttackHitCheck()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UHDGA_AttackHitCheck::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UHDAT_Trace* AttackTraceTask = UHDAT_Trace::CreateTask(this, TargetActorClass);
	AttackTraceTask->OnComplete.AddDynamic(this, &UHDGA_AttackHitCheck::OnTraceResultCallback);
	AttackTraceTask->ReadyForActivation();
}

void UHDGA_AttackHitCheck::OnTraceResultCallback(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	if (UAbilitySystemBlueprintLibrary::TargetDataHasHitResult(TargetDataHandle, 0))
	{
		//UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
		//NULL_CHECK(SourceASC);
		//
        //AHDProjectile* SourceProjectile = Cast<AHDProjectile>(SourceASC->GetOwner());
		//NULL_CHECK(SourceProjectile);
		//
		//FHitResult HitResult = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetDataHandle, 0);
		//UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitResult.GetActor());
		//NULL_CHECK(TargetASC);
		//
		//UHDCharacterPlayerSpeedAttributeSet* TargetAttribute = const_cast<UHDCharacterPlayerSpeedAttributeSet*>(TargetASC->GetSet<UHDCharacterPlayerSpeedAttributeSet>());
        //NULL_CHECK(TargetAttribute);
		//
		//const float ProjectileDamage = SourceProjectile->Damage;
		//TargetAttribute->SetCurrentHealth(TargetAttribute->GetCurrentHealth() - ProjectileDamage);
	}

	const bool bReplicatedEndAbility = true;
	const bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}
