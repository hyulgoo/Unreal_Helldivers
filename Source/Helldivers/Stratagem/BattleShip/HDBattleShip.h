// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HDBattleShip.generated.h"

class AHDEagleFighter;
class AHDProjectileBase;
struct FHDStratagemEffectData;

UCLASS()
class HELLDIVERS_API AHDBattleShip : public AActor
{
	GENERATED_BODY()
	
public:	
	explicit AHDBattleShip();

	void							ActivateStratagem(const FName StratagemName, const FTransform& Transform, const float StratagemActiveDelay);

protected:
	virtual void					BeginPlay() override final;

private:
	FHDStratagemEffectData*			FindStratagemEffectData(const FName StratagemName) const;

	void							OrbitalStrikeWithDelay(const FHDStratagemEffectData& StratagemEffectData, int32 BombIndex);
	void							EagleStrike(const FHDStratagemEffectData& StratagemEffectData);

private:
	UPROPERTY(VisibleAnywhere)
	TSubclassOf<AHDEagleFighter>	EagleFighterClass;

	TObjectPtr<AHDEagleFighter>		EagleFighter;

    UPROPERTY(VisibleAnywhere)
    TSubclassOf<AHDProjectileBase>	    ProjectileBombClass;
    
    UPROPERTY(VisibleAnywhere)
    TSubclassOf<AHDProjectileBase>	    ProjectileBulletClass;

	// StratagemEffectData
    UPROPERTY(VisibleAnywhere)
	TObjectPtr<UDataTable>			StratagemEffectDataTable;

    FTransform 						StratagemTransform;
	uint8							CurrentStratagemIndex;
	FTimerHandle					ActiveStratagemTimerHandle;
	bool							bCanUseStratagem;
};
