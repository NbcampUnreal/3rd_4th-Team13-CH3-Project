#pragma once

#include "AttributeSet.h"
#include "MatrixAttributeSet.generated.h"

UCLASS()
class MATRIX_API UMatrixAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UMatrixAttributeSet();

	// Health
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	static FGameplayAttribute GetHealthAttribute();
	void SetHealth(float Health);
	float GetHealth() const;

	// MaxHealth
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	static FGameplayAttribute GetMaxHealthAttribute();
	void SetMaxHealth(float MaxHealth);
	float GetMaxHealth() const;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);

	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
};
