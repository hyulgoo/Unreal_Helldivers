// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HDDeadInterface.generated.h"

UINTERFACE(MinimalAPI)
class UHDDeadInterface : public UInterface
{
	GENERATED_BODY()
};

class HELLDIVERS_API IHDDeadInterface
{
	GENERATED_BODY()

public:
	virtual void SetDead() = 0;
};
