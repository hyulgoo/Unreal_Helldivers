// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "HDHellpod.generated.h"

class UInputComponent;

UCLASS()
class HELLDIVERS_API AHDHellpod : public APawn
{
	GENERATED_BODY()

public:
	explicit AHDHellpod();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void OnHit
};
