// gaming
#include "WwUECharacter.h"
#include "WwUEProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "AkGameplayStatics.h"
#include "WwUEWwise.h"
#include "AkAudioEvent.h"
#include "AkComponent.h"
//#include "AkAudioDevice.h"

DEFINE_LOG_CATEGORY(LogCharacter);

AWwUECharacter::AWwUECharacter()
{
	// Character doesnt have a rifle at start
	bHasRifle = false;
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	AkComponent = CreateDefaultSubobject<UAkComponent>(TEXT("AkComponent"));
	AkComponent->SetupAttachment(GetRootComponent());

}

void AWwUECharacter::BeginPlay()
{
	Super::BeginPlay();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AWwUECharacter::SetHasRifle(bool bNewHasRifle)
{
	bHasRifle = bNewHasRifle;
}

bool AWwUECharacter::GetHasRifle()
{
	return bHasRifle;
}

void AWwUECharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if (!ensure(EnhancedInputComponent))
	{
		UE_LOG(LogCharacter, Error, TEXT("SetupPlayerInputComponent failed | EnhancedInputComponent = nullptr"));
	}

	// Jumping
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

	// Moving
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AWwUECharacter::Move);

	// Looking
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AWwUECharacter::Look);

	// Firing
	EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &AWwUECharacter::StartFiring);
	EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &AWwUECharacter::StopFiring);
}

void AWwUECharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AWwUECharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AWwUECharacter::StartFiring(const FInputActionValue& Value)
{
	if (bIsFiring)
	{
		return;
	}

	UE_LOG(LogCharacter, Verbose, TEXT("StartFiring = %s"), *GetName());

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Purple, FString::Printf(TEXT("START FIRING")));
	}

	bIsFiring = true;
	NumShotsFiredThisStream = 0;

	FireSingle();

	FTimerDelegate TD_FireInterval;
	TD_FireInterval.BindUFunction(this, GET_FUNCTION_NAME_CHECKED(AWwUECharacter, FireSingle));
	GetWorldTimerManager().SetTimer(TH_FireInterval, TD_FireInterval, FireInterval, true);

	PrepareAkMIDICallback();
}

void AWwUECharacter::StopFiring(const FInputActionValue& Value)
{
	if (!bIsFiring)
	{
		return;
	}

	UE_LOG(LogCharacter, Verbose, TEXT("StopFiring = %s"), *GetName());

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Purple, FString::Printf(TEXT("STOP FIRING, Num = %d"), NumShotsFiredThisStream));
	}

	bIsFiring = false;

	FTimerManager& WorldTimerManager = GetWorldTimerManager();
	if (ensure(WorldTimerManager.IsTimerActive(TH_FireInterval)))
	{
		WorldTimerManager.ClearTimer(TH_FireInterval);
	}

	ReleaseAkMIDICallback();

	NumShotsFiredThisStream = 0;
}

void AWwUECharacter::FireSingle()
{
	UE_LOG(LogCharacter, Verbose, TEXT("FireSingle = %s"), *GetName());

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Orange, FString::Printf(TEXT("(GAME) FireSingle")));
	}

	NumShotsFiredThisStream++;
}

void AWwUECharacter::PrepareAkMIDICallback()
{
	// Bind to AkAudioDevice's MIDI callback (PreProcessMessageQueueForRender)
	// So we start queueing gunfire audio from that callback
	FAkAudioDevice* AkAudioDevice = FAkAudioDevice::Get();
	if (AkAudioDevice == nullptr)
	{
		UE_LOG(LogCharacter, Error, TEXT("PrepareAkMIDICallback failed! | Failed to get AkAudioDevice"));
		return;
	}
	AkAudioDevice->OnMessageWaitToSend.BindUObject(this, &AWwUECharacter::AkMIDICallback);

	AkAudioSettings AudioSettings = UAkMIDIGameplayStatics::GetAudioSettings();

	SamplesPerCallback = AudioSettings.uNumSamplesPerFrame;
	PostLengthSamples = FMath::Max(PostLengthSamples, SamplesPerCallback);
	CurrentCallbackCount = 0;
	MsPerCallback = ((double)AudioSettings.uNumSamplesPerFrame / (double)AudioSettings.uNumSamplesPerSecond) * 1000;
	TimeUntilNextFireMs = 0;
	bReleaseMIDICallback = false;

	InitAkMIDIFireInterval();
}

void AWwUECharacter::ReleaseAkMIDICallback()
{
	// Unbind fromm AkAudioDevice's MIDI callback (PreProcessMessageQueueForRender)
	// So we stop queueing any additional gunfire audio
	FAkAudioDevice* AkAudioDevice = FAkAudioDevice::Get();
	if (AkAudioDevice == nullptr)
	{
		UE_LOG(LogCharacter, Error, TEXT("ReleaseAkMIDICallback failed! | Failed to get AkAudioDevice"));
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

	//UAkMIDIGameplayStatics::StopMIDIOnEvent(FireSingleAkEvent, AkComponent, FiringPlayingID);
}

void AWwUECharacter::AkMIDICallback(AkAudioSettings* AudioSettings)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, FString::Printf(TEXT("AkMIDICallback")));
	}

	// Calculate current frame and next frame times
	double CurrentCallbackTimeMs = (double)CurrentCallbackCount * MsPerCallback;
	double NextCallbackTimeMs = CurrentCallbackTimeMs + MsPerCallback;

	// Failsafe!
	if (CurrentCallbackTimeMs > TimeUntilNextFireMs)
	{
		TimeUntilNextFireMs = CurrentCallbackTimeMs;
	}

	// Must we post this frame?
	if (TimeUntilNextFireMs >= CurrentCallbackTimeMs && TimeUntilNextFireMs <= NextCallbackTimeMs)
	{
		// Calculate sample offset, relative to current frame, to post MIDI eventss
		double SampleOffsetPercent = (TimeUntilNextFireMs - CurrentCallbackTimeMs) / MsPerCallback;
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

		if (FiringPlayingID != AK_INVALID_PLAYING_ID)
		{
			UAkMIDIGameplayStatics::StopMIDIOnEvent(FireSingleAkEvent, AkComponent, FiringPlayingID);
		}

		// Post MIDI events
		FiringPlayingID = UWwUEWwise::PostMIDIOnEvent(FireSingleAkEvent, AkComponent, MIDIEvents, FiringPlayingID);
		++PostCounter;

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, FString::Printf(TEXT("Posting MIDI events, PostCounter = %d, PlayingID = %d, NoteOnSampleOffset = %d, NoteOffSampleOffset = %d"), PostCounter, FiringPlayingID, SampleOffset, SampleOffset + PostLengthSamples));
		}

		// Update post time in context
		TimeUntilNextFireMs += (FireInterval * 1000);
	}

	// Update counter
	++CurrentCallbackCount;

	if (bReleaseMIDICallback)
	{
		ReleaseAkMIDICallback();
	}
}

void AWwUECharacter::OnFireIntervalChanged(float Value)
{
	InitAkMIDIFireInterval();
}

void AWwUECharacter::InitAkMIDIFireInterval()
{
	double FireIntervalMs = FireInterval * 1000;
	double CurrentCallbackTimeMs = (double)CurrentCallbackCount * MsPerCallback;

	// Calculate potential next shot time
	// If the potential next shott happens sooner than our current expected next shot, use the potential instead
	double MaybeTimeUntilNextFireMs = CurrentCallbackTimeMs + FireIntervalMs;
	if (MaybeTimeUntilNextFireMs < TimeUntilNextFireMs)
	{
		TimeUntilNextFireMs = MaybeTimeUntilNextFireMs;
	}

	double PostLengthMs = FMath::Min(MsPerCallback, FireIntervalMs);
	PostLengthSamples = (uint32)((PostLengthMs / MsPerCallback) * SamplesPerCallback);
	PostLengthSamples = FMath::Min(PostLengthSamples, SamplesPerCallback);
}
