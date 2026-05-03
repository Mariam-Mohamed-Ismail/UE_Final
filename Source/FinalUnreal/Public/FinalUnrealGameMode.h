// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FinalUnrealGameMode.generated.h"

class UUserWidget;

UCLASS()
class FINALUNREAL_API AFinalUnrealGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AFinalUnrealGameMode();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="UI")
	TSubclassOf<UUserWidget> HUDWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="UI")
	TSubclassOf<UUserWidget> WinScreenClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="UI")
	TSubclassOf<UUserWidget> LoseScreenClass;

	UPROPERTY(BlueprintReadOnly, Category="Waves")
	int32 CurrentWave = 1;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleWaveStarted(int32 WaveNumber);

	UFUNCTION()
	void HandleAllWavesComplete();

	UFUNCTION()
	void HandlePlayerDied();

	void ShowEndScreen(TSubclassOf<UUserWidget> WidgetClass);

	UPROPERTY()
	UUserWidget* HUDInstance;
};
