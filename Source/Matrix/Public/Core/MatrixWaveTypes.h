#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "Weapons/WeaponSystem/WeaponBase.h"
#include "MatrixWaveTypes.generated.h"

class AEnemyCharacter;

// === Wave System Types ===

USTRUCT(BlueprintType)
struct FEnemySpawnInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Info")
    TSubclassOf<AEnemyCharacter> EnemyClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Info")
    FGameplayTag SpawnPointTag;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Info")
    int32 SpawnCount = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Info")
    EWeaponType WeaponType = EWeaponType::None;

    FEnemySpawnInfo()
    {
        EnemyClass = nullptr;
        SpawnPointTag = FGameplayTag::EmptyTag;
        SpawnCount = 1;
        WeaponType = EWeaponType::None;
    }
};

USTRUCT(BlueprintType)
struct FWaveData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave Data")
    TArray<FEnemySpawnInfo> SpawnInfos;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave Data")
    float WaveDuration = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave Data")
    bool bIsBossWave = false;

    FWaveData()
    {
        SpawnInfos.Empty();
        WaveDuration = 0.0f;
        bIsBossWave = false;
    }
};

USTRUCT(BlueprintType)
struct FLevelData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Data")
    UDataTable* WaveDataTable;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Data")
    FString LevelName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Data")
    int32 MaxWaves = 1;

    FLevelData()
    {
        WaveDataTable = nullptr;
        LevelName = TEXT("");
        MaxWaves = 1;
    }
};
