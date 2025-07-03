// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/HDCharacterRagdollInterface.h"
#include "Interface/HDDeadInterface.h"
#include "AbilitySystemInterface.h"
#include "HDCharacterBase.generated.h"

class UHDCharacterControlData;
class USkeletalMeshComponent;
class UHDAbilitySystemComponent;
class UGameplayAbility;
struct FHDCharacterStat;
enum class EHDCharacterControlType : uint8;

UCLASS()
class HELLDIVERS_API AHDCharacterBase : public ACharacter, public IAbilitySystemInterface, public IHDCharacterRagdollInterface, public IHDDeadInterface
{
    GENERATED_BODY()

public:
    explicit                                AHDCharacterBase();

protected:
    virtual UAbilitySystemComponent*        GetAbilitySystemComponent() const override final;
    void                                    SetAbilitySystemComponent(UAbilitySystemComponent* ASC);
    virtual void                            SetDead() override;

    virtual void				            SetRagdoll(const bool bRagdoll, const FVector& Impulse = FVector::ZeroVector) override;
    virtual const float			            GetRagdollPysicsLinearVelocity() const override;

private:
    FHDCharacterStat*                       GetAttributeStatDataByArmorType(const EHDArmorType NewArmorType);
    void                                    LoadDefaultMontage();

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage")
    TObjectPtr<UAnimMontage>                FireWeaponMontage;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage")
    TObjectPtr<UAnimMontage>                ReloadWeaponMontage;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage")
    TObjectPtr<UAnimMontage>                ThrowMontage;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage")
    TObjectPtr<UAnimMontage>                DeadMontage;
    
    float                                   DeadEventDelayTime;

private:
	UPROPERTY()
	TObjectPtr<UHDAbilitySystemComponent>	AbilitySystemComponent;

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<UGameplayAbility>>	Abilities;
    
    UPROPERTY(EditAnywhere)
    EHDArmorType                            ArmorType;

	UPROPERTY(EditAnywhere)
    TObjectPtr<UDataTable>					ArmorTypeStatusDataTable;
};
