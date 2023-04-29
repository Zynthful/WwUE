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
	void StartFiring(const FInputActionValue& Value);
	void StopFiring(const FInputActionValue& Value);

	UFUNCTION()
	void FireSingle();

	static void StaticFireSingleMIDICallback(AK::IAkGlobalPluginContext* Context, AkGlobalCallbackLocation Location, void* Cookie);
	void ObjectFireSingleMIDICallback(AK::IAkGlobalPluginContext* Context, AkGlobalCallbackLocation Location, void* Cookie);

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
};

