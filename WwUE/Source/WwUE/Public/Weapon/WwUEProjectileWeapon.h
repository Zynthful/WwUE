// gaming

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WwUEProjectileWeapon.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogWeapon, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFireIntervalChangedSignature, float, NewFireInterval);

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

public:
	virtual void StartFiring();
	virtual void StopFiring();

	UFUNCTION(BlueprintCallable, Category = "Firing")
	void SetFireInterval(float NewFireInterval);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firing")
	float GetFireInterval();

protected:

	UFUNCTION()
	virtual void FireSingle();

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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sound")
	UAkComponent* AkComponent;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Data")
	UWeaponData* WeaponData;

	UPROPERTY(BlueprintReadOnly, Category = "Firing")
	float LastFireTime;

	UPROPERTY(Transient)
	float InternalFireInterval = 0.1f;

protected:
	FTimerHandle TH_Fire;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Firing")
	uint32 bRefiring : 1;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Firing")
	uint32 bFiring : 1;

	/* MIDI ------------------------------------------------------------------- */

	/* Whether to log the AkMIDICallbacks to the screen */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound|Debug")
	uint8 bPrintAkMIDICallbacks : 1;

	/* Number of times we've called PostMIDIOnEvent in the current stream of shots */
	UPROPERTY(Transient)
	uint32 PostCounter;

	/* Length of the MIDI events to post in samples*/
	UPROPERTY(Transient)
	uint32 PostLengthSamples;

	/* Number of samples per callback */
	UPROPERTY(Transient)
	uint32 SamplesPerCallback;

	/* How many callbacks have happened so far */
	UPROPERTY(Transient)
	uint32 CurrentCallbackCount;

	/* Time between callbacks in seconds*/
	UPROPERTY(Transient)
	double CallbackInterval;

	/* Time until the next shot in seconds*/
	UPROPERTY(Transient)
	double TimeUntilNextFire;

	UPROPERTY(Transient)
	/* Whether to release the MIDI callback on finishing AkMIDICallback */
	uint8 bReleaseMIDICallback : 1;

	UPROPERTY(Transient)
	uint32 FirePlayingID;

	/* End MIDI -------------------------------------------------------------- */

	UPROPERTY(Transient)
	uint32 NumShotsFiredThisStream;

public:
	UPROPERTY(BlueprintAssignable, Category = "Firing")
	FOnFireIntervalChangedSignature OnFireIntervalChangedSignature;
};
