// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimNotify_ThrowMontageDetach.h"
#include "Define/HDDefine.h"

FString UAnimNotify_ThrowMontageDetach::GetNotifyName_Implementation() const
{
    return TEXT("ThrowDetach");
}

void UAnimNotify_ThrowMontageDetach::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    Super::Notify(MeshComp, Animation, EventReference);
    NULL_CHECK(MeshComp);
}
