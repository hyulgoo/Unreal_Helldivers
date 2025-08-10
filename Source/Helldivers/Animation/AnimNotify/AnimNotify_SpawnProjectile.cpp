// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimNotify_SpawnProjectile.h"
#include "AbilitySystem/GameplayAbilityHelper.h"
#include "Define/HDDefine.h"

FString UAnimNotify_SpawnProjectile::GetNotifyName_Implementation() const
{
    return EventTag.IsValid() ? EventTag.GetTagName().ToString() : TEXT("Empty");
}

void UAnimNotify_SpawnProjectile::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    AActor* WeaponActor = MeshComp->GetOwner();
    NULL_CHECK(WeaponActor);

    AActor* OwnerActor = WeaponActor->GetOwner();
    NULL_CHECK(OwnerActor);

    FGameplayAbilityHelper::SendGameplayEventToTarget(EventTag, WeaponActor, OwnerActor);
}
