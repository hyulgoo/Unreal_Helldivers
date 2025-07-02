// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "HDInputActionComponent.generated.h"

class UInputAction;
class UHDCharacterControlData;
struct FTaggedInputAction;
enum class EHDCharacterPoseState : uint8;
enum class EHDCharacterControlType : uint8;
enum class EHDCharacterInputAction : uint8;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HELLDIVERS_API UHDInputActionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	explicit						                                    UHDInputActionComponent();

	const bool						                                    IsSprint() const ;
	void							                                    SetSprint(const bool bSprint) ;

	const EHDCharacterPoseState	                                        GetCharacterPoseState() const;
	void							                                    SetCharacterPoseState(const EHDCharacterPoseState NewState, const bool bForced = false);
	void							                                    RestorePoseState();

    const EHDCharacterMovementState                                     GetCharacterMovementState() const;
    void                                                                SetCharacterMovementState(const EHDCharacterMovementState NewState);

	void							                                    SetSpringArmDefaultZOffset(const float ZOffset);
	void							                                    ChangeCameraZOffsetByCharacterMovementState(const EHDCharacterPoseState State);

private:
	UPROPERTY(EditAnywhere, )
	TArray<FTaggedInputAction>					                        TaggedHoldActions;
	
	UPROPERTY(EditAnywhere, )
	TArray<FTaggedInputAction>					                        TaggedToggleActions;
	
	UPROPERTY(EditAnywhere, )
	TMap<EHDCharacterInputAction, TObjectPtr<UInputAction>>             InputActionMap;

	EHDCharacterPoseState		                                        PoseState;
	EHDCharacterPoseState		                                        PrevPoseState;
    EHDCharacterMovementState                                           MovementState;

	bool							                                    bIsSprint;
	
	float					                                            SpringArmZOffset;
    
	// Control
	EHDCharacterControlType						                        CurrentCharacterControlType;
	
    UPROPERTY(EditAnywhere)
    TMap<EHDCharacterControlType, TObjectPtr<UHDCharacterControlData>>  CharacterControlDataMap;	
};
