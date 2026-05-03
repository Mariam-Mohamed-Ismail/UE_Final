// Fill out your copyright notice in the Description page of Project Settings.

#include "FinalUnrealGameMode.h"
#include "AI/WaveManager.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "EngineUtils.h"

AFinalUnrealGameMode::AFinalUnrealGameMode()
{
}

void AFinalUnrealGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Spawn HUD on local player and force clean gameplay input state.
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		if (HUDWidgetClass)
		{
			HUDInstance = CreateWidget<UUserWidget>(PC, HUDWidgetClass);
			if (HUDInstance)
			{
				HUDInstance->AddToViewport();
			}
		}
		FInputModeGameOnly GameMode;
		PC->SetInputMode(GameMode);
		PC->SetShowMouseCursor(false);
	}

	// Find the first WaveManager in the level and bind its delegates.
	for (TActorIterator<AWaveManager> It(GetWorld()); It; ++It)
	{
		AWaveManager* WM = *It;
		if (!WM) { continue; }
		WM->OnWaveStarted.AddDynamic(this, &AFinalUnrealGameMode::HandleWaveStarted);
		WM->OnAllWavesComplete.AddDynamic(this, &AFinalUnrealGameMode::HandleAllWavesComplete);
		WM->OnPlayerDied.AddDynamic(this, &AFinalUnrealGameMode::HandlePlayerDied);
		break;
	}
}

void AFinalUnrealGameMode::HandleWaveStarted(int32 WaveNumber)
{
	CurrentWave = WaveNumber;
}

void AFinalUnrealGameMode::HandleAllWavesComplete()
{
	ShowEndScreen(WinScreenClass);
}

void AFinalUnrealGameMode::HandlePlayerDied()
{
	ShowEndScreen(LoseScreenClass);
}

void AFinalUnrealGameMode::ShowEndScreen(TSubclassOf<UUserWidget> WidgetClass)
{
	if (HUDInstance)
	{
		HUDInstance->RemoveFromParent();
		HUDInstance = nullptr;
	}

	if (!WidgetClass) { return; }

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC) { return; }

	if (UUserWidget* W = CreateWidget<UUserWidget>(PC, WidgetClass))
	{
		W->AddToViewport(100);
		PC->SetShowMouseCursor(true);

		FInputModeUIOnly Mode;
		Mode.SetWidgetToFocus(W->TakeWidget());
		PC->SetInputMode(Mode);

		UGameplayStatics::SetGamePaused(this, true);
	}
}
