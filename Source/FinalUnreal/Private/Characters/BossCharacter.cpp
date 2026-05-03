// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BossCharacter.h"

#include "BrainComponent.h"
#include "Characters/StatsComponent.h"
#include "Characters/EStat.h"
#include "AI/EnemyAIController.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "BossCharacter.h"

ABossCharacter::ABossCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	StatsComp = CreateDefaultSubobject<UStatsComponent>(TEXT("Stats Component"));

	AIControllerClass = AEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ABossCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (StatsComp)
	{
		StatsComp->OnDeath.AddDynamic(this, &ABossCharacter::HandleDeath);
	}
}

void ABossCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABossCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

float ABossCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float Applied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (bIsDead || !StatsComp) { return 0.f; }

	StatsComp->ReduceHealth(DamageAmount);
	return DamageAmount;
}

void ABossCharacter::HandleDeath()
{
	if (bIsDead) { return; }
	bIsDead = true;

	GetCharacterMovement()->DisableMovement();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (AAIController* AIC = Cast<AAIController>(GetController()))
	{
		AIC->BrainComponent->StopLogic(TEXT("Dead"));
		AIC->UnPossess();
	}

	OnEnemyDied.Broadcast(this);

	SetLifeSpan(2.0f);
}

void ABossCharacter::AttackTarget(AActor* Target)
{
	if (bIsDead || !Target) { return; }

	const float Now = GetWorld()->GetTimeSeconds();
	if (Now - LastAttackTime < AttackCooldown) { return; }

	const float Distance = FVector::Dist(GetActorLocation(), Target->GetActorLocation());
	if (Distance > AttackRange) { return; }

	LastAttackTime = Now;
	UGameplayStatics::ApplyDamage(Target, AttackDamage, GetController(), this, nullptr);
}

void ABossCharacter::DetectPawn(APawn* DetectedPawn, APawn* PawnToDetect)
{
	EEnemyState CurrentState{ static_cast<EEnemyState>(BlackboardComp->GetValueAsEnum(TEXT("CurrentState"))) };

	if (DetectedPawn != PawnToDetect || CurrentState != EEnemyState::Idle) { return; }

	BlackboardComp->SetValueAsEnum(TEXT("CurrentState"), EEnemyState::Range);
}
