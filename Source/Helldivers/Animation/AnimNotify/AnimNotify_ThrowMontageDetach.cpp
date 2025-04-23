// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimNotify_ThrowMontageDetach.h"
#include "Interface/HDCharacterCommandInterface.h"
#include "Stratagem/HDStratagem.h"

FString UAnimNotify_ThrowMontageDetach::GetNotifyName_Implementation() const
{
    return TEXT("ThrowDetach");
}

void UAnimNotify_ThrowMontageDetach::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    Super::Notify(MeshComp, Animation, EventReference);

    if(MeshComp == nullptr)
    {
        return;
    }

    IHDCharacterCommandInterface* CharacterCommandInterface = Cast<IHDCharacterCommandInterface>(MeshComp->GetOwner());
    if (CharacterCommandInterface)
    {
        AHDStratagem* Stratagem = CharacterCommandInterface->GetStratagem();
        if (Stratagem)
        {
            Stratagem->AddImpulseToStratagem(Stratagem->GetActorRotation().Vector());
        }
    }
}
