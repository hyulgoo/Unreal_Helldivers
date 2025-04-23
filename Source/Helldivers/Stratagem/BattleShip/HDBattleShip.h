// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HDBattleShip.generated.h"

class AHDEagleFighter;
class AHDProjectile;
struct FHDStratagemEffectData;

UCLASS()
class HELLDIVERS_API AHDBattleShip : public AActor
{
	GENERATED_BODY()
	
public:	
	explicit AHDBattleShip();

	void							ActivateStratagem(const FName StratagemName, const FTransform& Transform, const float StratagemActiveDelay);

protected:
	virtual void					BeginPlay() override;

private:
	FHDStratagemEffectData*			FindStratagemEffectData(const FName StratagemName) const;

	void							OrbitalStrikeWithDelay(const FHDStratagemEffectData& StratagemEffectData, int32 BombIndex);
	void							EagleStrike(FHDStratagemEffectData* StratagemEffectData);

private:
	UPROPERTY(VisibleAnywhere)
	TSubclassOf<AHDEagleFighter>	EagleFighterClass;

	TObjectPtr<AHDEagleFighter>		EagleFighter;

    UPROPERTY(VisibleAnywhere)
    TSubclassOf<AHDProjectile>	    ProjectileBombClass;
    
    UPROPERTY(VisibleAnywhere)
    TSubclassOf<AHDProjectile>	    ProjectileBulletClass;

	// StratagemEffectData
	UDataTable*						StratagemEffectDataTable;
    FTransform 						StratagemTransform;
	uint8							CurrentStratagemIndex;
	FTimerHandle					ActiveStratagemTimerHandle;
};
