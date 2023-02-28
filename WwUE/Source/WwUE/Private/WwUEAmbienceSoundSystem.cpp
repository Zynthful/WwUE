// gaming

#include "WwUEAmbienceSoundSystem.h"
#include "WwUEAmbienceZone.h"
#include "WwUEAmbienceZoneData.h"
#include "../Plugins/Wwise/Source/AkAudio/Classes/AkGameplayStatics.h"

void UWwUEAmbienceSoundSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UWwUEAmbienceSoundSystem::Deinitialize()
{
	Super::Deinitialize();
}

void UWwUEAmbienceSoundSystem::AddAmbienceZone(AWwUEAmbienceZone* Zone)
{
	if (Zone == nullptr)
	{
		return;
	}

	InAmbienceZones.Add(Zone);

	if (CurrentAmbienceZone != nullptr && Zone->GetPriority() < CurrentAmbienceZone->GetPriority())
	{
		return;
	}

	SetAmbience(Zone);
}

void UWwUEAmbienceSoundSystem::RemoveAmbienceZone(AWwUEAmbienceZone* Zone)
{
	if (Zone == nullptr)
	{
		return;
	}

	InAmbienceZones.Remove(Zone);

	if (InAmbienceZones.IsEmpty())
	{
		ClearAmbience();
	}
	else if (AWwUEAmbienceZone* HighestPriorityAmbienceZone = GetHighestPriorityAmbienceZone())
	{
		SetAmbience(HighestPriorityAmbienceZone);
	}
}

void UWwUEAmbienceSoundSystem::SetAmbience(AWwUEAmbienceZone* Zone)
{
	if (Zone == nullptr)
	{
		return;
	}

	// Stop old ambience
	if (CurrentAmbienceZone != nullptr)
	{
		if (UWwUEAmbienceZoneData* OldZoneData = CurrentAmbienceZone->GetAmbienceData())
		{
			for (UAkAudioEvent* StopEvent : OldZoneData->StopSoundBed.Events)
			{
				UAkGameplayStatics::PostEvent(StopEvent, CurrentAmbienceZone, 0, FOnAkPostEventCallback());
			}
		}
	}

	// Play new ambience
	if (UWwUEAmbienceZoneData* NewZoneData = Zone->GetAmbienceData())
	{
		for (UAkAudioEvent* PlayEvent : NewZoneData->PlaySoundBed.Events)
		{
			UAkGameplayStatics::PostEvent(PlayEvent, Zone, 0, FOnAkPostEventCallback());
		}
	}

	CurrentAmbienceZone = Zone;
}

void UWwUEAmbienceSoundSystem::ClearAmbience()
{
	if (CurrentAmbienceZone == nullptr)
	{
		return;
	}

	if (UWwUEAmbienceZoneData* OldZoneData = CurrentAmbienceZone->GetAmbienceData())
	{
		for (UAkAudioEvent* StopEvent : OldZoneData->StopSoundBed.Events)
		{
			UAkGameplayStatics::PostEvent(StopEvent, CurrentAmbienceZone, 0, FOnAkPostEventCallback());
		}
	}

	CurrentAmbienceZone = nullptr;
}

AWwUEAmbienceZone* UWwUEAmbienceSoundSystem::GetHighestPriorityAmbienceZone()
{
	AWwUEAmbienceZone* HighestPriorityAmbienceZone = nullptr;
	for (AWwUEAmbienceZone* Zone : InAmbienceZones)
	{
		if (HighestPriorityAmbienceZone == nullptr || Zone->GetPriority() >= HighestPriorityAmbienceZone->GetPriority())
		{
			HighestPriorityAmbienceZone = Zone;
		}
	}
	return HighestPriorityAmbienceZone;
}
