// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/HDCharacterRagdollInterface.h"
#include "Interface/HDDeadInterface.h"
#include "HDCharacterBase.generated.h"

class UHDCharacterControlData;
class USkeletalMeshComponent;
enum class EHDCharacterControlType   : uint8;

UCLASS()
class HELLDIVERS_API AHDCharacterBase : public ACharacter, public IHDCharacterRagdollInterface, public IHDDeadInterface
{
    GENERATED_BODY()

public:
    explicit                    AHDCharacterBase();

protected:
    virtual void                SetDead() override;
    void                        PlayDeadAnimation();

    virtual void				SetRagdoll(const bool bRagdoll, const FVector& Impulse = FVector::ZeroVector) override;
    virtual const float			GetRagdollPysicsLinearVelocity() const override;

    void                        LoadDefaultMontage();

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage")
    TObjectPtr<UAnimMontage>    FireWeaponMontage;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage")
    TObjectPtr<UAnimMontage>    ReloadWeaponMontage;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage")
    TObjectPtr<UAnimMontage>    ThrowMontage;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage")
    TObjectPtr<UAnimMontage>    DeadMontage;
    
    float                       DeadEventDelayTime;
};
