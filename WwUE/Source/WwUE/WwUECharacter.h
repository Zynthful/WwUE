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
class AWwUEProjectileWeapon;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAddInventoryItemSignature, AWwUEProjectileWeapon*, InventoryItem);

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

protected:

	UFUNCTION(BlueprintCallable)
	void StartFiring(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable)
	void StopFiring(const FInputActionValue& Value);

	/* ------ Inventory --------------------------------------------------------------------------------- */

public:

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void AddInventoryItem(AWwUEProjectileWeapon* InventoryItem);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void RemoveInventoryItem(AWwUEProjectileWeapon* InventoryItem);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void ClearInventory();

	/* ------ End Inventory --------------------------------------------------------------------------------- */

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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WwUECharacter|Sound")
	UAkComponent* AkComponent;

	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
	AWwUEProjectileWeapon* CurrentWeapon;

public:
	UPROPERTY(BlueprintAssignable, Category = "Firing")
	FOnAddInventoryItemSignature OnAddInventoryItemSignature;
};

