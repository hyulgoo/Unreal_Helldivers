// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "HDGameState.generated.h"

class AHDBattleShip;

UCLASS()
class HELLDIVERS_API AHDGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	explicit AHDGameState() = default;

    AHDBattleShip*  GetBattleShip() const { return BattleShip; }
    void            SetBattleShip(AHDBattleShip* NewBattleShip) { BattleShip = NewBattleShip; }

protected:
    virtual void    BeginPlay() override;

private:
    UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
    TObjectPtr<AHDBattleShip>   BattleShip;
    
    UPROPERTY(EditAnywhere)
    TSubclassOf<AHDBattleShip>  BattleShipClass;
};
