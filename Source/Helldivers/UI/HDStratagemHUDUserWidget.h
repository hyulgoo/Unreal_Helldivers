// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HDStratagemHUDUserWidget.generated.h"

class UImage;
class UVerticalBox;
class UHDStratagemInfoUserWidget;
class UHDStratagemComponent;
struct FGameplayEventData;
enum class EHDCommandInput : uint8;

enum class EHDStratagemWidgetMoveState
{
	NONE,
	APPEAR,
	DISAPPEAR,
};

UCLASS()
class HELLDIVERS_API UHDStratagemHUDUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    void                                            InitializeStratagemHUD(UHDStratagemComponent* InStratagemComponent);
	void											SetHUDActiveByCurrentInputMatchList(const TArray<FName>& MatchStratagemList, const int32 CurrentInputNum);
	void											SetAllWidgetOpacity(const float Opacity);
	void											WidgetAppear(const bool bAppear);
    void                                            OnStratagemEventReceived(const FGameplayEventData* Payload);

protected:
	virtual void									NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override final;
    void											SetStratagemListHUD(UDataTable* StratagemDataTable);

protected:	
    UPROPERTY()
    TObjectPtr<UHDStratagemComponent>				StratagemComponent;

    UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox>						VB_StratagemList;
	
    UPROPERTY(EditAnywhere)
	TMap<EHDCommandInput, UTexture2D*>				CommandIconList;
	
    UPROPERTY(EditAnywhere)
	TMap<FName, UTexture2D*>						StratagemIconList;
	
    UPROPERTY(EditAnywhere)
	TMap<uint8, UTexture2D*>						RemainImageList;
	
    UPROPERTY(EditAnywhere)
	TSubclassOf<UHDStratagemInfoUserWidget>			StratagemInfoWidgetClass;

	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UHDStratagemInfoUserWidget>>	StratagemInfoWidgetList;
	
	int8											MaxCommandCount;

	float											WidgetAppearDistance    = 0.f;
	EHDStratagemWidgetMoveState						MoveState               = EHDStratagemWidgetMoveState::NONE;

};
