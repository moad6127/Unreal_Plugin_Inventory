// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Plugin_Inventory : ModuleRules
{
	public Plugin_Inventory(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"Plugin_Inventory",
			"Plugin_Inventory/Variant_Platforming",
			"Plugin_Inventory/Variant_Platforming/Animation",
			"Plugin_Inventory/Variant_Combat",
			"Plugin_Inventory/Variant_Combat/AI",
			"Plugin_Inventory/Variant_Combat/Animation",
			"Plugin_Inventory/Variant_Combat/Gameplay",
			"Plugin_Inventory/Variant_Combat/Interfaces",
			"Plugin_Inventory/Variant_Combat/UI",
			"Plugin_Inventory/Variant_SideScrolling",
			"Plugin_Inventory/Variant_SideScrolling/AI",
			"Plugin_Inventory/Variant_SideScrolling/Gameplay",
			"Plugin_Inventory/Variant_SideScrolling/Interfaces",
			"Plugin_Inventory/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
