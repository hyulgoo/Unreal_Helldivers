// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/HDCharacterPlayer.h"
#include "AbilitySystemInterface.h"
#include "GAS/Struct/HDTaggedInputAction.h"
#include "HDGASCharacterPlayer.generated.h"

class UAbilitySystemComponent;
class UEnhancedInputComponent;
class UGameplayAbility;
class UGameplayEffect;
class UCurveFloat;
struct FGameplayTag;
struct FGameplayEventData;
struct FHDCharacterStat;

UENUM(BlueprintType)
enum class EHDCharacterInputAction : uint8
{
	ThirdLook,
	ThirdMove,
	FirstLook,
	FirstMove,
	ChangeControl,
	Count
};

USTRUCT(BlueprintType)
struct FTagEventBindInfo
{
    GENERATED_BODY()

public:
	FTagEventBindInfo()
		: BindFunctionName(FName())
		, InputAction(nullptr)
		, EventConditionTag(FGameplayTag())
	{
	};

    UPROPERTY(EditDefaultsOnly)
    FName						BindFunctionName;

    UPROPERTY(EditDefaultsOnly)
    TObjectPtr<UInputAction>	InputAction;

    UPROPERTY(EditDefaultsOnly)
    FGameplayTag				EventConditionTag;
};

UCLASS()
class HELLDIVERS_API AHDGASCharacterPlayer : public AHDCharacterPlayer, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	explicit									AHDGASCharacterPlayer();

	virtual UAbilitySystemComponent*			GetAbilitySystemComponent() const override final;
	void										SetArmor(const EHDArmorType NewArmorType);
	void										ChangeCharacterControlType();

protected:	
	virtual void								BeginPlay() override final;
	virtual void								Tick(float DeltaTime) override final;
	virtual void								SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override final;
	virtual void								PossessedBy(AController* NewController) override final;
	void										SetupGASInputComponent(UEnhancedInputComponent* EnhancedInputComponent);
	void										SetGASEventInputComponent(UEnhancedInputComponent* EnhancedInputComponent);
	void										GASInputPressed(const FGameplayTag Tag);
	void										GASInputReleased(const FGameplayTag Tag);

	UFUNCTION()
	void										InputStratagemCommand(const FInputActionValue& Value);

	void										HandleGameplayEvent(const FGameplayEventData* Payload);

private:
	void										SetStratagemHUDAppear(const bool bAppear);

	void										InitAbilitySystemComponent();
	void										InitializeAttributeSet();
	const FHDCharacterStat*						GetCharacterStatByArmorType(const EHDArmorType ArmorType) const;

	virtual	void								SetSprint(const bool bSprint) override final;
	virtual void								SetCrouch(const bool bCrouch) override final;
	virtual void								SetProne(const bool bProne) override final;
	virtual void								SetShouldering(const bool bShoulder) override final;
	void										ThirdPersonLook(const FInputActionValue& Value);
	void										ThirdPersonMove(const FInputActionValue& Value);
	void										FirstPersonLook(const FInputActionValue& Value);
	void										FirstPersonMove(const FInputActionValue& Value);
	void										SetCharacterControl(const EHDCharacterControlType NewCharacterControlType);
	void										SetCharacterControlData(UHDCharacterControlData* CharacterControlData);

	UFUNCTION()
	void										OnCameraSpringArmLengthTImelineUpdate(const float Value);

private:
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent>			AbilitySystemComponent;

	UPROPERTY(EditAnywhere, Category = "GASPlayer|Stat")
	TSubclassOf<UGameplayEffect>				InitStatEffect;

	UPROPERTY(EditAnywhere, Category = "GASPlayer|Input")
	TArray<TSubclassOf<UGameplayAbility>>		StartAbilities;

	UPROPERTY(EditAnywhere, Category = "GASPlayer|Input")
	TArray<FTaggedInputAction>					TaggedTriggerActions;
	
	UPROPERTY(EditAnywhere, Category = "GASPlayer|Input")
	TMap<EHDCharacterInputAction, TObjectPtr<UInputAction>> InputActionMap;

	UPROPERTY(EditAnywhere, Category = "GASPlayer|Input")
	FGameplayTagContainer						EventCallTags;

	UPROPERTY(EditAnywhere, Category = "GASPlayer|Input")
	TArray<FTagEventBindInfo>					TagEventBindInfoList;

	// Stat
	UPROPERTY(EditAnywhere, Category = "GASPlayer|Stat")
	EHDArmorType								ArmorType;
	
	UPROPERTY(EditAnywhere, Category = "GASPlayer|Stat")
	TObjectPtr<UDataTable>						ArmorTypeStatusDataTable;

	// Control
	EHDCharacterControlType						CurrentCharacterControlType;
	
    UPROPERTY(EditAnywhere, Category = "GASPlayer|CharacterControl")
    TMap<EHDCharacterControlType, TObjectPtr<UHDCharacterControlData>> CharacterControlDataMap;
	
	FTimeline									ArmLengthTimeline;
};