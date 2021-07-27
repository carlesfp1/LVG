// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UnderscoreEditor : ModuleRules
{
	public UnderscoreEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivatePCHHeaderFile = "Public/UnderscoreEditor.h";

		PublicIncludePaths.AddRange(
			new string[] {
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"EditorFramework",
				"UnrealEd",
				"AssetTools",
				"ContentBrowser",
				"Underscore",
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"Settings",
				"UnrealEd",
			}
			);
	}
}
