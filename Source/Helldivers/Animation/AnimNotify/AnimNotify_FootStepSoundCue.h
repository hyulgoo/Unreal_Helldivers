// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_FootStepSoundCue.generated.h"

UENUM(BlueprintType)
enum class EPhysicsMaterialType : uint8
{
	Default_Dirt,
	Stone,
	Wood,
	Metal,
	Count
};

UCLASS()
class HELLDIVERS_API UAnimNotify_FootStepSoundCue : public UAnimNotify
{
	GENERATED_BODY()

public:
	explicit												UAnimNotify_FootStepSoundCue();

protected:
	virtual FString											GetNotifyName_Implementation() const override final;
	virtual void											Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

private:
	const EPhysicsMaterialType								GetPhysicsMaterialTypeByPhysicSurface(const EPhysicalSurface PhysicSurface);

public:
	UPROPERTY(EditAnywhere, Category = "AnimNotify")
	FName													FootSocketName;

	UPROPERTY(EditAnywhere, Category = "AnimNotify")
	float													TraceDistance;
	
	UPROPERTY(EditAnywhere, Category = "AnimNotify")
	TMap<EPhysicsMaterialType, FGameplayTag>				PhysicsCueTagMap;
};
