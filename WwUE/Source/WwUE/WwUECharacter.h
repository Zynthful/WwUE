// gaming
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "WwUECharacter.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCharacter, Log, All);

class UInputComponent;
class UInputAction;
class UInputMappingContext;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;
class UWeaponData;
class UAkComponent;
class UAkAudioEvent;

namespace AK
{
	class IAkGlobalPluginContext;
}

struct AkAudioSettings;
struct FMIDIEvent;

enum AkGlobalCallbackLocation;

UCLASS(config=Game)
class AWwUECharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AWwUECharacter();

protected:
	virtual void BeginPlay();

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }

	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	/** Setter to set the bool */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	void SetHasRifle(bool bNewHasRifle);

	/** Getter for the bool */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	bool GetHasRifle();

protected:

	UFUNCTION(BlueprintCallable)
	void StartFiring(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable)
	void StopFiring(const FInputActionValue& Value);

	UFUNCTION()
	void FireSingle();

	/* Bind to MIDI callback so we can start queueing gunfire audio ahead of time, plus calculating the necessary variables */
	void PrepareAkMIDICallback();

	/* Unbind from MIDI callback so we stop playing gunfire audio, plus resetting the necessary variables */
	void ReleaseAkMIDICallback();

	/* MIDI callback from Wwise which is executed every buffer/audio frame 
	 * This is used to trigger the gunfire audio at the exact sample (at x% through a buffer/audio frame) to ensure sample accuracy
	*/
	void AkMIDICallback(AkAudioSettings* AudioSettings);

	void OnFireIntervalChanged(float Value);

	void InitAkMIDIFireInterval();

private:
	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = "WwUECharacter|Mesh")
	USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WwUECharacter|Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WwUECharacter|Input", meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WwUECharacter|Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WwUECharacter|Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WwUECharacter|Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/* Fire Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WwUECharacter|Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* FireAction;


protected:
	UPROPERTY(EditDefaultsOnly, Category = "WwUECharacter|Weapon", meta = (ClampMin = 0.01f))
	float FireInterval = 0.2f;

	UPROPERTY(Transient)
	uint8 bIsFiring : 1;

	/** Bool for AnimBP to switch to another animation set */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WwUECharacter|Weapon")
	bool bHasRifle;

	UPROPERTY(Transient)
	FTimerHandle TH_FireInterval;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WwUECharacter|Sound")
	UAkComponent* AkComponent;

	UPROPERTY(EditDefaultsOnly, Category = "WwUECharacter|Sound")
	UAkAudioEvent* FireSingleAkEvent;

	/* MIDI ------------------------------------------------------------------- */

	/* Pool of MIDI events that can be posted on the next callback */
	UPROPERTY()
	TArray<FMIDIEvent> MIDIEventPool;

	/* Number of times we've called PostMIDIOnEvent in the current stream of shots */
	uint32 PostCounter;

	/* Length of the MIDI events to post in samples*/
	uint32 PostLengthSamples;

	/* Number of samples per callback */
	uint32 SamplesPerCallback;

	/* How many callbacks have happened so far */
	uint32 CurrentCallbackCount;

	/* Time between callbacks in milliseconds*/
	double MsPerCallback;

	/* Time between shots in milliseconds*/
	//double FireIntervalMs;

	/* Time until the next shot in milliseconds*/
	double TimeUntilNextFireMs;

	/* Whether to release the MIDI callback on finishing AkMIDICallback */
	uint8 bReleaseMIDICallback : 1;

	uint32 FiringPlayingID;

	/* End MIDI -------------------------------------------------------------- */

	uint32 NumShotsFiredThisStream;


};

