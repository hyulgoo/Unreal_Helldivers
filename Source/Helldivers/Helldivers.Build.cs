// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Helldivers : ModuleRules
{
	public Helldivers(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.Add("Helldivers");

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG", "Niagara", "PhysicsCore", "Landscape", "Foliage" });

		PrivateDependencyModuleNames.AddRange(new string[] {
            "GameFeatures",
            "GameplayAbilities",
            "GameplayTasks",
            "GameplayTags",
            "Landscape"
        });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
