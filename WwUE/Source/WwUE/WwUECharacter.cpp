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

	bIsFiring = true;

	FireSingle();

	FTimerDelegate TD_FireInterval;
	TD_FireInterval.BindUFunction(this, GET_FUNCTION_NAME_CHECKED(AWwUECharacter, FireSingle));
	GetWorldTimerManager().SetTimer(TH_FireInterval, TD_FireInterval, FireInterval, true);
}

void AWwUECharacter::StopFiring(const FInputActionValue& Value)
{
	if (!bIsFiring)
	{
		return;
	}

	UE_LOG(LogCharacter, Verbose, TEXT("StopFiring = %s"), *GetName());

	bIsFiring = false;

	FTimerManager& WorldTimerManager = GetWorldTimerManager();
	if (ensure(WorldTimerManager.IsTimerActive(TH_FireInterval)))
	{
		WorldTimerManager.ClearTimer(TH_FireInterval);
	}
}

void AWwUECharacter::FireSingle()
{
	UE_LOG(LogCharacter, Verbose, TEXT("FireSingle = %s"), *GetName());

	// this throws an exception : (
	//AK::SoundEngine::RegisterGlobalCallback(&StaticFireSingleMIDICallback, AkGlobalCallbackLocation_PreProcessMessageQueueForRender);

	if (FireSingleAkEvent)
	{
		FMIDIEvent MIDIEvent;
		MIDIEvent.Channel = 0;
		MIDIEvent.NoteType = EMIDINoteType::NoteOn;
		MIDIEvent.ParamOne = 60;
		MIDIEvent.ParamTwo = 96;

		uint64 SampleTick = UAkMIDIGameplayStatics::GetSampleTick();
		GEngine->AddOnScreenDebugMessage(1372843, 2.f, FColor::Red, FString::Printf(TEXT("SampleTick = %llu"), SampleTick));

		uint32 CurrentCallbackCount = 0;

		// Calculate milliseconds per callback
		// Multiply by 1000 to convert seconds -> milliseconds
		AkAudioSettings AudioSettings = UAkMIDIGameplayStatics::GetAudioSettings();
		double MsPerCallback = ((uint64)AudioSettings.uNumSamplesPerFrame / (uint64)AudioSettings.uNumSamplesPerSecond) * 1000;

		uint32 SamplesPerCallback = AudioSettings.uNumSamplesPerFrame;

		//double InterPostTimeMs;
		//double PostLengthMs = FMath::Min(MsPerCallback, InterPostTimeMs);
		//uint32 PostLengthSamples = (uint32)((PostLengthMs / MsPerCallback) * SamplesPerCallback);

		//double CurrentTimeMs = (double)CurrentCallbackCount * MsPerCallback;

		UWwUEWwise::PostMIDIOnEvent(FireSingleAkEvent, AkComponent, MIDIEvent);
	}
}

void AWwUECharacter::StaticFireSingleMIDICallback(AK::IAkGlobalPluginContext* Context, AkGlobalCallbackLocation Location, void* Cookie)
{

}

void AWwUECharacter::ObjectFireSingleMIDICallback(AK::IAkGlobalPluginContext* Context, AkGlobalCallbackLocation Location, void* Cookie)
{
	uint8 Note = 70;
	uint8 Channel = 1;
	uint32 StartOffsetSamples = 0;
	//UWwUEWwise::PostMIDIOnEvent(FireSingleAkEvent, AkComponent, Note, Channel, StartOffsetSamples);
}