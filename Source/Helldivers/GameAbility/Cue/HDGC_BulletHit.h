// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "HDGC_BulletHit.generated.h"

class UParticleSystem;

UCLASS()
class HELLDIVERS_API UHDGC_BulletHit : public UGameplayCueNotify_Static
{
	GENERATED_BODY()
	
public:
	explicit UHDGC_BulletHit();

	virtual bool OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) const override final;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameplayCue)
	TObjectPtr<UParticleSystem> ParticleSystem;
};
