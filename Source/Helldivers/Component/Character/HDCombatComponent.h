
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

    friend class            AHDCharacterPlayer;

public:	
	explicit                UHDCombatComponent();


    const bool				Fire(const bool IsPressed);
    void                    EquipWeapon(AHDWeapon* NewWeapon);
    AHDWeapon*              GetWeapon() const { return Weapon; }
    const bool 				CanFire();

    const bool              CanReload();
    void                    Reload();

protected:
    virtual void            TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    const bool              FireFinished();
    void                    ReloadFinished();
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
