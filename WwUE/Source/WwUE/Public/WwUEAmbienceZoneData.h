// gaming

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WwUEAmbienceZoneData.generated.h"

class UAkAudioEvent;

USTRUCT(BlueprintType)
struct FAmbienceSoundBed
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<UAkAudioEvent*> Events;
};

/**
 * 
 */
UCLASS()
class WWUE_API UWwUEAmbienceZoneData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FAmbienceSoundBed PlaySoundBed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FAmbienceSoundBed StopSoundBed;
};