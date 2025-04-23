// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HDCharacterCommandInterface.generated.h"

enum class EHDCommandInput : uint8;
class AHDStratagem;

UINTERFACE(MinimalAPI)
class UHDCharacterCommandInterface : public UInterface
{
	GENERATED_BODY()
};

class HELLDIVERS_API IHDCharacterCommandInterface
{
	GENERATED_BODY()

public:
	virtual AHDStratagem*	GetStratagem() const = 0;
	virtual const FVector   GetThrowDirection() const = 0;
	virtual void			ThrowStratagem() = 0;
};
