// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HDCasing.generated.h"

class USoundCue;

UCLASS()
class HELLDIVERS_API AHDCasing : public AActor
{
	GENERATED_BODY()
	
public:	
	explicit							AHDCasing();

protected:
    virtual void						BeginPlay() override;

    UFUNCTION()
    virtual void						OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent>	CasingMesh;

	UPROPERTY(EditAnywhere)
	float								ShellEjectionImpulse = 10.f;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundCue>				ShellSound;
};
