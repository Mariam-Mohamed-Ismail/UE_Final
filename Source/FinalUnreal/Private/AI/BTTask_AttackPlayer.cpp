// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/BTTask_AttackPlayer.h"
#include "AI/EnemyAIController.h"
#include "Characters/BossCharacter.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_AttackPlayer::UBTTask_AttackPlayer()
{
	NodeName = TEXT("Attack Player");
}

EBTNodeResult::Type UBTTask_AttackPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	if (!AIC) { return EBTNodeResult::Failed; }

	ABossCharacter* Boss = Cast<ABossCharacter>(AIC->GetPawn());
	if (!Boss) { return EBTNodeResult::Failed; }

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) { return EBTNodeResult::Failed; }

	AActor* Target = Cast<AActor>(BB->GetValueAsObject(AEnemyAIController::PlayerKeyName));
	if (!Target) { return EBTNodeResult::Failed; }

	Boss->AttackTarget(Target);
	return EBTNodeResult::Succeeded;
}
