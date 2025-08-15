#include "AI/Services/PlayerLocationSharingService.h"

// Initialize singleton instance to null
UPlayerLocationSharingService* UPlayerLocationSharingService::SingletonInstance = nullptr;

UPlayerLocationSharingService* UPlayerLocationSharingService::GetInstance()
{
	if (!SingletonInstance)
	{
		// Create a new instance and keep it alive
		SingletonInstance = NewObject<UPlayerLocationSharingService>();
		SingletonInstance->AddToRoot(); // Prevents garbage collection
	}
	return SingletonInstance;
}

void UPlayerLocationSharingService::UpdatePlayerLocation(const FVector& NewLocation)
{
	LastKnownPlayerPosition = NewLocation;
	bIsLocationSet = true;
}

bool UPlayerLocationSharingService::GetLastKnownPlayerLocation(FVector& OutLocation)
{
	if (bIsLocationSet)
	{
		OutLocation = LastKnownPlayerPosition;
		return true;
	}
	return false;
}

bool UPlayerLocationSharingService::HasValidLocation() const
{
	return bIsLocationSet;
}

void UPlayerLocationSharingService::InvalidateLocation(const FVector& OldLocation)
{
	// Only invalidate if the location is still the same one we went to.
	// This prevents an AI from invalidating a fresh location update.
	if (bIsLocationSet && LastKnownPlayerPosition.Equals(OldLocation, 100.0f))
	{
		bIsLocationSet = false;
	}
}
