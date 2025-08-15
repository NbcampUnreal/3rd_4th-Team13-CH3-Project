#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PlayerLocationSharingService.generated.h"

UCLASS()
class MATRIX_API UPlayerLocationSharingService : public UObject
{
	GENERATED_BODY()

public:
	// Singleton instance getter
	static UPlayerLocationSharingService* GetInstance();

	// Call this to update the player's last known position
	void UpdatePlayerLocation(const FVector& NewLocation);

	// Call this to get the player's last known position
	    UFUNCTION(BlueprintCallable, Category = "AI")
    bool GetLastKnownPlayerLocation(FVector& OutLocation);

    void InvalidatePlayerLocation();

	// Is there a valid location known?
	bool HasValidLocation() const;

	// Call this when an AI reaches the location and finds no one
	void InvalidateLocation(const FVector& OldLocation);

private:
	// The actual last known position of the player
	UPROPERTY()
	FVector LastKnownPlayerPosition;

	// Flag to check if the location has ever been set
	bool bIsLocationSet = false;

	// Singleton instance
	static UPlayerLocationSharingService* SingletonInstance;
};
