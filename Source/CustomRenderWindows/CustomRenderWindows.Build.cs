// Some copyright should be here...

using UnrealBuildTool;
using System.IO;

public class CustomRenderWindows : ModuleRules
{
	public CustomRenderWindows(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
#if PLATFORM_WINDOWS
		PublicDelayLoadDLLs.Add("sstools.dll");  //ƻ����֧���ӳټ���
#endif

		// �� sstools.dll �� sstools.lib �ļ����Ƶ������ ThirdParty �ļ�����
		string PluginPath = Utils.MakePathRelativeTo(ModuleDirectory, Target.RelativeEnginePath);
		AdditionalPropertiesForReceipt.Add("CopyPluginLibraries", Path.Combine(PluginPath, "ThirdParty"));

		// �� ThirdParty �ļ�����ӵ��������Դ�б���
		PublicIncludePaths.Add(Path.Combine(PluginPath, "ThirdParty"));
		PublicAdditionalLibraries.Add(Path.Combine(PluginPath, "ThirdParty", "sstools.lib"));

		RuntimeDependencies.Add(Path.Combine(PluginPath, "ThirdParty/sstools.dll"));


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
				"ImageWrapper", "RenderCore",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore", "RHI"
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
