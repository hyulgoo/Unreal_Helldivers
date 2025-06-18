
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Character/CharacterTypes/HDCharacterStateTypes.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/TimelineComponent.h"
#include "HDCombatComponent.generated.h"

class AHDWeapon;
class AHDStratagem;
class UCharacterMovementComponent;
class USpringArmComponent;
enum class EHDCombatState : uint8;
enum class EHDFireType :uint8;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent, DependsOn=CharacterMovementComponent))
class HELLDIVERS_API UHDCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	explicit                                UHDCombatComponent();

    const bool				                Fire(const bool IsPressed);
    const bool 				                CanFire() const;

    void                                    SpawnDefaultWeapon();
    void                                    EquipWeapon(AHDWeapon* NewWeapon);

    AHDWeapon*                              GetWeapon() const;
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

	const bool                              FireFinished();
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

    const float                             GetDefaultFOV() const;
    const bool                              IsUseRotateBone() const;

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
    
    FRotator				                StartingAimRotation;
    float					                AimOffset_Yaw;
    float					                InterpAimOffset_Yaw;
    float					                AimOffset_Pitch;

    uint8					                bIsCharacterLookingViewport : 1;
    uint8					                bUseRotateRootBone : 1;
    
	UPROPERTY(EditAnywhere,                 Category = "Combat")
	float					                TurnThreshold;

	EHDTurningInPlace		                TurningInPlace;

    uint8					                bIsShoulder : 1;
    uint8					                bIsFireButtonPressed : 1;

    EHDCombatState			                CombatState;

    FVector					                HitTarget;

    float                                   DefaultFOV;
    UPROPERTY(EditAnywhere,                 Category = "Combat")
	float					                ZoomedFOV;

	float					                CurrentFOV;

    UPROPERTY(EditAnywhere,                 Category = "Combat")
	float					                ZoomInterpSpeed;

    float					                ErgonomicFactor;
    
    UPROPERTY()
	TObjectPtr<AHDWeapon>	                Weapon;
    
    UPROPERTY(EditAnywhere,                 Category = "Combat")
	TObjectPtr<UCurveFloat>	                DefaultCurve;

	FTimeline				                SpringArmArmLengthTimeline;
	float					                SpringArmTargetArmLength;
    
	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<AHDWeapon>					DefaultWeaponClass;
};
