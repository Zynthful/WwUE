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

	// Call OnFireIntervalChanged delegates
	SetFireInterval(FireInterval);
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

void AWwUECharacter::SetFireInterval(float NewInterval)
{
	FireInterval = FMath::Max(0.01f, NewInterval);
	if (OnFireIntervalChangedSignature.IsBound())
	{
		OnFireIntervalChangedSignature.Broadcast(FireInterval);
	}
}

float AWwUECharacter::GetFireInterval()
{
	return FireInterval;
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

	OnFireIntervalFinished();
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
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Purple, FString::Printf(TEXT("STOP FIRING, NumShots = %d, NumPosts = %d"), NumShotsFiredThisStream, PostCounter));
	}

	bIsFiring = false;

	FTimerManager& WorldTimerManager = GetWorldTimerManager();
	if (ensure(WorldTimerManager.IsTimerActive(TH_FireInterval)))
	{
		WorldTimerManager.ClearTimer(TH_FireInterval);
	}

	NumShotsFiredThisStream = 0;
}

void AWwUECharacter::OnFireIntervalFinished()
{
	FireSingle();

	FTimerDelegate TD_FireInterval;
	TD_FireInterval.BindUFunction(this, GET_FUNCTION_NAME_CHECKED(AWwUECharacter, OnFireIntervalFinished));
	GetWorldTimerManager().SetTimer(TH_FireInterval, TD_FireInterval, FireInterval, false);
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

void AWwUECharacter::AddInventoryItem(AWwUEProjectileWeapon* InventoryItem)
{

}

void AWwUECharacter::RemoveInventoryItem(AWwUEProjectileWeapon* InventoryItem)
{
}

void AWwUECharacter::ClearInventory()
{
}
