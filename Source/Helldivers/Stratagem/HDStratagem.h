// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HDStratagem.generated.h"

class USphereComponent;
class UNiagaraSystem;
class UNiagaraComponent;

UCLASS()
class HELLDIVERS_API AHDStratagem : public AActor
{
	GENERATED_BODY()

	friend class AHDCharacterPlayer;

public:	
    AHDStratagem(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void								SetStratagemInfo(const FName StratagemName, const float StratagemActiveDelay);
	void								AddImpulseToStratagem(const FVector& ThrowDirection);

protected:
	virtual void						BeginPlay() override final;
	
	UFUNCTION()
    virtual void						OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

private:
	void								SpawnPointLaser();

private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent>	StratagemMesh;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<USphereComponent>		CollisionSphere;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem>			PointLaserNiagara;
	
	UPROPERTY()
	TObjectPtr<UNiagaraComponent>		NiagaraComponent;
	
	UPROPERTY(EditAnywhere)
	float								StratagemActiveDelay;

	FVector								ThrowDirection  = FVector::ZeroVector;
	float								ThrowImpulse    = 3000.f;
	FName								StratagemName   = FName();

};
