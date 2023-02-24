// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class WwUE : ModuleRules
{
	public WwUE(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput", "AkAudio" });
	}
}
