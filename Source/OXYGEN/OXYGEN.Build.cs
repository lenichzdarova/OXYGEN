// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class OXYGEN : ModuleRules
{
	public OXYGEN(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"CommonGame",
			"CommonUser",
			"CommonUI",
			"Core",
			"CoreUObject",
			"DeveloperSettings",
			"Engine",
			"EnhancedInput",
			"GameFeatures",
			"GameplayAbilities",
			"GameplayMessageNodes",
			"GameplayMessageRuntime",
			"GameplayTags",
			"GameplayTasks",
			"InputCore",
			"IrisCore",
			"ModularGameplay",
			"ModularGameplayActors",
			"NetCommon",
			"NetCore",
			"Networking",
			"Niagara",
			"OnlineSubsystem",
			"OnlineSubsystemUtils",
			"Slate",
			"SlateCore",
			"UMG"
		});

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Steam OSS is loaded at runtime when DefaultPlatformService=Steam
		DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
		
		PublicIncludePaths.AddRange(new string[]
		{
			System.IO.Path.Combine(ModuleDirectory, "AbilitySystem"),
			System.IO.Path.Combine(ModuleDirectory, "Character"),
			System.IO.Path.Combine(ModuleDirectory, "Core"),
			System.IO.Path.Combine(ModuleDirectory, "Equipment"),
			System.IO.Path.Combine(ModuleDirectory, "Interaction"),
			System.IO.Path.Combine(ModuleDirectory, "Inventory"),
			System.IO.Path.Combine(ModuleDirectory, "SaveSystem"),
			System.IO.Path.Combine(ModuleDirectory, "System"),
			System.IO.Path.Combine(ModuleDirectory, "UI"),
		});
	}
}
