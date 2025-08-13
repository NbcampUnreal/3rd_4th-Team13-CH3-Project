#include "Items/BaseItem.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

ABaseItem::ABaseItem()
{
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->SetupAttachment(Scene);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(Collision);

	bIsPickedUp = false;
	ItemCount = 0;
}

void ABaseItem::PickedUpItem()
{
	SetActorEnableCollision(false);
	SetActorHiddenInGame(true);
	ItemCount++;
	bIsPickedUp = true;
}

void ABaseItem::UseItem()
{
	ItemCount--;
	bIsPickedUp = false;

	if (ActivateSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ActivateSound, GetActorLocation());
	}

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC)
	{
		FVector PlayerLocation;
		FRotator PlayerRotation;
		PC->GetPlayerViewPoint(PlayerLocation, PlayerRotation);
		
		FVector ParticleLocation = PlayerLocation + PlayerRotation.Vector() * 150.f;
		
		FRotator ParticleRotation = PlayerRotation;
		
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ActivateParticle, ParticleLocation, ParticleRotation);
	}
}

void ABaseItem::SetItemCount(int32 Count)
{
	ItemCount = Count;
}
