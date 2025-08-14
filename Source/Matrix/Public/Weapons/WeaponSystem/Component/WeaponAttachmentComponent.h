#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponAttachmentComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MATRIX_API UWeaponAttachmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWeaponAttachmentComponent();

	UFUNCTION(BlueprintCallable, Category = "Attachment")
	void AttachToOwner(USceneComponent* CharacterMesh, AActor* WeaponActor, UStaticMeshComponent* WeaponMeshComp);
	UFUNCTION(BlueprintCallable, Category = "Attachment")
	void DetachFromOwner(AActor* WeaponActor, UStaticMeshComponent* WeaponMeshComp);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attachment")
	FName AttachSocket;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attachment")
	FRotator MeshInitialRotation;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attachment")
	FVector MeshInitialScale;
};
