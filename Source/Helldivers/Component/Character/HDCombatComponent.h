
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HDCombatComponent.generated.h"

class AHDWeapon;
enum class EHDCombatState : uint8;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HELLDIVERS_API UHDCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	explicit                UHDCombatComponent();

    const bool				Fire(const bool IsPressed);
    void                    EquipWeapon(AHDWeapon* NewWeapon);
    AHDWeapon*              GetWeapon() const { return Weapon; }
    const bool 				CanFire();

    const bool              CanReload() const;
    void                    Reload();

    const EHDCombatState    GetCombatState() const;
    void                    SetCombatState(const EHDCombatState State);

    const bool              IsShoulder() const;
    void                    SetShoulder(const bool bShoudler);

    const bool              FireFinished();
    void                    ReloadFinished();

    const FVector&          GetHitTarget() const;
    void                    SetHitTarget(const FVector& NewHitTarget);

    const float             GetCurrentFOV() const;
    void                    SetCurrentFOV(const float NewFOV);

    const float             GetZoomInterpSpeed() const { return ZoomInterpSpeed; }

protected:
    virtual void            TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    void					TraceUnderCrosshairs();

private:
    bool					bIsShoulder;
    bool					bIsFireButtonPressed;

    EHDCombatState			CombatState;

    FVector					HitTarget;

    UPROPERTY(EditAnywhere, Category = "FOV")
	float					ZoomedFOV;

	float					CurrentFOV;

    UPROPERTY(EditAnywhere, Category = "FOV")
	float					ZoomInterpSpeed;

    float					ErgonomicFactor;
    
private:
    UPROPERTY(EditAnywhere, Category = "Weapon")
	TObjectPtr<AHDWeapon>	Weapon;
};
