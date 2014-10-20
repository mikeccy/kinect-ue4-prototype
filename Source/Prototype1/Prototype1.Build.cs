// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Prototype1 : ModuleRules
{
	public Prototype1(TargetInfo Target)
	{
        // Speedup build
        MinFilesUsingPrecompiledHeaderOverride = 1;
        bFasterWithoutUnity = true;

        // Dependency
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

        // Kinect lib
        string SDKDIR = Utils.ResolveEnvironmentVariable("%KINECTSDK20_DIR%").Replace("\\", "/");

        if (Target.Platform == UnrealTargetPlatform.Win64 || Target.Platform == UnrealTargetPlatform.Win32)
        {
            PrivateIncludePaths.Add(SDKDIR + "inc/");

            string PlatformPath = (Target.Platform == UnrealTargetPlatform.Win64) ? "x64/" : "x86/";
            string LibPath = SDKDIR + "Lib/" + PlatformPath;

            PublicLibraryPaths.Add(LibPath);
            PublicAdditionalLibraries.Add(LibPath + "Kinect20.lib");
        }

	}
}
