// gaming


#include "WwUEAmbienceZone.h"
#include "../WwUECharacter.h"
#include "WwUEAmbienceSoundSystem.h"
#include "AkComponent.h"

// Sets default values
AWwUEAmbienceZone::AWwUEAmbienceZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AkComponent = CreateDefaultSubobject<UAkComponent>(TEXT("Ak Component"));
	AkComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

// Called when the game starts or when spawned
void AWwUEAmbienceZone::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWwUEAmbienceZone::OnActorEnter(AActor* Actor)
{
	ActorsInside.Add(Actor);

	if (PlaySettings == EAmbiencePlaySettings::OnEnter)
	{
		OnTriggerPlay(Actor);
	}
}

void AWwUEAmbienceZone::OnActorExit(AActor* Actor)
{
	ActorsInside.Remove(Actor);

	if (StopSettings == EAmbienceStopSettings::OnExit)
	{
		OnTriggerStop(Actor);
	}
}

void AWwUEAmbienceZone::OnTriggerPlay(AActor* Actor)
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

void AWwUEAmbienceZone::OnTriggerStop(AActor* Actor)
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

