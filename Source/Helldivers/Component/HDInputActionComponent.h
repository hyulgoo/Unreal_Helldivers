// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "HDInputActionComponent.generated.h"

class UInputAction;
class UHDCharacterControlData;
enum class EHDCharacterStanceState : uint8;
enum class EHDCharacterMovementState : uint8;
enum class EHDCharacterControlType : uint8;
enum class EHDCharacterInputAction : uint8;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HELLDIVERS_API UHDInputActionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	explicit						UHDInputActionComponent();

	const EHDCharacterStanceState	GetStanceState() const;
	void							SetStanceState(const EHDCharacterStanceState NewState, const bool bForced = false);
	void							RestoreStanceState();

    const EHDCharacterMovementState GetMovementState() const;
    void                            SetMovementState(const EHDCharacterMovementState NewState);

	void							SetSpringArmDefaultZOffset(const float ZOffset);
	void							ChangeCameraZOffsetByCharacterMovementState(const EHDCharacterStanceState State);

    UHDCharacterControlData*        SetControlType(const EHDCharacterControlType Type);
    const EHDCharacterControlType   GetControlType() const;

    const TMap<EHDCharacterInputAction, TObjectPtr<UInputAction>>& GetInputActionMap() const;

private:
	EHDCharacterStanceState		    StanceState;
	EHDCharacterStanceState		    PrevStanceState;
	EHDCharacterMovementState       MovementState;
	
	float					        SpringArmZOffset;
    
	UPROPERTY(EditAnywhere)
	TMap<EHDCharacterInputAction, TObjectPtr<UInputAction>> InputActionMap;

	EHDCharacterControlType			CurrentCharacterControlType;
	
    UPROPERTY(EditAnywhere)
    TMap<EHDCharacterControlType, TObjectPtr<UHDCharacterControlData>> CharacterControlDataMap;	
};
