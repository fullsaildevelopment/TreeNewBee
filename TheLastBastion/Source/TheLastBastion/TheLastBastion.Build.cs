// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TheLastBastion : ModuleRules
{
	public TheLastBastion(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "OnlineSubsystem", "OnlineSubsystemUtils", "UMG", "Slate", "SlateCore" });

        DynamicallyLoadedModuleNames.Add("OnlineSubsystemNull");
        DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
    }
}
