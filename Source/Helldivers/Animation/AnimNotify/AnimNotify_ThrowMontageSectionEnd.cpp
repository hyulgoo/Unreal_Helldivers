// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimNotify_ThrowMontageSectionEnd.h"
#include "Interface/HDWeaponInterface.h"
#include "Define/HDDefine.h"

FString UAnimNotify_ThrowMontageSectionEnd::GetNotifyName_Implementation() const
{
    return TEXT("ThrowSectionEnd");
}

void UAnimNotify_ThrowMontageSectionEnd::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	NULL_CHECK(MeshComp);

	AActor* OwnerActor = MeshComp->GetOwner();
	NULL_CHECK(OwnerActor);

    IHDWeaponInterface* WeaponInterface = Cast<IHDWeaponInterface>(OwnerActor);
    NULL_CHECK(WeaponInterface);
    WeaponInterface->SetWeaponActive(true);
}
