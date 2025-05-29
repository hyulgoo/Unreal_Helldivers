// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/AbilityTask/HDAT_Trace.h"
#include "GAS/GameplayAbilityTargetActor/HDTA_Trace.h"
#include "AbilitySystemComponent.h"

UHDAT_Trace* UHDAT_Trace::CreateTask(UGameplayAbility* OwningAbility, TSubclassOf<AHDTA_Trace> TargetActor)
{
	UHDAT_Trace* NewTask = NewAbilityTask<UHDAT_Trace>(OwningAbility);
	NewTask->TargetActorClass = TargetActor;
	return NewTask;
}

void UHDAT_Trace::Activate()
{
	Super::Activate();

	SpawnAndInitializeTargetActor();
	FinalizeTargetActor();

	SetWaitingOnAvatar();
}

void UHDAT_Trace::OnDestroy(bool AbilityEnded)
{
	if (SpawnedTargetActor)
	{
		SpawnedTargetActor->Destroy();
	}

	Super::OnDestroy(AbilityEnded);
}

void UHDAT_Trace::SpawnAndInitializeTargetActor()
{
	SpawnedTargetActor = Cast<AHDTA_Trace>(Ability->GetWorld()->SpawnActorDeferred<AGameplayAbilityTargetActor>(TargetActorClass, FTransform::Identity, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn));
	if (SpawnedTargetActor)
	{
		SpawnedTargetActor->SetShowDebug(true);
		SpawnedTargetActor->TargetDataReadyDelegate.AddUObject(this, &UHDAT_Trace::OnTargetDataReadyCallback);
	}
}

void UHDAT_Trace::FinalizeTargetActor()
{
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (ASC)
	{
		const FTransform SpawnTransform = ASC->GetAvatarActor()->GetTransform();
		SpawnedTargetActor->FinishSpawning(SpawnTransform);

		ASC->SpawnedTargetActors.Push(SpawnedTargetActor);
		SpawnedTargetActor->StartTargeting(Ability);
		SpawnedTargetActor->ConfirmTargeting();
	}
}

void UHDAT_Trace::OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& DataHandle)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnComplete.Broadcast(DataHandle);
	}

	EndTask();
}
