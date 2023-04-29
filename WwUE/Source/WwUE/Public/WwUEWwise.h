// gaming
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AkGameplayTypes.h"
#include "AkMIDIGameplayStatics.h"
#include "WwUEWwise.generated.h"

class UAkAudioEvent;
class UAkComponent;

struct FMIDIEvent;

DECLARE_LOG_CATEGORY_EXTERN(LogSound, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogMIDI, Log, All);

/**
 * 
 */
UCLASS()
class WWUE_API UWwUEWwise : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	static AkPlayingID Play(UAkAudioEvent* AkEvent, AActor* Actor, bool bStopWhenAttachedToDestroyed = false);

	static AkPlayingID Play(UAkAudioEvent* AkEvent, UAkComponent* AkComponent, bool bStopWhenAttachedToDestroyed = false);

	static AkPlayingID PostMIDIOnEvent(UAkAudioEvent* AkEvent, UAkComponent* AkComponent, FMIDIEvent MIDIEvent, AkPlayingID PlayingID = 0, bool bAbsoluteOffsets = false);

	static AkPlayingID PostMIDIOnEvent(UAkAudioEvent* AkEvent, UAkComponent* AkComponent, TArray<FMIDIEvent>& MIDIEvents, AkPlayingID PlayingID = 0, bool bAbsoluteOffsets = false);

	static bool SetRTPCValue(AActor* Actor, FName RTPCName, float Value, int32 InterpolationTimeMs);

	static bool SetRTPCValue(UAkComponent* AkComponent, FString RTPCName, float Value, int32 InterpolationTimeMs);

	static bool SetRTPCValue(FName RTPCName, float Value, int32 InterpolationTimeMs);

	static bool SetSwitch(AActor* Actor, FName SwitchGroupName, FName SwitchStateName);

	static bool SetSwitch(UAkComponent* AkComponent, FString SwitchGroupName, FString SwitchStateName);

	static bool SetState(FName StateGroupName, FName StateName);
};
