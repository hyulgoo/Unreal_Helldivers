// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimNotify_ThrowMontageSectionEnd.h"
#include "Interface/HDWeaponInterface.h"

FString UAnimNotify_ThrowMontageSectionEnd::GetNotifyName_Implementation() const
{
    return TEXT("ThrowSectionEnd");
}

void UAnimNotify_ThrowMontageSectionEnd::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp == nullptr)
	{
		return;
	}

	AActor* OwnerActor = MeshComp->GetOwner();
	if (OwnerActor)
	{
		IHDWeaponInterface* WeaponInterface = Cast<IHDWeaponInterface>(OwnerActor);
		if (WeaponInterface)
		{
			WeaponInterface->SetWeaponActive(true);
		}
	}
}
