
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Character/CharacterTypes/HDCharacterStateTypes.h"
#include "Components/TimelineComponent.h"
#include "HDCombatComponent.generated.h"

class UAnimMontage;
class UAnimationAsset;
class UCharacterMovementComponent;
class USpringArmComponent;
class UAbilitySystemComponent;
class UGameplayEffect;
class AHDWeapon;
class AHDStratagem;
class AHDProjectileBase;
enum class EHDFireType :uint8;
enum class EHDWeaponAnimationType : uint8;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSpreadChanged, float);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HELLDIVERS_API UHDCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
    UHDCombatComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    AHDWeapon*                              SpawnDefaultWeapon();
    void                                    EquipWeapon(AHDWeapon* NewWeapon, UAbilitySystemComponent* ASC);
    void                                    SpawnProjectile();     

    AHDWeapon*                              GetWeapon() const;
    USkeletalMeshComponent*                 GetWeaponMesh() const;
    void                                    SetWeaponActive(const bool bActive);
    UAnimMontage*                           GetWeaponMontage() const;
    const EHDFireType                       GetWeaponFireType() const;
    const float                             GetWeaponFireDelay() const;
    const int32                             GetWeaponAmmoCount() const;
    const int32                             GetWeaponMaxAmmoCount() const;
    const int32                             GetWeaponCapacityCount() const;
    const int32                             GetWeaponMaxCapacityCount() const;
    const float                             GetWeaponZoomedFOV() const;
    const float                             GetWeaponZoomInterpSpeed() const;
    const bool                              IsWeaponAutoFire() const;
    const bool                              IsEquippedWeapon() const;
    const bool                              IsShoulder() const;
    void                                    SetShoulder(const bool bShoudler);

    const float                             GetAimOffset_Yaw() const;
    const float                             GetAimOffset_Pitch() const;

    const FVector&                          GetHitTarget() const;

    const float                             GetCurrentFOV() const;
    void                                    SetCurrentFOV(const float NewFOV);
    const float                             GetDefaultFOV() const;
    const float                             GetZoomInterpSpeed() const;

    const bool                              IsUseRotateBone() const;
    const bool                              IsCharacterLookingViewport() const;
    void                                    SetSpringArmTargetLength(const float TargetArmLength);

    const EHDTurningInPlace                 GetTurnInPlace() const;
    UAnimMontage*                           GetCombatMontage(const EHDCombatMontage MontageType);

protected:
    virtual void                            BeginPlay() override final;
    virtual void                            TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    void					                TraceUnderCrosshairs();
    void					                AimOffset(const float DeltaTime);
    void									TurnInPlace(const float DeltaTime);

    UFUNCTION()
    void                                    OnSpringArmLengthUpdate(const float Value);

public:
    FOnSpreadChanged                        OnSpreadChanged;

private:
    UPROPERTY()
    TObjectPtr<UCharacterMovementComponent> CharacterMovement;

    UPROPERTY()
    TObjectPtr<USpringArmComponent>         SpringArm;
    
    FRotator				                StartingAimRotation             = FRotator::ZeroRotator;
    float					                AimOffset_Yaw                   = 0.f;
    float					                AimOffset_Pitch                 = 0.f;
    float					                InterpAimOffset_Yaw             = 0.f;

    uint8					                bIsCharacterLookingViewport : 1 = false;
    uint8					                bUseRotateRootBone : 1          = false;
    
	UPROPERTY(EditAnywhere)
	float					                TurnThreshold;

	EHDTurningInPlace		                TurningInPlace                  = EHDTurningInPlace::NotTurning;
    uint8					                bIsShoulder : 1                 = false;
    uint8					                bIsEquipped : 1                 = false;
    FVector					                HitTarget                       = FVector::ZeroVector;
    float                                   DefaultFOV                      = 50.f;
    float					                CurrentFOV                      = 0.f;
	float					                ZoomedFOV                       = 0.f;
	float					                ZoomInterpSpeed                 = 0.f;
    float					                ErgonomicFactor                 = 0.f;
    float 				                    CurrentSpread                   = 0.f;
    
    UPROPERTY(EditAnywhere)
	TObjectPtr<UCurveFloat>	                DefaultCurve;

	FTimeline				                SpringArmArmLengthTimeline;
	float					                SpringArmTargetArmLength        = 0.f;
    
	UPROPERTY(EditAnywhere)
	TSubclassOf<AHDWeapon>					DefaultWeaponClass;
    
    UPROPERTY()
	TObjectPtr<AHDWeapon>	                Weapon;
    
    UPROPERTY()
	TObjectPtr<UGameplayEffect>	            ReloadGameplayEffect;

    UPROPERTY(EditAnywhere)
    TMap<EHDCombatMontage,TObjectPtr<UAnimMontage>> CombatMontage;
};
