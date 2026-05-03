// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/CombatComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "Interfaces/MainPlayer.h"
#include "Combat/TraceComponent.h"
#include "TimerManager.h"
#include "Engine/World.h"
// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	CharacterRef = GetOwner<ACharacter>();
	
}


// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UCombatComponent::ComboAttack()
{
	if(CharacterRef->Implements<UMainPlayer>())
	{
		IMainPlayer* IPlayerRef{ Cast<IMainPlayer>(CharacterRef) };

		if(IPlayerRef && !IPlayerRef->HasEnoughStamina(StaminaCost))
		{
			return;
		}
	}

	if(!bCanAttack){
		return;
	}

	bCanAttack = false;

	if (AttackAnimations.Num() > 0 && AttackAnimations.IsValidIndex(ComboCounter) && AttackAnimations[ComboCounter])
	{
		CharacterRef->PlayAnimMontage(AttackAnimations[ComboCounter]);
	}

	ComboCounter++;

	int MaxCombo{ AttackAnimations.Num() };
	if (MaxCombo > 0)
	{
		ComboCounter = UKismetMathLibrary::Wrap(ComboCounter, -1, (MaxCombo - 1));
	}
	else
	{
		ComboCounter = 0;
	}

	OnAttackPerformedDelegat.Broadcast(StaminaCost);

	// Drive the trace window from input (independent of animation notifies).
	if (UTraceComponent* Trace = CharacterRef->FindComponentByClass<UTraceComponent>())
	{
		Trace->bIsAttacking = true;
		Trace->HandleResetAttack(); // clear ignore list at the start of each swing

		FTimerHandle CloseHandle;
		TWeakObjectPtr<UTraceComponent> WeakTrace(Trace);
		GetWorld()->GetTimerManager().SetTimer(
			CloseHandle,
			FTimerDelegate::CreateLambda([this, WeakTrace]()
			{
				if (WeakTrace.IsValid())
				{
					WeakTrace->bIsAttacking = false;
					WeakTrace->HandleResetAttack();
				}
				bCanAttack = true;
			}),
			0.4f,
			false
		);
	}
	else
	{
		// No trace component — still allow next attack.
		FTimerHandle CloseHandle;
		GetWorld()->GetTimerManager().SetTimer(CloseHandle, FTimerDelegate::CreateLambda([this]() { bCanAttack = true; }), 0.4f, false);
	}
}

void UCombatComponent::HandleResetAttack()
{
	bCanAttack = true;

}

