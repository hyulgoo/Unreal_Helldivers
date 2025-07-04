// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "HDTA_Trace.generated.h"

UCLASS()
class HELLDIVERS_API AHDTA_Trace : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()

public:
	explicit		AHDTA_Trace() = default;

	virtual void	StartTargeting(UGameplayAbility* Ability) override final;
	virtual void	ConfirmTargetingAndContinue() override final;
	void			SetShowDebug(const bool InShowDebug) { bShowDebug = InShowDebug; }

protected:
	virtual FGameplayAbilityTargetDataHandle MakeTargetData() const;

private:
	bool			bShowDebug = false;
};
