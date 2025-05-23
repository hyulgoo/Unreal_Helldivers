// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameplayTagContainer.h"
#include "Components/TimelineComponent.h"
#include "HDHellpod.generated.h"

class UInputComponent;
class AHDCharacterPlayer;
class UBoxComponent;
class USpringArmComponent;
class UCameraComponent;
class UProjectileMovementComponent;

UCLASS()
class HELLDIVERS_API AHDHellpod : public APawn
{
	GENERATED_BODY()

public:
	explicit							AHDHellpod();

protected:
	virtual void						BeginPlay() override;
	virtual void						Tick(float DeltaTime) override;
	virtual void						SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;	

	UFUNCTION()
	void								OnBoxHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

private:
	void								SpawnCharacter();

	UFUNCTION()
	void								OnSpawnTimelineUpdate(const float Value);

	UFUNCTION()
	void								SpawnCharacterEnd();

private:
    UPROPERTY(EditDefaultsOnly, Category = "Hellpod")
	TObjectPtr<UStaticMeshComponent>			HellpodMesh;
	
    UPROPERTY(EditDefaultsOnly, Category = "Hellpod")
	TObjectPtr<UBoxComponent>					CollisionBox;
	
	UPROPERTY(VisibleAnywhere, Category = "Hellpod")
	TObjectPtr<UProjectileMovementComponent>	ProjectileMovementComponent;

    UPROPERTY(EditDefaultsOnly, Category = "Hellpod|Camera")
	TObjectPtr<USpringArmComponent>				CameraBoom;
	
    UPROPERTY(EditDefaultsOnly, Category = "Hellpod|Camera")
	TObjectPtr<UCameraComponent>				FollowCamera;
	
    UPROPERTY(EditDefaultsOnly, Category = "Hellpod|Spawn")
    float										FallSpeed;
	
    UPROPERTY(EditDefaultsOnly, Category = "Hellpod|Spawn")
    TSubclassOf<AHDCharacterPlayer>				CharacterClass;
	
    UPROPERTY(EditDefaultsOnly, Category = "Hellpod|Effect")
	FGameplayTag								ImpactTag;
	
    UPROPERTY(EditDefaultsOnly, Category = "Hellpod|Spawn")
	float										SpawnTime;
	
	UPROPERTY();
	TObjectPtr<AHDCharacterPlayer>				SpawnedCharacter;

	FTimeline									SpawnCharacterTimeline;
    UPROPERTY(EditDefaultsOnly, Category = "Hellpod|Spawn")
	TObjectPtr<UCurveFloat>						SpawnCurveFloat;
};
