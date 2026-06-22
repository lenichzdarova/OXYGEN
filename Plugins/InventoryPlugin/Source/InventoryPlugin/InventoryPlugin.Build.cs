// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class InventoryPlugin : ModuleRules
{
	public InventoryPlugin(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"ApplicationCore",
				"AIModule",
				"AudioMixer",
				"AudioModulation",
				"CommonGame",
				"CommonInput",
				"CommonLoadingScreen",
				"CommonUI",
				"CommonUser",
				"CoreOnline",
				"CoreUObject",
				"DeveloperSettings",
				"Engine",
				"EngineSettings",
				"EnhancedInput",
				"GameFeatures",
				"GameplayAbilities",
				"GameplayMessageNodes",
				"GameplayMessageRuntime",
				"GameplayTags",
				"GameplayTasks",
				"GameSubtitles",
				"IrisCore",
				"ModularGameplay",
				"ModularGameplayActors",
				"NetCore",
				"Niagara",
				"OnlineBase",
				"OnlineSubsystem",
				"OnlineSubsystemUtils",
				"PhysicsCore",
				"RHI",
				"SignificanceManager",
				"Slate",
				"SlateCore",
				"UMG"
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
