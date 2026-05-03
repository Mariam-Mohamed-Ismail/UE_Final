// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

UCLASS()
class FINALUNREAL_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	AEnemyAIController();

	// Blackboard key name for the player target (Object key).
	static const FName PlayerKeyName;

protected:
	virtual void OnPossess(APawn* InPawn) override;
};
