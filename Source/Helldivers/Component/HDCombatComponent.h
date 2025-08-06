
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Character/CharacterTypes/HDCharacterStateTypes.h"
#include "Components/TimelineComponent.h"
#include "HDCombatComponent.generated.h"

class UAnimMontage;
class AHDWeapon;
class AHDStratagem;
class UCharacterMovementComponent;
class USpringArmComponent;
enum class EHDFireType :uint8;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HELLDIVERS_API UHDCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
    UHDCombatComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    const bool				                Fire();
    const bool 				                CanFire() const;

    AHDWeapon*                              SpawnDefaultWeapon();
    void                                    EquipWeapon(AHDWeapon* NewWeapon);

    USkeletalMeshComponent*                 GetWeaponMesh() const;
    void                                    SetWeaponActive(const bool bActive);
    const EHDFireType                       GetWeaponFireType() const;
    const float                             GetWeaponFireDelay() const;
    const int32                             GetWeaponAmmoCount() const;
    const int32                             GetWeaponMaxAmmoCount() const;
    const int32                             GetWeaponCapacityCount() const;
    const int32                             GetWeaponMaxCapacityCount() const;
    const float                             GetWeaponZoomedFOV() const;
    const float                             GetWeaponZoomInterpSpeed() const;


    const bool                              NeedReload() const;
    const bool                              CanReload() const;
    void                                    Reload();
    const float                             GetWeaponReloadDelay(const bool bShoulder) const;

    const EHDCombatState                    GetCombatState() const;
    void                                    SetCombatState(const EHDCombatState State);

    const bool                              IsShoulder() const;
    void                                    SetShoulder(const bool bShoudler);

	const bool                              ContinueFire();
	void                                    ReloadFinished();

    const float                             GetAimOffset_Yaw() const;
    const float                             GetAimOffset_Pitch() const;

    const FVector&                          GetHitTarget() const;
    void                                    SetHitTarget(const FVector& NewHitTarget);

    const float                             GetCurrentFOV() const;
    void                                    SetCurrentFOV(const float NewFOV);

    const float                             GetZoomInterpSpeed() const;

    const bool                              IsCharacterLookingViewport() const;
    void                                    SetSpringArmTargetLength(const float TargetArmLength);

    const EHDTurningInPlace                 GetTurnInPlace() const;

    const float                             GetDefaultFOV() const;
    const bool                              IsUseRotateBone() const;

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

    EHDCombatState			                CombatState                     = EHDCombatState::Unoccupied;
    FVector					                HitTarget                       = FVector::ZeroVector;
    float                                   DefaultFOV                      = 50.f;
    float					                CurrentFOV                      = 0.f;

    UPROPERTY(EditAnywhere)
	float					                ZoomedFOV;

    UPROPERTY(EditAnywhere)
	float					                ZoomInterpSpeed;

    float					                ErgonomicFactor                 = 0.f;
    
    UPROPERTY()
	TObjectPtr<AHDWeapon>	                Weapon;
    
    UPROPERTY(EditAnywhere)
	TObjectPtr<UCurveFloat>	                DefaultCurve;

	FTimeline				                SpringArmArmLengthTimeline;
	float					                SpringArmTargetArmLength        = 0.f;
    
	UPROPERTY(EditAnywhere)
	TSubclassOf<AHDWeapon>					DefaultWeaponClass;
    
    UPROPERTY(EditAnywhere)
    TMap<EHDCombatMontage,TObjectPtr<UAnimMontage>> CombatMontage;
};
