// gaming

#pragma once

#include "CoreMinimal.h"
#include "WwUEAmbienceZone.h"
#include "WwUEAmbienceZoneSpline.generated.h"

/**
 * 
 */
UCLASS()
class WWUE_API AWwUEAmbienceZoneSpline : public AWwUEAmbienceZone
{
	GENERATED_BODY()

protected:
	virtual void OnActorEnter(AActor* Actor) override;
	virtual void OnActorExit(AActor* Actor) override;
};
