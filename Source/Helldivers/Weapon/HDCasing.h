// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "HDCasing.generated.h"

struct FGameplayTag;

UCLASS()
class HELLDIVERS_API AHDCasing : public AActor
{
	GENERATED_BODY()
	
public:	
    AHDCasing(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
    virtual void						BeginPlay() override;

    UFUNCTION()
    virtual void						OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
private:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMeshComponent>	CasingMesh;
    
	UPROPERTY(EditDefaultsOnly)
    FGameplayTag				        ShellSoundTag;
    
	UPROPERTY(EditDefaultsOnly)
	float								ShellEjectionImpulse = 10.f;
};
