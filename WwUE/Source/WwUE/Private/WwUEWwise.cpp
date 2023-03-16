// gaming


#include "WwUEWwise.h"
#include "AkGameplayStatics.h"
#include "AkComponent.h"

DEFINE_LOG_CATEGORY(LogSound);

int32 UWwUEWwise::PostEvent(UAkAudioEvent* AkEvent, AActor* Actor, bool bStopWhenAttachedToDestroyed)
{
	if (AkEvent == nullptr)
	{
		UE_LOG(LogSound, Error, TEXT("SetActorRTPCValue failed | AkEvent = nullptr"));
		return AK_INVALID_PLAYING_ID;
	}

	if (Actor == nullptr)
	{
		UE_LOG(LogSound, Error, TEXT("SetActorRTPCValue failed | Actor = nullptr"));
		return AK_INVALID_PLAYING_ID;
	}

	FOnAkPostEventCallback NullCallback;
	return UAkGameplayStatics::PostEvent(AkEvent, Actor, 0, NullCallback, bStopWhenAttachedToDestroyed);
}

int32 UWwUEWwise::PostEventOnComponent(UAkAudioEvent* AkEvent, UAkComponent* AkComponent, bool bStopWhenAttachedToDestroyed)
{
	if (AkEvent == nullptr)
	{
		UE_LOG(LogSound, Error, TEXT("SetActorRTPCValue failed | AkEvent = nullptr"));
		return AK_INVALID_PLAYING_ID;
	}

	if (AkComponent == nullptr)
	{
		UE_LOG(LogSound, Error, TEXT("SetActorRTPCValue failed | AkComponent = nullptr"));
		return AK_INVALID_PLAYING_ID;
	}

	FOnAkPostEventCallback NullCallback;
	return AkComponent->PostAkEvent(AkEvent, 0, NullCallback, TEXT(""));
}

bool UWwUEWwise::SetActorRTPCValue(AActor* Actor, FName RTPCName, float Value, int32 InterpolationTimeMs)
{
	if (Actor == nullptr)
	{
		UE_LOG(LogSound, Error, TEXT("SetActorRTPCValue failed | Actor = nullptr"));
		return false;
	}

	UAkGameplayStatics::SetRTPCValue(nullptr, Value, InterpolationTimeMs, Actor, RTPCName);
	return true;
}

bool UWwUEWwise::SetAkComponentRTPCValue(UAkComponent* AkComponent, FString RTPCName, float Value, int32 InterpolationTimeMs)
{
	if (AkComponent == nullptr)
	{
		UE_LOG(LogSound, Error, TEXT("SetAkComponentRTPCValue failed | AkComponent = nullptr"));
		return false;
	}

	AkComponent->SetRTPCValue(nullptr, Value, InterpolationTimeMs, RTPCName);
	return true;
}

bool UWwUEWwise::SetGlobalRTPCValue(FName RTPCName, float Value, int32 InterpolationTimeMs)
{
	UAkGameplayStatics::SetRTPCValue(nullptr, Value, InterpolationTimeMs, nullptr, RTPCName);
	return true;
}

bool UWwUEWwise::SetActorSwitch(AActor* Actor, FName SwitchGroupName, FName SwitchStateName)
{
	if (Actor == nullptr)
	{
		UE_LOG(LogSound, Error, TEXT("SetActorRTPCValue failed | Actor = nullptr"));
		return false;
	}

	UAkGameplayStatics::SetSwitch(nullptr, Actor, SwitchGroupName, SwitchStateName);
	return true;
}

bool UWwUEWwise::SetAkComponentSwitch(UAkComponent* AkComponent, FString SwitchGroupName, FString SwitchStateName)
{
	if (AkComponent == nullptr)
	{
		UE_LOG(LogSound, Error, TEXT("SetAkComponentRTPCValue failed | AkComponent = nullptr"));
		return false;
	}

	AkComponent->SetSwitch(nullptr, SwitchGroupName, SwitchStateName);
	return true;
}

bool UWwUEWwise::SetState(FName StateGroupName, FName StateName)
{
	UAkGameplayStatics::SetState(nullptr, StateGroupName, StateName);
	return true;
}