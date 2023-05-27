// gaming

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "VODataTable.generated.h"


USTRUCT(BlueprintType)
struct FVODataTableRow : public FTableRowBase
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FString ExternalSourceCookie;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FString ExternalSourceFileName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FString SpeakerName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FString EventName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FName Subtitle;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (UIMin = 0, UIMax = 1, ClampMin = 0, ClampMax = 1))
    float OutputBusVolume = 1.f;
};