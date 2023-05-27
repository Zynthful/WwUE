// gaming


#include "Sound/WwUEVOSystem.h"
#include "Sound/VODataTable.h"
#include "AkGameplayTypes.h"
#include "AkAudioDevice.h"
#include "AkComponent.h"
#include "WwUEGameInstance.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogVOSystem);

void UWwUEVOSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Get VODataTable
	UWwUEGameInstance* WwUEGameInstance = Cast<UWwUEGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (WwUEGameInstance != nullptr)
	{
		VODataTable = WwUEGameInstance->GetVODataTable();
	}
}

void UWwUEVOSystem::Deinitialize()
{
	Super::Deinitialize();
}

int32 UWwUEVOSystem::PlayVO(AActor* Actor, FName RowName)
{
	if (Actor == nullptr)
	{
		UE_LOG(LogVOSystem, Error, TEXT("UWwUEVOSystem::PlayVO failed | Actor was nullptr | You must provide an Actor to play the VO on | RowName = [%s]"), *RowName.ToString());
		return AK_INVALID_PLAYING_ID;
	}

	if (VODataTable == nullptr)
	{
		UE_LOG(LogVOSystem, Error, TEXT("UWwUEVOSystem::PlayVO failed | VODataTable was nullptr | RowName = [%s]"), *RowName.ToString());
		return AK_INVALID_PLAYING_ID;
	}
	
	const FVODataTableRow* Row = VODataTable->FindRow<FVODataTableRow>(RowName, TEXT("WwUEVOSystem"));
	if (Row == nullptr)
	{
		UE_LOG(LogVOSystem, Error, TEXT("UWwUEVOSystem::PlayVO failed | RowNotFound | RowName = [%s]"), *RowName.ToString());
		return AK_INVALID_PLAYING_ID;
	}

	if (Row->ExternalSourceCookie.IsEmpty())
	{
		UE_LOG(LogVOSystem, Error, TEXT("UWwUEVOSystem::PlayVO failed | ExternalSourceCookieName was empty | RowName=[%s]"), *RowName.ToString());
		return AK_INVALID_PLAYING_ID;
	}

	if (Row->ExternalSourceFileName.IsEmpty())
	{
		UE_LOG(LogVOSystem, Error, TEXT("UWwUEVOSystem::PlayVO failed | ExternalSourceFileName was empty | RowName=[%s]"), *RowName.ToString());
		return AK_INVALID_PLAYING_ID;
	}

	FAkAudioDevice* AkAudioDevice = FAkAudioDevice::Get();
	if (!ensure(AkAudioDevice != nullptr))
	{
		UE_LOG(LogVOSystem, Error, TEXT("UWwUEVOSystem::PlayVO failed | AkAudioDevice was nullptr | RowName=[%s]"), *RowName.ToString());
		return AK_INVALID_PLAYING_ID;
	}

	// Get AkComponent
	FName AttachPointName = FName("");
	FVector* Location = nullptr;
	UAkComponent* AkComponent = AkAudioDevice->GetAkComponent(Actor->GetRootComponent(), AttachPointName, Location, EAttachLocation::KeepRelativeOffset);
	if (!ensure(AkComponent != nullptr))
	{
		UE_LOG(LogVOSystem, Error, TEXT("UWwUEVOSystem::PlayVO failed | AkComponent was nullptr | RowName=[%s]"), *RowName.ToString());
		return AK_INVALID_PLAYING_ID;
	}

	AkComponent->OcclusionRefreshInterval = 1.f;	// slower than 0.2f default
	AkComponent->StopWhenOwnerDestroyed = true;
	AkComponent->SetOutputBusVolume(Row->OutputBusVolume);

	FAkExternalSourceInfo ExternalSourceInfo = MakeAkExternalSourceInfo(Row);
	TArray<FAkExternalSourceInfo> ExternalSources;
	ExternalSources.Emplace(ExternalSourceInfo);

	// TODO: Implement callback mask
	int32 CallbackMask = 0;

	// TODO: Implement post event callback function OnPostEvent
	FOnAkPostEventCallback PostEventCallback;
	
	AkPlayingID PlayingID = AkComponent->PostAkEventByNameWithDelegate(Row->EventName, CallbackMask, PostEventCallback, ExternalSources);
	if (PlayingID == AK_INVALID_PLAYING_ID)
	{
		UE_LOG(LogVOSystem, Error, TEXT("UWwUEVOSystem::PlayVO failed | PostEvent returned AK_INVALID_PLAYING_ID | Check Wwise profiler log | RowName=[%s]"), *RowName.ToString());
		return AK_INVALID_PLAYING_ID;
	}

	return PlayingID;
}

FAkExternalSourceInfo UWwUEVOSystem::MakeAkExternalSourceInfo(const FVODataTableRow* Row)
{
	FAkExternalSourceInfo ExternalSourceInfo;
	if (Row == nullptr)
	{
		UE_LOG(LogVOSystem, Error, TEXT("UWwUEVOSystem::MakeAkExternalSourceInfo failed | Row was nullptr"));
		ExternalSourceInfo.ExternalSrcName = FString("INVALID_EXTERNAL_SOURCE_NAME");
		ExternalSourceInfo.FileName = FString("INVALID_FILE_NAME");
		return ExternalSourceInfo;
	}

	// TODO: Implement support for different codecs
	ExternalSourceInfo.CodecID = AkCodecId::Vorbis;

	// TODO: Implement support for streaming
	ExternalSourceInfo.IsStreamed = false;

	ExternalSourceInfo.ExternalSrcName = Row->ExternalSourceCookie;
	ExternalSourceInfo.FileName = Row->ExternalSourceFileName;
	ExternalSourceInfo.ExternalSourceAsset = nullptr;

	return ExternalSourceInfo;
}
