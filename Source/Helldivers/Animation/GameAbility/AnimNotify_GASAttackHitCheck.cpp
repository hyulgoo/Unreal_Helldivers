// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/GameAbility/AnimNotify_GASAttackHitCheck.h"
#include "AbilitySystemBlueprintLibrary.h"

FString UAnimNotify_GASAttackHitCheck::GetNotifyName_Implementation() const
{
	return TEXT("GASAttackHitCheck");
}

void UAnimNotify_GASAttackHitCheck::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

    if (MeshComp == nullptr)
    {
        return;
    }

	AActor* OwnerActor = MeshComp->GetOwner();
	if (OwnerActor)
	{
		FGameplayEventData PayloadData;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerActor, TriggetGameplayTag, PayloadData);
	}	
}
