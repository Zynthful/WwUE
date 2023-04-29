// gaming

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	UAkAudioEvent* StartFiringSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	UAkAudioEvent* StopFiringSound;
};
