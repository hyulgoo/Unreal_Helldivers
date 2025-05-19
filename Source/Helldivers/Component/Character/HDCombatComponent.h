
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

private:
    void                    FireTimerFinished();

public:
    bool					bIsShoulder;
    bool					bCanFire;
    bool					bIsFireButtonPressed;

    FTimerHandle			FireTimer;

    EHDCombatState			CombatState;

    FVector					HitTarget;

	UPROPERTY(EditAnywhere, Category = FOV)
	float					ZoomedFOV;

	float					CurrentFOV;

	UPROPERTY(EditAnywhere, Category = FOV)
	float					ZoomInterpSpeed;

    float					ErgonomicFactor;
    
private:
	UPROPERTY(EditAnywhere, Category = Weapon)
	TObjectPtr<AHDWeapon>	Weapon;
};
