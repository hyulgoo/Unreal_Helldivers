// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameplayTagContainer.h"
#include "Components/TimelineComponent.h"
#include "HDHellpod.generated.h"

class UInputComponent;
class UInputAction;
class UInputMappingContext;
class AHDCharacterPlayer;
class UBoxComponent;
class USpringArmComponent;
class UCameraComponent;
class UFloatingPawnMovement;
struct FInputActionValue;

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

	void								MoveHellpod(const FInputActionValue& Value);
	void								RotateHellpodByCurrentImpulse(const float DeltaTime);

private:
    UPROPERTY(EditDefaultsOnly, Category = "Hellpod")
	TObjectPtr<UStaticMeshComponent>	HellpodMesh;
	
    UPROPERTY(EditDefaultsOnly, Category = "Hellpod")
	TObjectPtr<UBoxComponent>			CollisionBox;
	
    UPROPERTY(EditDefaultsOnly, Category = "Hellpod")
    float								MaxMoveSpeed;

	UPROPERTY(EditAnywhere, Category = "Hellpod|Input")
	TObjectPtr<UInputAction>			MoveAction;

	UPROPERTY(EditAnywhere, Category = "Hellpod|Input")
	TObjectPtr<UInputMappingContext>	InputMappingContext;
	
    UPROPERTY(EditDefaultsOnly, Category = "Hellpod|Camera")
	TObjectPtr<USpringArmComponent>		CameraBoom;
	
    UPROPERTY(EditDefaultsOnly, Category = "Hellpod|Camera")
	TObjectPtr<UCameraComponent>		FollowCamera;
	
    UPROPERTY(EditDefaultsOnly, Category = "Hellpod|Spawn")
    float								FallSpeed;
	
    UPROPERTY(EditDefaultsOnly, Category = "Hellpod|Spawn")
    TSubclassOf<AHDCharacterPlayer>		CharacterClass;
	
    UPROPERTY(EditDefaultsOnly, Category = "Hellpod|Effect")
	FGameplayTag						ImpactTag;
	
    UPROPERTY(EditDefaultsOnly, Category = "Hellpod|Spawn")
	float								SpawnTime;
	
	UPROPERTY();
	TObjectPtr<AHDCharacterPlayer>		SpawnedCharacter;

	FTimeline							SpawnCharacterTimeline;
    UPROPERTY(EditDefaultsOnly, Category = "Hellpod|Spawn")
	TObjectPtr<UCurveFloat>				SpawnCurveFloat;

	float								InputForward;
	float								InputRight;
	FVector2D							CurrentInput;

	UPROPERTY(EditAnywhere, Category = "Hellpod|Input")
	float								MaxPitchAngle;

	UPROPERTY(EditAnywhere, Category = "Hellpod|Input")
	float								MaxRollAngle;

	FRotator							MeshDefaultRelativeRotation;
	FRotator							MeshMinRelativeRotation;
	FRotator							MeshMaxRelativeRotation;
};
