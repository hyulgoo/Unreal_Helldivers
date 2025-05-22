// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameplayTagContainer.h"
#include "HDHellpod.generated.h"

class UInputComponent;
class AHDCharacterPlayer;
class UBoxComponent;

UCLASS()
class HELLDIVERS_API AHDHellpod : public APawn
{
	GENERATED_BODY()

public:
	explicit		AHDHellpod();

protected:
	virtual void	BeginPlay() override;
	virtual void	Tick(float DeltaTime) override;
	virtual void	SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
	void			OnBoxeHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

private:
	void			SpawnCharacter();

private:
    UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMeshComponent>	HellpodMesh;
	
    UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UBoxComponent>			CollisionComponent;
	
    UPROPERTY(EditDefaultsOnly)
    float								FallSpeed;
	
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<AHDCharacterPlayer>		CharacterClass;
	
    UPROPERTY(EditDefaultsOnly)
	FGameplayCueTag						ImpactTag;
};
