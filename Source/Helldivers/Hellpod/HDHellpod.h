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
	void								SetSpawnTimeline();

	void								MoveHellpod(const FInputActionValue& Value);
	void								RotateHellpodByCurrentImpulse(const float DeltaTime);
	void								ShakeCamera(const float Scale);

private:
    UPROPERTY(EditDefaultsOnly, Category = "Hellpod")
	TObjectPtr<UStaticMeshComponent>	HellpodMesh;
	
    UPROPERTY(EditDefaultsOnly, Category = "Hellpod")
	TObjectPtr<UBoxComponent>			CollisionBox;
	
	// Input
	UPROPERTY(EditDefaultsOnly, Category = "Hellpod|Input")
	TObjectPtr<UInputAction>			MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = "Hellpod|Input")
	TObjectPtr<UInputMappingContext>	InputMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Hellpod|Input")
    float								MaxMoveSpeed;
	
    UPROPERTY(EditDefaultsOnly, Category = "Hellpod|Input")
    float								FallSpeed;

	FVector2D							CurrentInput;
	
	UPROPERTY(EditDefaultsOnly, Category = "Hellpod|Input")
	float								MaxPitchAngle;

	UPROPERTY(EditDefaultsOnly, Category = "Hellpod|Input")
	float								MaxRollAngle;

	FRotator							MeshDefaultRelativeRotation;
	bool								bIsLanded;

	// Camera
    UPROPERTY(EditDefaultsOnly, Category = "Hellpod|Camera")
	TObjectPtr<USpringArmComponent>		CameraBoom;
	
    UPROPERTY(EditDefaultsOnly, Category = "Hellpod|Camera")
	TObjectPtr<UCameraComponent>		FollowCamera;

	UPROPERTY(VisibleAnywhere, Category = "Hellpod|Camera")
	TObjectPtr<UCameraShakeSourceComponent> CameraShakeSource;
	
	UPROPERTY(EditDefaultsOnly, Category = "Hellpod|Camera")
	TSubclassOf<UCameraShakeBase>		FallCameraShakeClass;

	UPROPERTY(EditDefaultsOnly, Category = "Hellpod|Camera")
	float								CameraShakeScaleWhenFalling;
	
	// Spawn
    UPROPERTY(EditDefaultsOnly, Category = "Hellpod|Spawn")
    TSubclassOf<AHDCharacterPlayer>		CharacterClass;
	
	
    UPROPERTY(EditDefaultsOnly, Category = "Hellpod|Spawn")
	float								SpawnTime;
	
	UPROPERTY();
	TObjectPtr<AHDCharacterPlayer>		SpawnedCharacter;

	FTimeline							SpawnCharacterTimeline;
    UPROPERTY(EditDefaultsOnly, Category = "Hellpod|Spawn")
	TObjectPtr<UCurveFloat>				SpawnCurveFloat;

	//Effect		
    UPROPERTY(EditDefaultsOnly, Category = "Hellpod|Effect")
	FGameplayTag						ImpactTag;
};
