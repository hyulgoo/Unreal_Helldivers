// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemInterface.h"
#include "HDBattleShip.generated.h"

class AHDEagleFighter;
class AHDProjectileBase;
class USphereComponent;
struct FHDStratagemEffectData;

UCLASS()
class HELLDIVERS_API AHDBattleShip : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:	
	explicit                            AHDBattleShip();

    virtual UAbilitySystemComponent*    GetAbilitySystemComponent() const override;
	void							    ActivateStratagem(const FName StratagemName, const FTransform& Transform, const float StratagemActiveDelay);

protected:
	virtual void					    BeginPlay() override final;

private:
	FHDStratagemEffectData*			    FindStratagemEffectData(const FName StratagemName) const;

	void							    OrbitalStrikeWithDelay(const FHDStratagemEffectData& StratagemEffectData, int32 BombIndex);
	void							    EagleStrike(const FHDStratagemEffectData& StratagemEffectData);

protected:
    UPROPERTY(EditDefaultsOnly, Category = "BattleShip")
    TObjectPtr<UStaticMeshComponent>    BattleShipMesh;
    
    UPROPERTY(EditDefaultsOnly, Category = "BattleShip")
	TObjectPtr<USphereComponent>		CollisionSphere;

    UPROPERTY(VisibleAnywhere, Category = "GAS")
    TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(EditAnywhere, Category = "Stratagem|Eagle")
    TSubclassOf<AHDEagleFighter>		EagleFighterClass;

    TObjectPtr<AHDEagleFighter>			EagleFighter;

    UPROPERTY(EditAnywhere, Category = "Stratagem|Projectile")
    TSubclassOf<AHDProjectileBase>		ProjectileBombClass;

    UPROPERTY(EditAnywhere, Category = "Stratagem|Projectile")
    TSubclassOf<AHDProjectileBase>		ProjectileBulletClass;

    // StratagemEffectData
    UPROPERTY(EditAnywhere, Category = "Stratagem|Data")
    TObjectPtr<UDataTable>				StratagemEffectDataTable;

    FTransform 							StratagemTransform;
    FTimerHandle						ActiveStratagemTimerHandle;
    uint8								CurrentStratagemIndex;
    bool								bCanUseStratagem;
};
