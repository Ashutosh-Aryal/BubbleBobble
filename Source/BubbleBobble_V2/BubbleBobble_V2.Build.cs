// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BubbleBobble_V2 : ModuleRules
{
	public BubbleBobble_V2(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "Paper2D", "AIModule", "GameplayTasks", "UMG", "Kismet" });
	}
}
