// MatrixGameState.cpp
#include "GameFramework/MatrixGameState.h"
#include "Net/UnrealNetwork.h"

AMatrixGameState::AMatrixGameState()
{
	CurrentGameState = EGameState::None;
}

void AMatrixGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMatrixGameState, CurrentGameState);
	DOREPLIFETIME(AMatrixGameState, CurrentWave);
	DOREPLIFETIME(AMatrixGameState, EnemiesRemaining);
}

void AMatrixGameState::SetGameState(EGameState NewState)
{
	if (HasAuthority())
	{
		if (CurrentGameState != NewState)
		{
			CurrentGameState = NewState;
			OnRep_GameState();
		}
	}
}

void AMatrixGameState::OnRep_GameState()
{
	OnGameStateChanged.Broadcast(CurrentGameState);
	UE_LOG(LogTemp, Warning, TEXT("Game State Changed to: %s"), *UEnum::GetValueAsString(CurrentGameState));
}