// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/StatsComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
UStatsComponent::UStatsComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
	// Seed default values — these can still be overridden per-Blueprint
	Stats.Add(EStat::Health, 100.f);
	Stats.Add(EStat::MaxHealth, 100.f);
	Stats.Add(EStat::Stamina, 100.f);
	Stats.Add(EStat::MaxStamina, 100.f);
}

void UStatsComponent::BeginPlay()
{ 
	Super::BeginPlay();
}

void UStatsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UStatsComponent::ReduceHealth(float Amount)
{
	if (bHasDied) { return; }
	if (!Stats.Contains(EStat::Health) || !Stats.Contains(EStat::MaxHealth)) { return; }

	Stats[EStat::Health] -= Amount;
	Stats[EStat::Health] = UKismetMathLibrary::FClamp(
		Stats[EStat::Health],
		0.f,
		Stats[EStat::MaxHealth]
	);

	OnHealthChanged.Broadcast(Stats[EStat::Health], Stats[EStat::MaxHealth]);

	if (Stats[EStat::Health] <= 0.f)
	{
		bHasDied = true;
		OnDeath.Broadcast();
	}
}

void UStatsComponent::ReduceStamina(float Amount)
{
	if (!Stats.Contains(EStat::Stamina) || !Stats.Contains(EStat::MaxStamina)) { return; }

	Stats[EStat::Stamina] -= Amount;
	Stats[EStat::Stamina] = UKismetMathLibrary::FClamp(
		Stats[EStat::Stamina],
		0.f,
		Stats[EStat::MaxStamina]
	);

	bCanRegen = false;

	FLatentActionInfo FunctionInfo{
		0,
		100,
		TEXT("EnableRegen"),
		this
	};

	UKismetSystemLibrary::RetriggerableDelay(
		GetWorld(),
		StaminaDelayDuration,
		FunctionInfo
	);
}

void UStatsComponent::RegenStamina(float Amount)
{
	if (!bCanRegen) { return; }

	Stats[EStat::Stamina] = UKismetMathLibrary::FInterpTo_Constant(
		Stats[EStat::Stamina],
		Stats[EStat::MaxStamina],
		GetWorld()->GetDeltaSeconds(),
		StaminaRegenRate
	);
}

void UStatsComponent::EnableRegen()
{
	bCanRegen = true;
}

float UStatsComponent::GetHealth() const
{
	return Stats.Contains(EStat::Health) ? Stats[EStat::Health] : 0.f;
}

float UStatsComponent::GetMaxHealth() const
{
	return Stats.Contains(EStat::MaxHealth) ? Stats[EStat::MaxHealth] : 0.f;
}
