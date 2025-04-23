#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Character/CharacterTypes/HDArmorTypes.h"
#include "HDCharacterStat.generated.h"

USTRUCT(BlueprintType)
struct FHDCharacterStat : public FTableRowBase
{
	GENERATED_BODY()

public:
	FHDCharacterStat() = default;

	FHDCharacterStat operator+(const FHDCharacterStat& Other) const
	{
		const float* const ThisPtr = reinterpret_cast<const float* const>(this);
		const float* const OtherPtr = reinterpret_cast<const float* const>(&Other);

		FHDCharacterStat Result;
		float* ResultPtr = reinterpret_cast<float*>(&Result);
		int32 StatNum = sizeof(FHDCharacterStat) / sizeof(float);
		for (int32 i = 0; i < StatNum; i++)
		{
			ResultPtr[i] = ThisPtr[i] + OtherPtr[i];
		}

		return Result;
	}

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float MaxHp = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float MovementSpeed = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float MaxStamina = 0.f;	
};
