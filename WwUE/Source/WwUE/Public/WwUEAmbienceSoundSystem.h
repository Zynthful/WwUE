// gaming

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "WwUEAmbienceSoundSystem.generated.h"

class AWwUEAmbienceZone;

/**
 * 
 */
UCLASS()
class WWUE_API UWwUEAmbienceSoundSystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	virtual void AddAmbienceZone(AWwUEAmbienceZone* Zone);
	virtual void RemoveAmbienceZone(AWwUEAmbienceZone* Zone);
	virtual void SetAmbience(AWwUEAmbienceZone* Zone);
	virtual void ClearAmbience();

private:
	AWwUEAmbienceZone* GetHighestPriorityAmbienceZone();

protected:
	UPROPERTY(Transient)
	TSet<AWwUEAmbienceZone*> InAmbienceZones;

	UPROPERTY(Transient)
	AWwUEAmbienceZone* CurrentAmbienceZone;
};
