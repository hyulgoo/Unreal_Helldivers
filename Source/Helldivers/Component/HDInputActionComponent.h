// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "Define/HDDefine.h"
#include "GameData/HDTaggedInputAction.h"
#include "HDInputActionComponent.generated.h"

class UInputAction;
class UHDCharacterControlData;
enum class EHDCharacterStanceState : uint8;
enum class EHDCharacterMovementState : uint8;
enum class EHDCharacterControlType : uint8;
enum class EHDCharacterInputAction : uint8;

UCLASS()
class HELLDIVERS_API UHDInputActionComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:	
	explicit						                        UHDInputActionComponent();

	const EHDCharacterStanceState	                        GetStanceState() const;
	void							                        SetStanceState(const EHDCharacterStanceState NewState, const bool bForced = false);
	void							                        RestoreStanceState();

    const EHDCharacterMovementState                         GetMovementState() const;
    void                                                    SetMovementState(const EHDCharacterMovementState NewState);

	void							                        SetSpringArmDefaultZOffset(const float ZOffset);
	void							                        ChangeCameraZOffsetByCharacterMovementState(const EHDCharacterStanceState State);

    UHDCharacterControlData*                                SetControlType(const EHDCharacterControlType Type);
    const EHDCharacterControlType                           GetControlType() const;

    const TMap<EHDCharacterInputAction, TObjectPtr<UInputAction>>& GetInputActionMap() const;

    template<class UserClass, typename TriggeredFuncType, typename ReleasedFuncType, typename ToggledFuncType>
    void SetTaggedInputActionDataAsset(UHDInputDataAsset* DataAsset, UserClass* Object, TriggeredFuncType TriggeredFunc, ReleasedFuncType ReleasedFunc, ToggledFuncType ToggledFunc);

private:
	EHDCharacterStanceState		                            StanceState;
	EHDCharacterStanceState		                            PrevStanceState;
	EHDCharacterMovementState                               MovementState;
	
	float					                                SpringArmZOffset;
    
	UPROPERTY(EditAnywhere)
	TMap<EHDCharacterInputAction, TObjectPtr<UInputAction>> InputActionMap;

	EHDCharacterControlType			                        CurrentCharacterControlType;
	
    UPROPERTY(EditAnywhere)
    TMap<EHDCharacterControlType, TObjectPtr<UHDCharacterControlData>> CharacterControlDataMap;	
};

template<class UserClass, typename TriggeredFuncType, typename ReleasedFuncType, typename ToggledFuncType>
void UHDInputActionComponent::SetTaggedInputActionDataAsset(UHDInputDataAsset* DataAsset, UserClass* Object, TriggeredFuncType TriggeredFunc, ReleasedFuncType ReleasedFunc, ToggledFuncType ToggledFunc)
{
    NULL_CHECK(DataAsset);

    for (const FHDTaggedInputAction& InputAction : DataAsset->AbilityInputActions)
    {
        if (InputAction.TriggerType == EHDTriggerType::Hold)
        {
            if (TriggeredFunc)
            {
                BindAction(InputAction.InputAction, ETriggerEvent::Triggered, Object, TriggeredFunc, InputAction.InputTag);
            }

            if (ReleasedFunc)
            {
                BindAction(InputAction.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, InputAction.InputTag);
            }
        }
        else if (InputAction.TriggerType == EHDTriggerType::Toggle)
        {
            if (ToggledFunc)
            {
                BindAction(InputAction.InputAction, ETriggerEvent::Started, Object, ToggledFunc, InputAction.InputTag);
            }
        }
        else if (InputAction.TriggerType == EHDTriggerType::Trigger)
        {
            if (TriggeredFunc)
            {
                BindAction(InputAction.InputAction, ETriggerEvent::Started, Object, TriggeredFunc, InputAction.InputTag);
            }
        }
    }
}