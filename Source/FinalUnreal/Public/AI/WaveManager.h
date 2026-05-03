// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WaveManager.generated.h"

class ABossCharacter;
class UStatsComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaveStartedSignature, int32, WaveNumber);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAllWavesCompleteSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerDiedSignature);

UCLASS()
class FINALUNREAL_API AWaveManager : public AActor
{
	GENERATED_BODY()

public:	
	AWaveManager();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves")
	TSubclassOf<ABossCharacter> EnemyClass;

	// Number of enemies to spawn per wave (size = number of waves).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves")
	TArray<int32> WaveSizes = { 3, 5 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves")
	float SpawnRadius = 800.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves")
	float DelayBetweenWaves = 2.f;

	UPROPERTY(BlueprintAssignable)
	FOnWaveStartedSignature OnWaveStarted;

	UPROPERTY(BlueprintAssignable)
	FOnAllWavesCompleteSignature OnAllWavesComplete;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerDiedSignature OnPlayerDied;

	UFUNCTION(BlueprintPure)
	int32 GetCurrentWaveNumber() const { return CurrentWaveIndex + 1; }

protected:
	virtual void BeginPlay() override;

	void StartWave(int32 WaveIndex);

	UFUNCTION()
	void HandleEnemyDied(ABossCharacter* DeadEnemy);

	UFUNCTION()
	void HandlePlayerDied();

	int32 CurrentWaveIndex = -1;
	int32 AliveEnemiesInWave = 0;
	bool bGameOver = false;
};
