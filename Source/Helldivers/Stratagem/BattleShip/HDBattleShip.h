// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemInterface.h"
#include "HDBattleShip.generated.h"

class AHDEagleFighter;
class AHDProjectileBase;
struct FHDStratagemEffectData;

UCLASS()
class HELLDIVERS_API AHDBattleShip : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:	
	explicit AHDBattleShip();

    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	void							    ActivateStratagem(const FName StratagemName, const FTransform& Transform, const float StratagemActiveDelay);

protected:
	virtual void					    BeginPlay() override final;

private:
	FHDStratagemEffectData*			    FindStratagemEffectData(const FName StratagemName) const;

	void							    OrbitalStrikeWithDelay(const FHDStratagemEffectData& StratagemEffectData, int32 BombIndex);
	void							    EagleStrike(const FHDStratagemEffectData& StratagemEffectData);

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
    TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(EditAnywhere)
    TSubclassOf<AHDEagleFighter>		EagleFighterClass;

    TObjectPtr<AHDEagleFighter>			EagleFighter;

    UPROPERTY(EditAnywhere)
    TSubclassOf<AHDProjectileBase>		ProjectileBombClass;

    UPROPERTY(EditAnywhere)
    TSubclassOf<AHDProjectileBase>		ProjectileBulletClass;

    // StratagemEffectData
    UPROPERTY(EditAnywhere)
    TObjectPtr<UDataTable>				StratagemEffectDataTable;

    FTransform 							StratagemTransform;
    uint8								CurrentStratagemIndex;
    FTimerHandle						ActiveStratagemTimerHandle;
    bool								bCanUseStratagem;
};
