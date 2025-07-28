// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HDCharacterRagdollInterface.generated.h"

UINTERFACE(MinimalAPI)
class UHDCharacterRagdollInterface : public UInterface
{
	GENERATED_BODY()
};

class HELLDIVERS_API IHDCharacterRagdollInterface
{
	GENERATED_BODY()

public:
	virtual void		SetRagdoll(const bool bIsRagdell, const FVector& Impulse = FVector::ZeroVector) = 0;
	virtual const float	GetRagdollPysicsLinearVelocity() const = 0;
};
