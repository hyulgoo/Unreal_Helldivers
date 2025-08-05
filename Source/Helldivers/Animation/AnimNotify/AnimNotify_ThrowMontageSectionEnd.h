// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_ThrowMontageSectionEnd.generated.h"

UCLASS()
class HELLDIVERS_API UAnimNotify_ThrowMontageSectionEnd : public UAnimNotify
{
    GENERATED_BODY()

protected:
    virtual FString GetNotifyName_Implementation() const override;
    virtual void    Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
