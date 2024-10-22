// Copyright Pandores Marketplace 2024. All Rights Reserved.

using UnrealBuildTool;
using System.IO;
using System;

public class FirebaseFeaturesEditor : ModuleRules
{
	public FirebaseFeaturesEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		// Unreal Engine Dependencies
		PrivateDependencyModuleNames.AddRange(new string[] 
		{ 
			"Core",
			"CoreUObject", 
			"Engine", 
			"Slate", 
			"SlateCore", 
			"UnrealEd", 
			"PropertyEditor", 
			"Settings", 
			"Projects",
			"GameProjectGeneration",
            "DesktopPlatform",
        });

#if UE_5_3_OR_LATER
		PrivateDependencyModuleNames.AddRange(new string[] 
		{
            "PluginWarden",
			"PortalServices",
			"LauncherPlatform",
        });
#endif
	}
}

