// gaming


#include "Weapon/WwUEProjectileWeapon.h"
#include "Weapon/WeaponData.h"
#include "AkGameplayStatics.h"
//#include "AkMIDIGameplayStatics.h"
#include "WwUEWwise.h"
#include "AkAudioEvent.h"
#include "AkComponent.h"

DEFINE_LOG_CATEGORY(LogWeapon);

AWwUEProjectileWeapon::AWwUEProjectileWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	if (WeaponData != nullptr)
	{
		InternalFireInterval = WeaponData->FireInterval;
	}

	AkComponent = CreateDefaultSubobject<UAkComponent>(TEXT("AkComponent"));
	AkComponent->SetupAttachment(RootComponent);
}

void AWwUEProjectileWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWwUEProjectileWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWwUEProjectileWeapon::StartFiring()
{
	if (WeaponData->FireMode == EWeaponFireMode::Automatic)
	{
		bRefiring = true;
	}

	PrepareAkMIDICallback();

	// TODO: Implement fire interval for if we retrigger StartFiring too early
	bool bRetriggeringEarly = false;
	if (bRetriggeringEarly)
	{
		//FTimerDelegate TD_Fire;
		//TD_Fire.BindUFunction(this, GET_FUNCTION_NAME_CHECKED(AWwUEProjectileWeapon, FireSingle));

		//float CurrentTime = GetWorld()->GetTimeSeconds();

		//GetWorldTimerManager().SetTimer(TH_Fire, TD_Fire, LastFireTime + WeaponData->FireInterval - CurrentTime, false);
	}
	else
	{
		FireSingle();
	}
}

void AWwUEProjectileWeapon::StopFiring()
{
	if (WeaponData->FireMode == EWeaponFireMode::Automatic)
	{
		bRefiring = false;
	}

	FTimerManager& WorldTimerManager = GetWorldTimerManager();
	if (WorldTimerManager.IsTimerActive(TH_Fire))
	{
		WorldTimerManager.ClearTimer(TH_Fire);
	}

	ReleaseAkMIDICallback();
}

void AWwUEProjectileWeapon::FireSingle()
{
	if (bRefiring)
	{
		FTimerDelegate TD_Fire;
		TD_Fire.BindUFunction(this, GET_FUNCTION_NAME_CHECKED(AWwUEProjectileWeapon, FireSingle));

		float CurrentTime = GetWorld()->GetTimeSeconds();

		// Set timer for next fire, taking into account frame time
		GetWorldTimerManager().SetTimer(TH_Fire, TD_Fire, LastFireTime + WeaponData->FireInterval - CurrentTime, false);
	}

	LastFireTime = GetWorld()->GetTimeSeconds();
}

void AWwUEProjectileWeapon::SetFireInterval(float NewFireInterval)
{
	if (NewFireInterval == InternalFireInterval)
	{
		return;
	}

	float OldFireInterval = InternalFireInterval;
	InternalFireInterval = FMath::Max(0.01f, NewFireInterval);
	OnFireIntervalChanged(OldFireInterval, InternalFireInterval);
}

void AWwUEProjectileWeapon::OnFireIntervalChanged(float OldFireInterval, float NewFireInterval)
{
	InitAkMIDIFireInterval();
}

void AWwUEProjectileWeapon::PrepareAkMIDICallback()
{
	// Bind to AkAudioDevice's MIDI callback (PreProcessMessageQueueForRender)
	// So we start queueing gunfire audio from that callback
	FAkAudioDevice* AkAudioDevice = FAkAudioDevice::Get();
	if (AkAudioDevice == nullptr)
	{
		UE_LOG(LogWeapon, Error, TEXT("PrepareAkMIDICallback failed! | Failed to get AkAudioDevice"));
		return;
	}
	AkAudioDevice->OnMessageWaitToSend.BindUObject(this, &AWwUEProjectileWeapon::AkMIDICallback);

	AkAudioSettings AudioSettings = UAkMIDIGameplayStatics::GetAudioSettings();

	SamplesPerCallback = AudioSettings.uNumSamplesPerFrame;
	PostLengthSamples = FMath::Max(PostLengthSamples, SamplesPerCallback);
	CurrentCallbackCount = 0;
	CallbackInterval = ((double)AudioSettings.uNumSamplesPerFrame / (double)AudioSettings.uNumSamplesPerSecond);
	TimeUntilNextFire = 0;
	bReleaseMIDICallback = false;
	PostCounter = 0;

	InitAkMIDIFireInterval();
}

void AWwUEProjectileWeapon::ReleaseAkMIDICallback()
{	
	// Unbind from AkAudioDevice's MIDI callback (PreProcessMessageQueueForRender)
	// So we stop queueing any additional gunfire audio
	FAkAudioDevice* AkAudioDevice = FAkAudioDevice::Get();
	if (AkAudioDevice == nullptr)
	{
		UE_LOG(LogWeapon, Error, TEXT("ReleaseAkMIDICallback failed! | Failed to get AkAudioDevice"));
		return;
	}

	// Ensure we live at least one callback
	// (so if we release too early, we still hear a fire sound)
	if (CurrentCallbackCount <= 0)
	{
		// Handle releasing callback on callback finished
		bReleaseMIDICallback = true;
	}
	else
	{
		AkAudioDevice->OnMessageWaitToSend.Unbind();
	}

	FirePlayingID = AK_INVALID_PLAYING_ID;

	//UWwUEWwise::Stop(FirePlayingID);
	//UAkMIDIGameplayStatics::StopMIDIOnEvent(FireSingleAkEvent, AkComponent, FirePlayingID);
}

void AWwUEProjectileWeapon::AkMIDICallback(AkAudioSettings* AudioSettings)
{
	if (GEngine && bPrintAkMIDICallbacks)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, FString::Printf(TEXT("AkMIDICallback")));
	}

	// Calculate current frame and next frame times
	double CurrentCallbackTime = (double)CurrentCallbackCount * CallbackInterval;
	double NextCallbackTime = CurrentCallbackTime + CallbackInterval;

	// Failsafe!
	if (CurrentCallbackTime > TimeUntilNextFire)
	{
		TimeUntilNextFire = CurrentCallbackTime;
	}

	// Must we post this frame?
	if (TimeUntilNextFire >= CurrentCallbackTime && TimeUntilNextFire <= NextCallbackTime)
	{
		// Calculate sample offset, relative to current frame, to post MIDI events
		double SampleOffsetPercent = (TimeUntilNextFire - CurrentCallbackTime) / CallbackInterval;
		uint32 SampleOffset = (uint32)(SampleOffsetPercent * SamplesPerCallback);

		TArray<FMIDIEvent> MIDIEvents;

		FMIDIEvent NoteOnMIDIEvent;
		NoteOnMIDIEvent.NoteType = EMIDINoteType::NoteOn;
		NoteOnMIDIEvent.Channel = 0;
		NoteOnMIDIEvent.OffsetSamples = SampleOffset;
		NoteOnMIDIEvent.ParamOne = 60;
		NoteOnMIDIEvent.ParamTwo = 96;

		FMIDIEvent NoteOffMIDIEvent;
		NoteOffMIDIEvent.NoteType = EMIDINoteType::NoteOff;
		NoteOffMIDIEvent.Channel = 0;
		NoteOffMIDIEvent.OffsetSamples = SampleOffset + PostLengthSamples;
		NoteOffMIDIEvent.ParamOne = 60;
		NoteOffMIDIEvent.ParamTwo = 0;

		MIDIEvents.Emplace(NoteOnMIDIEvent);
		MIDIEvents.Emplace(NoteOffMIDIEvent);

		if (FirePlayingID != AK_INVALID_PLAYING_ID)
		{
			UAkMIDIGameplayStatics::StopMIDIOnEvent(WeaponData->FireSingleAkEvent, AkComponent, FirePlayingID);
		}

		// Post MIDI events
		FirePlayingID = UWwUEWwise::PostMIDIOnEvent(WeaponData->FireSingleAkEvent, AkComponent, MIDIEvents, FirePlayingID);
		++PostCounter;

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, FString::Printf(TEXT("Posting MIDI events, PostCounter = %d, PlayingID = %d, NoteOnSampleOffset = %d, NoteOffSampleOffset = %d"), PostCounter, FirePlayingID, SampleOffset, SampleOffset + PostLengthSamples));
		}

		// Update post time in context
		TimeUntilNextFire += WeaponData->FireInterval;
	}

	// Update counter
	++CurrentCallbackCount;

	if (bReleaseMIDICallback)
	{
		ReleaseAkMIDICallback();
	}
}

void AWwUEProjectileWeapon::InitAkMIDIFireInterval()
{
	double CurrentCallbackTime = (double)CurrentCallbackCount * CallbackInterval;

	// Calculate potential next shot time
	// If the potential next shot happens sooner than our current expected next shot, use the potential instead
	// I think this is because if the fire rate changes mid-stream, we want to change the TimeUntilNextFire
	double MaybeTimeUntilNextFire = CurrentCallbackTime + WeaponData->FireInterval;
	if (MaybeTimeUntilNextFire < TimeUntilNextFire)
	{
		TimeUntilNextFire = MaybeTimeUntilNextFire;
	}

	double PostLength = FMath::Min(CallbackInterval, WeaponData->FireInterval);
	PostLengthSamples = (uint32)((PostLength / CallbackInterval) * SamplesPerCallback);
	PostLengthSamples = FMath::Min(PostLengthSamples, SamplesPerCallback);
}
