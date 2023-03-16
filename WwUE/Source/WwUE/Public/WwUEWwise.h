// gaming

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "../Plugins/Wwise/Source/AkAudio/Classes/AkGameplayStatics.h"
#include "WwUEWwise.generated.h"

class UAkAudioEvent;
class UAkComponent;

DECLARE_LOG_CATEGORY_EXTERN(LogSound, Log, All);

/**
 * 
 */
UCLASS()
class WWUE_API UWwUEWwise : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category = "WwUE Sound")
	static int32 PostEvent(UAkAudioEvent* AkEvent, AActor* Actor, bool bStopWhenAttachedToDestroyed = false);

	UFUNCTION(BlueprintCallable, Category = "WwUE Sound")
	static int32 PostEventOnComponent(UAkAudioEvent* AkEvent, UAkComponent* AkComponent, bool bStopWhenAttachedToDestroyed = false);

	UFUNCTION(BlueprintCallable, Category = "WwUE Sound")
	static bool SetActorRTPCValue(AActor* Actor, FName RTPCName, float Value, int32 InterpolationTimeMs);

	UFUNCTION(BlueprintCallable, Category = "WwUE Sound")
	static bool SetAkComponentRTPCValue(UAkComponent* AkComponent, FString RTPCName, float Value, int32 InterpolationTimeMs);

	UFUNCTION(BlueprintCallable, Category = "WwUE Sound")
	static bool SetGlobalRTPCValue(FName RTPCName, float Value, int32 InterpolationTimeMs);

	UFUNCTION(BlueprintCallable, Category = "WwUE Sound")
	static bool SetActorSwitch(AActor* Actor, FName SwitchGroupName, FName SwitchStateName);

	UFUNCTION(BlueprintCallable, Category = "WwUE Sound")
	static bool SetAkComponentSwitch(UAkComponent* AkComponent, FString SwitchGroupName, FString SwitchStateName);

	UFUNCTION(BlueprintCallable, Category = "WwUE Sound")
	static bool SetState(FName StateGroupName, FName StateName);
};
