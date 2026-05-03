// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/EnemyAIController.h"
#include "Characters/BossCharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

const FName AEnemyAIController::PlayerKeyName = TEXT("PlayerKey");

AEnemyAIController::AEnemyAIController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ABossCharacter* Boss = Cast<ABossCharacter>(InPawn);
	if (!Boss || !Boss->BehaviorTreeAsset) { return; }

	UBlackboardComponent* BlackboardComponent;
	UseBlackboard(Boss->BehaviorTreeAsset->BlackboardAsset, BlackboardComponent);

	if (APawn* Player = UGameplayStatics::GetPlayerPawn(this, 0))
	{
		BlackboardComponent->SetValueAsObject(PlayerKeyName, Player);
	}

	RunBehaviorTree(Boss->BehaviorTreeAsset);
}
