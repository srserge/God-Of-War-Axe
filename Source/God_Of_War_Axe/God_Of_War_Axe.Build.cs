// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class God_Of_War_Axe : ModuleRules
{
	public God_Of_War_Axe(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "UMG", "EnhancedInput", "Niagara", "GameplayTasks", "GeometryCollectionEngine" });

		PrivateDependencyModuleNames.AddRange(new string[] { "AnimGraphRuntime" });
		
		PublicIncludePaths.AddRange(
			new string[]
			{
				"God_Of_War_Axe",
				"God_Of_War_Axe/Public",
				"God_Of_War_Axe/Public/Characters",
				"God_Of_War_Axe/Public/Characters/Animations",
				"God_Of_War_Axe/Public/Pawns",
				"God_Of_War_Axe/Public/AI",				
				"God_Of_War_Axe/Public/Actors",
				"God_Of_War_Axe/Public/Modes", 
				"God_Of_War_Axe/Public/Combat",
				"God_Of_War_Axe/Public/UI",
				"God_Of_War_Axe/Public/GameplayComponents", 
				"God_Of_War_Axe/Public/Interfaces", 
			});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
