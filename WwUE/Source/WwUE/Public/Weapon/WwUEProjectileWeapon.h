// gaming

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WwUEProjectileWeapon.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogWeapon, Log, All);

class UWeaponData;
class UAkComponent;

struct AkAudioSettings;


UCLASS()
class WWUE_API AWwUEProjectileWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWwUEProjectileWeapon();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void StartFiring();
	virtual void StopFiring();

	UFUNCTION()
	virtual void FireSingle();

	UFUNCTION(BlueprintCallable, Category = "Firing")
	void SetFireInterval(float NewFireInterval);

	void OnFireIntervalChanged(float OldFireInterval, float NewFireInterval);

	/* Bind to MIDI callback so we can start queueing gunfire audio ahead of time, plus calculating the necessary variables */
	void PrepareAkMIDICallback();

	/* Unbind from MIDI callback so we stop playing gunfire audio, plus resetting the necessary variables */
	void ReleaseAkMIDICallback();

	/* MIDI callback from Wwise which is executed every buffer/audio frame
	* This is used to trigger the gunfire audio at the exact sample (at x% through a buffer/audio frame) to ensure sample accuracy
	*/
	void AkMIDICallback(AkAudioSettings* AudioSettings);

	void InitAkMIDIFireInterval();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sound")
	UAkComponent* AkComponent;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Data")
	UWeaponData* WeaponData;

	UPROPERTY(BlueprintReadOnly, Category = "Firing")
	float LastFireTime;

	UPROPERTY(BlueprintReadOnly, Category = "Firing")
	float InternalFireInterval = 0.1f;

protected:
	FTimerHandle TH_Fire;

	UPROPERTY(BlueprintReadOnly, Category = "Firing")
	uint32 bRefiring : 1;

	/* MIDI ------------------------------------------------------------------- */

	/* Whether to log the AkMIDICallbacks to the screen */
	uint8 bPrintAkMIDICallbacks : 1;

	/* Number of times we've called PostMIDIOnEvent in the current stream of shots */
	uint32 PostCounter;

	/* Length of the MIDI events to post in samples*/
	uint32 PostLengthSamples;

	/* Number of samples per callback */
	uint32 SamplesPerCallback;

	/* How many callbacks have happened so far */
	uint32 CurrentCallbackCount;

	/* Time between callbacks in seconds*/
	double CallbackInterval;

	/* Time until the next shot in seconds*/
	double TimeUntilNextFire;

	/* Whether to release the MIDI callback on finishing AkMIDICallback */
	uint8 bReleaseMIDICallback : 1;

	uint32 FirePlayingID;

	/* End MIDI -------------------------------------------------------------- */

	uint32 NumShotsFiredThisStream;
};
