// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HDCharacterBase.generated.h"

class UHDCharacterControlData;
class USkeletalMeshComponent;
enum class EHDCharacterControlType   : uint8;

UCLASS()
class HELLDIVERS_API AHDCharacterBase : public ACharacter
{
    GENERATED_BODY()

public:
    explicit AHDCharacterBase();

protected:
    virtual void                SetCharacterControlData(UHDCharacterControlData* CharacterControlData);

    virtual void                SetDead();
    void                        PlayDeadAnimation();

protected:
    UPROPERTY(EditAnywhere, Category = CharacterControl, Meta = (AllowPrivateAccess = "true"))
    TMap<EHDCharacterControlType, TObjectPtr<UHDCharacterControlData>> CharacterControlManager;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Montage, Meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UAnimMontage>    FireWeaponMontage;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Montage, Meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UAnimMontage>    ThrowMontage;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Montage, Meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UAnimMontage>    DeadMontage;
    
    float                       DeadEventDelayTime = 5.f;
};
