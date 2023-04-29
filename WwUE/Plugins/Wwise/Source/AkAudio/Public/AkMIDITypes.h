// gaming

#pragma once

#include "CoreMinimal.h"
#include "AkMIDITypes.generated.h"

UENUM(BlueprintType)
enum class EMIDINoteType : uint8
{
	NoteOn,
	NoteOff,
};

USTRUCT(BlueprintType)
struct FMIDIEvent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	EMIDINoteType NoteType = EMIDINoteType::NoteOn;

	/* Frame offset in samples to post this MIDI event */
	UPROPERTY(EditAnywhere)
	uint64 OffsetSamples;

	UPROPERTY(EditAnywhere, meta = (ClampMin = 0, ClampMax = 15, UIMin = 0, UIMax = 15))
	uint8 Channel;

	/* Data used for different things based on the NoteType 
	
		- NoteOn	Note		Range: 0-127	Default: 60 (i.e. C4/Middle C)
		- NoteOff	Note		Range: 0-127	Default: 60 (i.e. C4/Middle C)

	*/
	UPROPERTY(EditAnywhere)
	uint8 ParamOne;

	/* Data used for different things based on the NoteType

		- NoteOn	Velocity	Range: 0-127	Default: 96
		- NoteOff	Velocity	Range: 0-127	Default: 96
	
	*/
	UPROPERTY(EditAnywhere)
	uint8 ParamTwo;
};