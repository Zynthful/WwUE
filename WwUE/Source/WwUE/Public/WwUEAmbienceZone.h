// gaming

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WwUEAmbienceZone.generated.h"

class UWwUEAmbienceZoneData;

UENUM(BlueprintType)
enum EAmbiencePlaySettings
{
	OnEnter,
	OnAudible,
};

UENUM(BlueprintType)
enum EAmbienceStopSettings
{
	OnExit,
	OnNotAudible,
};

UCLASS()
class WWUE_API AWwUEAmbienceZone : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWwUEAmbienceZone();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void OnActorEnter(AActor* Actor);

	UFUNCTION(BlueprintCallable)
	void OnActorExit(AActor* Actor);

	UFUNCTION(BlueprintCallable)
	void OnTriggerPlay(AActor* Actor);

	UFUNCTION(BlueprintCallable)
	void OnTriggerStop(AActor* Actor);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	int GetPriority();
	UWwUEAmbienceZoneData* GetAmbienceData();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UWwUEAmbienceZoneData* AmbienceData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<EAmbiencePlaySettings> PlaySettings = EAmbiencePlaySettings::OnEnter;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<EAmbienceStopSettings> StopSettings = EAmbienceStopSettings::OnExit;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int Priority = 0;
};
