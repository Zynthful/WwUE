// gaming

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AkGameplayTypes.h"
#include "AkMIDITypes.h"
#include "AkMIDIGameplayStatics.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAkMIDI, Log, All)

class UAkComponent;

/**
 * 
 */
UCLASS()
class AKAUDIO_API UAkMIDIGameplayStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Audiokinetic|MIDI")
	static int32 PostMIDIOnEvent(UAkAudioEvent* AkEvent, UAkComponent* AkComponent, UPARAM(ref) TArray<FMIDIEvent>& MIDIEvents, int32 PlayingID = 0, bool bAbsoluteOffsets = false);

	UFUNCTION(BlueprintCallable, Category = "Audiokinetic|MIDI")
	static bool StopMIDIOnEvent(UAkAudioEvent* AkEvent, UAkComponent* AkComponent, int32 PlayingID = 0);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Audiokinetic|MIDI")
	static int64 GetSampleTick();

	static AkAudioSettings GetAudioSettings();

	static AkMIDIPost MakeAkMIDIPost(FMIDIEvent MIDIEvent);
	
};
