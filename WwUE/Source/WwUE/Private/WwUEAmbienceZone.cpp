// gaming


#include "WwUEAmbienceZone.h"
#include "../WwUECharacter.h"
#include "WwUEAmbienceSoundSystem.h"

// Sets default values
AWwUEAmbienceZone::AWwUEAmbienceZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AWwUEAmbienceZone::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWwUEAmbienceZone::OnActorEnter(AActor* Actor)
{
	if (AWwUECharacter* Character = Cast<AWwUECharacter>(Actor))
	{
		if (Character->IsLocallyControlled())
		{
			if (UWwUEAmbienceSoundSystem* AmbienceSoundSystem = GetWorld()->GetSubsystem<UWwUEAmbienceSoundSystem>())
			{
				AmbienceSoundSystem->AddAmbienceZone(this);
			}
		}
	}
}

void AWwUEAmbienceZone::OnActorExit(AActor* Actor)
{
	if (AWwUECharacter* Character = Cast<AWwUECharacter>(Actor))
	{
		if (Character->IsLocallyControlled())
		{
			if (UWwUEAmbienceSoundSystem* AmbienceSoundSystem = GetWorld()->GetSubsystem<UWwUEAmbienceSoundSystem>())
			{
				AmbienceSoundSystem->RemoveAmbienceZone(this);
			}
		}
	}
}

void AWwUEAmbienceZone::OnTriggerPlay(AActor* Actor)
{
}

void AWwUEAmbienceZone::OnTriggerStop(AActor* Actor)
{
}

// Called every frame
void AWwUEAmbienceZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

int AWwUEAmbienceZone::GetPriority()
{
	return Priority;
}

UWwUEAmbienceZoneData* AWwUEAmbienceZone::GetAmbienceData()
{
	return AmbienceData;
}

