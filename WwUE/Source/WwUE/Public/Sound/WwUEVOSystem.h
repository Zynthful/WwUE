// gaming

#pragma once

#include "CoreMinimal.h"
#include "WwUEVOSystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogVOSystem, Log, All);

class UDataTable;
struct FVODataTableRow;
struct FAkExternalSourceInfo;

/**
 * 
 */
UCLASS()
class WWUE_API UWwUEVOSystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	// UWorldSubsystem Begin
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// UWorldSubsystem End
	
public:
	UFUNCTION(BlueprintCallable)
	int32 PlayVO(AActor* Actor, FName RowName);

	// Cached VODataTable
	UDataTable* VODataTable;

	FAkExternalSourceInfo MakeAkExternalSourceInfo(const FVODataTableRow* Row);
};
