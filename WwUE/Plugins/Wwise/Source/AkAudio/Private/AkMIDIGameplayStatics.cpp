// gaming


#include "AkMIDIGameplayStatics.h"
#include "AkComponent.h"
#include "AkAudioEvent.h"

DEFINE_LOG_CATEGORY(LogAkMIDI)

PRAGMA_DISABLE_OPTIMIZATION

int32 UAkMIDIGameplayStatics::PostMIDIOnEvent(UAkAudioEvent* AkEvent, UAkComponent* AkComponent, UPARAM(ref) TArray<FMIDIEvent>& MIDIEvents, int32 PlayingID, bool bAbsoluteOffsets)
{
	if (AkEvent == nullptr)
	{
		UE_LOG(LogAkMIDI, Error, TEXT("PostMIDIOnEvent failed | AkEvent was nullptr"))
		return AK_INVALID_PLAYING_ID;
	}

	if (AkComponent == nullptr)
	{
		UE_LOG(LogAkMIDI, Error, TEXT("PostMIDIOnEvent failed | AkComponent was nullptr"))
		return AK_INVALID_PLAYING_ID;
	}

	if (MIDIEvents.IsEmpty())
	{
		UE_LOG(LogAkMIDI, Error, TEXT("PostMIDIOnEvent failed | MIDIEvents was empty"))
		return AK_INVALID_PLAYING_ID;
	}

	AkUniqueID EventID = FAkAudioDevice::GetIDFromString(AkEvent->GetName());

	AkUniqueID GameObjectID = AkComponent->GetAkGameObjectID();
	AK::SoundEngine::RegisterGameObj(GameObjectID);

	TArray<AkMIDIPost> AkMIDIPosts;
	for (FMIDIEvent& MIDIEvent : MIDIEvents)
	{
		AkMIDIPost MIDIPost = MakeAkMIDIPost(MIDIEvent);
		AkMIDIPosts.Emplace(MIDIPost);
	}

	AkUInt32 Flags = 0;
	AkCallbackFunc CallbackFunction = NULL;
	void* Cookie = NULL;
	PlayingID = AK::SoundEngine::PostMIDIOnEvent(EventID, GameObjectID, AkMIDIPosts.GetData(), AkMIDIPosts.Num(), bAbsoluteOffsets, Flags, CallbackFunction, Cookie, PlayingID);

	bool bAllowSyncRender = false;
	AK::SoundEngine::RenderAudio(bAllowSyncRender);

	return PlayingID;
}

int64 UAkMIDIGameplayStatics::GetSampleTick()
{
	return AK::SoundEngine::GetSampleTick();
}

AkAudioSettings UAkMIDIGameplayStatics::GetAudioSettings()
{
	AkAudioSettings AudioSettings;
	AK::SoundEngine::GetAudioSettings(AudioSettings);
	return AudioSettings;
}

AkMIDIPost UAkMIDIGameplayStatics::MakeAkMIDIPost(FMIDIEvent MIDIEvent)
{
	AkMIDIPost Post = AkMIDIPost();

	Post.uOffset = MIDIEvent.OffsetSamples;
	Post.byChan = MIDIEvent.Channel;

	switch (MIDIEvent.NoteType)
	{
	case (EMIDINoteType::NoteOn):
		Post.byType = AK_MIDI_EVENT_TYPE_NOTE_ON;
		Post.NoteOnOff.byNote = MIDIEvent.ParamOne;
		Post.NoteOnOff.byVelocity = MIDIEvent.ParamTwo;
		break;
	case (EMIDINoteType::NoteOff):
		Post.byType = AK_MIDI_EVENT_TYPE_NOTE_OFF;
		Post.NoteOnOff.byNote = MIDIEvent.ParamOne;
		Post.NoteOnOff.byVelocity = MIDIEvent.ParamTwo;
		break;
	default:
		UE_LOG(LogAkMIDI, Error, TEXT("MakeAKMIDIPost failed | NoteType is invalid"))
		Post.byType = AK_MIDI_EVENT_TYPE_INVALID;
		break;
	}

	return Post;
}

PRAGMA_ENABLE_OPTIMIZATION
