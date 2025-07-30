// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class Matrix : ModuleRules
{
	public Matrix(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "NavigationSystem", "AIModule", "GameplayTasks", "UMG", "Slate", "SlateCore" });

		// 게임플레이 어빌리티 시스템(GAS) 사용 시 활성화
		// PublicDependencyModuleNames.Add("GameplayAbilities");
		
		PrivateDependencyModuleNames.AddRange(new string[] { "Gameplay", "UI", "GameMode", "AI" });
		
		// 헤더 검색 경로 설정
		PublicIncludePaths.AddRange(new string[] {
			Path.Combine(ModuleDirectory, "Core/Public"),
			Path.Combine(ModuleDirectory, "Gameplay/Public"),
			Path.Combine(ModuleDirectory, "UI/Public"),
			Path.Combine(ModuleDirectory, "GameMode/Public"),
			Path.Combine(ModuleDirectory, "InventorySystem/Public"),
			Path.Combine(ModuleDirectory, "AI/Public")
		});
		
		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
