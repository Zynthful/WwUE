// gaming

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WeaponTypes.h"
#include "WeaponData.generated.h"

class UAkAudioEvent;

/**
 * 
 */
UCLASS()
class WWUE_API UWeaponData : public UDataAsset
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Firing")
	float FireInterval = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Firing")
	EWeaponFireMode FireMode = EWeaponFireMode::Single;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	UAkAudioEvent* FireSingleAkEvent;
};
