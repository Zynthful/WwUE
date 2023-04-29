// gaming
#include "WwUEWwise.h"
#include "AkComponent.h"
#include "AkGameplayStatics.h"
//#include "../Content/Sound/Wwise//SoundBanks/Common/Wwise_IDs.h"
#include <AK/SoundEngine/Common/AkSoundEngine.h>
#include "AkAudioEvent.h"

DEFINE_LOG_CATEGORY(LogSound);
DEFINE_LOG_CATEGORY(LogMIDI);

PRAGMA_DISABLE_OPTIMIZATION

AkPlayingID UWwUEWwise::Play(UAkAudioEvent* AkEvent, AActor* Actor, bool bStopWhenAttachedToDestroyed)
{
	if (AkEvent == nullptr)
	{
		UE_LOG(LogSound, Error, TEXT("SetActorRTPCValue failed | AkEvent = nullptr"));
		return AK_INVALID_PLAYING_ID;
	}

	if (Actor == nullptr)
	{
		UE_LOG(LogSound, Error, TEXT("SetActorRTPCValue failed | Actor = nullptr"));
		return AK_INVALID_PLAYING_ID;
	}

	FOnAkPostEventCallback NullCallback;
	return UAkGameplayStatics::PostEvent(AkEvent, Actor, 0, NullCallback, bStopWhenAttachedToDestroyed);
}

AkPlayingID UWwUEWwise::Play(UAkAudioEvent* AkEvent, UAkComponent* AkComponent, bool bStopWhenAttachedToDestroyed)
{
	if (AkEvent == nullptr)
	{
		UE_LOG(LogSound, Error, TEXT("SetActorRTPCValue failed | AkEvent = nullptr"));
		return AK_INVALID_PLAYING_ID;
	}

	if (AkComponent == nullptr)
	{
		UE_LOG(LogSound, Error, TEXT("SetActorRTPCValue failed | AkComponent = nullptr"));
		return AK_INVALID_PLAYING_ID;
	}

	FOnAkPostEventCallback NullCallback;
	return AkComponent->PostAkEvent(AkEvent, 0, NullCallback, TEXT(""));
}

AkPlayingID UWwUEWwise::PostMIDIOnEvent(UAkAudioEvent* AkEvent, UAkComponent* AkComponent, FMIDIEvent MIDIEvent, AkPlayingID PlayingID, bool bAbsoluteOffsets)
{
	TArray<FMIDIEvent> MIDIEvents;
	MIDIEvents.Emplace(MIDIEvent);
	return PostMIDIOnEvent(AkEvent, AkComponent, MIDIEvents, PlayingID, bAbsoluteOffsets);
}

AkPlayingID UWwUEWwise::PostMIDIOnEvent(UAkAudioEvent* AkEvent, UAkComponent* AkComponent, TArray<FMIDIEvent>& MIDIEvents, AkPlayingID PlayingID, bool bAbsoluteOffsets)
{
	return UAkMIDIGameplayStatics::PostMIDIOnEvent(AkEvent, AkComponent, MIDIEvents, PlayingID, bAbsoluteOffsets);
}

bool UWwUEWwise::SetRTPCValue(AActor* Actor, FName RTPCName, float Value, int32 InterpolationTimeMs)
{
	if (Actor == nullptr)
	{
		UE_LOG(LogSound, Error, TEXT("SetActorRTPCValue failed | Actor = nullptr"));
		return false;
	}

	UAkGameplayStatics::SetRTPCValue(nullptr, Value, InterpolationTimeMs, Actor, RTPCName);
	return true;
}

bool UWwUEWwise::SetRTPCValue(UAkComponent* AkComponent, FString RTPCName, float Value, int32 InterpolationTimeMs)
{
	if (AkComponent == nullptr)
	{
		UE_LOG(LogSound, Error, TEXT("SetAkComponentRTPCValue failed | AkComponent = nullptr"));
		return false;
	}

	AkComponent->SetRTPCValue(nullptr, Value, InterpolationTimeMs, RTPCName);
	return true;
}

bool UWwUEWwise::SetRTPCValue(FName RTPCName, float Value, int32 InterpolationTimeMs)
{
	UAkGameplayStatics::SetRTPCValue(nullptr, Value, InterpolationTimeMs, nullptr, RTPCName);
	return true;
}

bool UWwUEWwise::SetSwitch(AActor* Actor, FName SwitchGroupName, FName SwitchStateName)
{
	if (Actor == nullptr)
	{
		UE_LOG(LogSound, Error, TEXT("SetActorRTPCValue failed | Actor = nullptr"));
		return false;
	}

	UAkGameplayStatics::SetSwitch(nullptr, Actor, SwitchGroupName, SwitchStateName);
	return true;
}

bool UWwUEWwise::SetSwitch(UAkComponent* AkComponent, FString SwitchGroupName, FString SwitchStateName)
{
	if (AkComponent == nullptr)
	{
		UE_LOG(LogSound, Error, TEXT("SetAkComponentRTPCValue failed | AkComponent was nullptr"));
		return false;
	}

	AkComponent->SetSwitch(nullptr, SwitchGroupName, SwitchStateName);
	return true;
}

bool UWwUEWwise::SetState(FName StateGroupName, FName StateName)
{
	UAkGameplayStatics::SetState(nullptr, StateGroupName, StateName);
	return true;
}

PRAGMA_ENABLE_OPTIMIZATION