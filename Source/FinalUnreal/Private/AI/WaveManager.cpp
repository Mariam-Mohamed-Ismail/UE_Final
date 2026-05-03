// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/WaveManager.h"
#include "Characters/BossCharacter.h"
#include "Characters/StatsComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

AWaveManager::AWaveManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AWaveManager::BeginPlay()
{
	Super::BeginPlay();

	// Hook player death.
	if (APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0))
	{
		if (UStatsComponent* PlayerStats = PlayerPawn->FindComponentByClass<UStatsComponent>())
		{
			PlayerStats->OnDeath.AddDynamic(this, &AWaveManager::HandlePlayerDied);
		}
	}

	StartWave(0);
}

void AWaveManager::StartWave(int32 WaveIndex)
{
	if (bGameOver) { return; }
	if (!WaveSizes.IsValidIndex(WaveIndex) || !EnemyClass)
	{
		OnAllWavesComplete.Broadcast();
		return;
	}

	CurrentWaveIndex = WaveIndex;
	const int32 NumToSpawn = WaveSizes[WaveIndex];
	AliveEnemiesInWave = 0;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	const FVector Origin = GetActorLocation();

	for (int32 i = 0; i < NumToSpawn; ++i)
	{
		const float Angle = (2.f * PI * i) / FMath::Max(NumToSpawn, 1);
		const FVector Offset(FMath::Cos(Angle) * SpawnRadius, FMath::Sin(Angle) * SpawnRadius, 0.f);
		const FVector SpawnLoc = Origin + Offset;

		ABossCharacter* Enemy = GetWorld()->SpawnActor<ABossCharacter>(EnemyClass, SpawnLoc, FRotator::ZeroRotator, Params);
		if (Enemy)
		{
			Enemy->OnEnemyDied.AddDynamic(this, &AWaveManager::HandleEnemyDied);
			++AliveEnemiesInWave;
		}
	}

	OnWaveStarted.Broadcast(GetCurrentWaveNumber());
}

void AWaveManager::HandleEnemyDied(ABossCharacter* DeadEnemy)
{
	if (bGameOver) { return; }

	--AliveEnemiesInWave;
	if (AliveEnemiesInWave > 0) { return; }

	const int32 NextIndex = CurrentWaveIndex + 1;
	if (!WaveSizes.IsValidIndex(NextIndex))
	{
		bGameOver = true;
		OnAllWavesComplete.Broadcast();
		return;
	}

	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateUObject(this, &AWaveManager::StartWave, NextIndex), DelayBetweenWaves, false);
}

void AWaveManager::HandlePlayerDied()
{
	bGameOver = true;
	OnPlayerDied.Broadcast();
}
