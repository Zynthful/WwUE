// gaming

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EWeaponFireMode : uint8
{
	Single		UMETA(DisplayName = "Single"),
	Automatic	UMETA(DisplayName = "Automatic"),
	Burst		UMETA(DisplayName = "Burst"),
};