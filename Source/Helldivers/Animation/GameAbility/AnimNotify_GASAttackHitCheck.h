// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "AnimNotify_GASAttackHitCheck.generated.h"

UCLASS()
class HELLDIVERS_API UAnimNotify_GASAttackHitCheck : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	explicit UAnimNotify_GASAttackHitCheck();

protected:
	virtual FString GetNotifyName_Implementation() const override;
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	UPROPERTY(EditAnywhere)
	FGameplayTag TriggetGameplayTag;
};
