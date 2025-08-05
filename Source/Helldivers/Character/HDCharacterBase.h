// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Interface/HDCharacterRagdollInterface.h"
#include "Interface/HDDeadInterface.h"
#include "Character/CharacterTypes/HDCharacterStateTypes.h"
#include "Define/HDDefine.h"
#include "Define/HDGameplayTag.h"
#include "HDCharacterBase.generated.h"

class UHDAbilitySystemComponent;
class UGameplayAbility;
class UGameplayEffect;
struct FHDCharacterAttributeData;

UCLASS()
class HELLDIVERS_API AHDCharacterBase : public ACharacter, public IAbilitySystemInterface, public IHDCharacterRagdollInterface, public IHDDeadInterface
{
    GENERATED_BODY()

public:
    AHDCharacterBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
    virtual UAbilitySystemComponent*        GetAbilitySystemComponent() const override final;
    void                                    SetAbilitySystemComponent(AActor* OwnerActor, UAbilitySystemComponent* ASC);

    template < class T >
    T*                                      GetAbilitySystemComponent() const { return Cast<T>(GetAbilitySystemComponent()); }

    virtual void                            SetDead() override;

    virtual void				            SetRagdoll(const bool bRagdoll, const FVector& Impulse = FVector::ZeroVector) override;
    virtual const float			            GetRagdollPysicsLinearVelocity() const override;

private:
    FHDCharacterAttributeData*              GetAttributeStatDataByArmorType(const EHDArmorType NewArmorType);
    void                                    LoadDefaultMontage();

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage")
    TObjectPtr<UAnimMontage>                DeadMontage;
    
    float                                   DeadEventDelayTime = 10.f;

private:
	UPROPERTY()
	TObjectPtr<UHDAbilitySystemComponent>	AbilitySystemComponent;

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<UGameplayAbility>>	Abilities;
    
    UPROPERTY(EditAnywhere)
    EHDArmorType                            ArmorType = EHDArmorType::Medium;

	UPROPERTY(EditAnywhere)
    TObjectPtr<UDataTable>					ArmorTypeStatusDataTable;

    UPROPERTY(EditAnywhere)
    TSubclassOf<UGameplayEffect>            InitAttributeStatEffect;
};
