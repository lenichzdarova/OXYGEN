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
			"CommonInput",  
			"Core",
			"CoreUObject",
			"DeveloperSettings",
			"Engine",
			"EngineSettings",       
			"EnhancedInput",
			"GameFeatures",
			"GameplayAbilities",
			"GameplayMessageRuntime",
			"GameplayTags",
			"GameplayTasks",
			"GameSettings",
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
			"UMG",
			"ReplicationGraph",
			"ExternalRpcRegistry",
			"Gauntlet",
			"GameSubtitles",
			"CommonLoadingScreen",
			"AsyncMixin",
			"AudioModulation",
			"AudioMixer",           
			"UIExtension",
			"Hotfix",
			"GameplayDebugger",
			"SignificanceManager",
			"ControlFlows",
			"HttpServer",
			"RHI",
			"AIModule",             
			"Json",                 
			"PropertyPath",         
			"PhysicsCore",          
			"ApplicationCore",
			"OnlineSubsystemUtils",
			"CoreOnline"
		});

		// Steam OSS is loaded at runtime when DefaultPlatformService=Steam
		DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
		
		PublicIncludePaths.AddRange(new string[]
		{
			ModuleDirectory,
			System.IO.Path.Combine(ModuleDirectory, "AbilitySystem"),
			System.IO.Path.Combine(ModuleDirectory, "Animation"),
			System.IO.Path.Combine(ModuleDirectory, "Audio"),
			System.IO.Path.Combine(ModuleDirectory, "Camera"),
			System.IO.Path.Combine(ModuleDirectory, "Character"),
			System.IO.Path.Combine(ModuleDirectory, "Cosmetics"),
			System.IO.Path.Combine(ModuleDirectory, "Development"),
			System.IO.Path.Combine(ModuleDirectory, "Feedback"),
			System.IO.Path.Combine(ModuleDirectory, "GameFeatures"),
			System.IO.Path.Combine(ModuleDirectory, "GameModes"),
			System.IO.Path.Combine(ModuleDirectory, "Hotfix"),
			System.IO.Path.Combine(ModuleDirectory, "Input"),
			System.IO.Path.Combine(ModuleDirectory, "Interaction"),
			System.IO.Path.Combine(ModuleDirectory, "Inventory"),
			System.IO.Path.Combine(ModuleDirectory, "Messages"),
			System.IO.Path.Combine(ModuleDirectory, "Performance"),
			System.IO.Path.Combine(ModuleDirectory, "Physics"),
			System.IO.Path.Combine(ModuleDirectory, "Player"),
			System.IO.Path.Combine(ModuleDirectory, "Settings"),
			System.IO.Path.Combine(ModuleDirectory, "System"),
			System.IO.Path.Combine(ModuleDirectory, "Tests"),
			System.IO.Path.Combine(ModuleDirectory, "UI"),
		});
	}
}
