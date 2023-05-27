// gaming

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "WwUEGameInstance.generated.h"

class UDataTable;

/**
 * 
 */
UCLASS()
class WWUE_API UWwUEGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UDataTable* GetVODataTable() const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	UDataTable* VODataTable;
	
};
