// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "HDGC_Base.generated.h"

class UParticleSystem;
class USoundCue;

UCLASS()
class HELLDIVERS_API UHDGC_Base : public UGameplayCueNotify_Static
{
	GENERATED_BODY()
	
public:
	explicit UHDGC_Base();

	virtual bool OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) const override final;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayCue|Pacticle")
	TObjectPtr<UParticleSystem> ParticleSystem;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayCue|Sound")
	TObjectPtr<USoundBase>		EffectSound;
};
