// Copyright Pandores Marketplace 2024. All Rights Reserved.

using UnrealBuildTool;
using System.IO;
using System;

#if UE_5_0_OR_LATER
using EpicGames.Core;
#else
using Tools.DotNETCommon;
#endif

public class FirebaseFeatures : ModuleRules
{
	string FirebaseSdkRoot;

	bool bIsPluginInEngineDir;

	public FirebaseFeatures(ReadOnlyTargetRules Target) : base(Target)
	{ 
		// Path pointing to the SDK's root.
		FirebaseSdkRoot = Path.Combine(PluginDirectory, "Source/ThirdParty/firebase_cpp_sdk/");

		// We need to read from Engine's config for modules' options.
		DirectoryReference ProjectDirRef = (Target != null && Target.ProjectFile != null) ? DirectoryReference.FromFile(Target.ProjectFile) : null;
		EngineConfig = ConfigCache.ReadHierarchy(ConfigHierarchyType.Engine, ProjectDirRef, Target.Platform);

		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		// Unreal Engine Dependencies
		PublicDependencyModuleNames .AddRange(new string[] { "Core" });
		PrivateDependencyModuleNames.AddRange(new string[] { "CoreUObject", "Engine", "OpenSSL", "Json" });

		// Config values for modules taken from DefaultEngine.ini.
		string FirebaseModulesSection = "/Script/FirebaseFeatures.FirebaseConfig";
		bEnableAuth			= GetFeaturesEnabledStatus(FirebaseModulesSection, "bEnableAuth",			true);
		bEnableAppCheck		= GetFeaturesEnabledStatus(FirebaseModulesSection, "bEnableAppCheck",		true);
		bEnableFirestore	= GetFeaturesEnabledStatus(FirebaseModulesSection, "bEnableFirestore",		true);
		bEnableDatabase		= GetFeaturesEnabledStatus(FirebaseModulesSection, "bEnableDatabase",		true);
		bEnableAdMob		= GetFeaturesEnabledStatus(FirebaseModulesSection, "bEnableAdMob",			true);
		bEnableAnalytics	= GetFeaturesEnabledStatus(FirebaseModulesSection, "bEnableAnalytics",		true);
		bEnableFunctions	= GetFeaturesEnabledStatus(FirebaseModulesSection, "bEnableFunctions",		true);
		bEnableMessaging	= GetFeaturesEnabledStatus(FirebaseModulesSection, "bEnableMessaging",		true);
		bEnableRemoteConfig	= GetFeaturesEnabledStatus(FirebaseModulesSection, "bEnableRemoteConfig",	true);
		bEnableStorage		= GetFeaturesEnabledStatus(FirebaseModulesSection, "bEnableStorage",		true);
		bEnableCrashlytics	= GetFeaturesEnabledStatus(FirebaseModulesSection, "bEnableCrashlytics",	true);
		bEnableDynamicLinks	= GetFeaturesEnabledStatus(FirebaseModulesSection, "bEnableDynamicLinks",	true);
		bEnablePerformance	= GetFeaturesEnabledStatus(FirebaseModulesSection, "bEnablePerformance",	true);

		// Prints useful information about the environment of the user.
		CheckEnvironment(Target);

		// Editor-only config.
		if (Target.Type == TargetType.Editor)
		{
			PrivateDependencyModuleNames.AddRange(new string[] 
			{ 
				"Slate", 
				"SlateCore", 
				"UnrealEd", 
				"PropertyEditor", 
				"Settings", 
				"Projects",
                "GameProjectGeneration",
            });
		}

		// Android Build configuration.
		if (Target.Platform == UnrealTargetPlatform.Android)
		{
			// Engine dependencies.
			PrivateDependencyModuleNames.Add("Launch");

			// Firebase includes.
			PublicIncludePaths.Add(Path.Combine(FirebaseSdkRoot, "11.8.0/inc/"));

			// Android's UPL file.
			AdditionalPropertiesForReceipt.Add("AndroidPlugin", Path.Combine(ModuleDirectory, "FirebaseSdk.android.upl.xml"));

			LinkFirebaseLibraries();
			
			AddRuntimeDependency(Path.Combine(GetServicesDir(Target), "google-services.json"));
		}

		// IOS Build Configuration.
		else if (Target.Platform == UnrealTargetPlatform.IOS)
		{
			// Required system frameworks.
			PublicFrameworks.AddRange(new string[]
			{
				//"UniformTypeIdentifiers",
				"CoreFoundation",
				"Foundation",
				"Security",
				"GSS",
				"SystemConfiguration",
				"JavaScriptCore",
				"LocalAuthentication",
                "AuthenticationServices",
            });

			SetupSwiftLinkage();

            // Firebase includes.
            PublicIncludePaths.Add(Path.Combine(FirebaseSdkRoot, "11.9.0/inc/"));

            // iOS UPL
            string PluginPath = Utils.MakePathRelativeTo(ModuleDirectory, Target.RelativeEnginePath);
            AdditionalPropertiesForReceipt.Add("IOSPlugin", Path.Combine(PluginPath, "FirebaseSdk.ios.upl.xml"));

            // Firebase frameworks

            if (bEnableCrashlytics)
            {
				AddFrameworkChecked("FirebaseCrashlytics");
            }

            if (bEnablePerformance)
            { 
				AddFrameworkChecked("FirebasePerformance");
				AddFrameworkChecked("GTMSessionFetcher");
				AddFrameworkChecked("FirebaseABTesting");
                AddFrameworkChecked("FirebaseSharedSwift");
                AddFrameworkChecked("FirebaseRemoteConfig");
			}

			if (true)
			{
				AddFirebaseFramework("firebase_app", DefineOverride: "firebase");
                AddFrameworkChecked("FBLPromises");
                AddFrameworkChecked("Promises");
                AddFrameworkChecked("FirebaseSessions");
				AddFrameworkChecked("FirebaseCore");
				AddFrameworkChecked("FirebaseCoreInternal");
				AddFrameworkChecked("FirebaseCoreExtension");
				AddFrameworkChecked("GoogleUtilities");
				AddFrameworkChecked("GoogleDataTransport");
				AddFrameworkChecked("FirebaseInstallations");
				AddFrameworkChecked("FirebaseAppCheck");
				AddFrameworkChecked("FirebaseAppCheckInterop");
				AddFrameworkChecked("AppCheckCore");
				AddFrameworkChecked("nanopb");
				AddFrameworkChecked("leveldb");
			}

			if (bEnableDatabase)
			{
				AddFirebaseFramework("firebase_database");
				AddFrameworkChecked("FirebaseDatabase");
				AddFrameworkChecked("leveldb");
			}

			if (bEnableStorage)
			{
				AddFirebaseFramework("firebase_storage");
				AddFrameworkChecked("FirebaseStorage");
				AddFrameworkChecked("GTMSessionFetcher");
			}

			if (bEnableDynamicLinks)
			{
				AddFirebaseFramework("firebase_dynamic_links");
				AddFrameworkChecked("FirebaseDynamicLinks");
			}

			if (bEnableFunctions)
			{
				AddFirebaseFramework("firebase_functions");
				AddFrameworkChecked("FirebaseFunctions");
				AddFrameworkChecked("FirebaseSharedSwift");
				AddFrameworkChecked("GTMSessionFetcher");
			}

			if (bEnableFirestore)
			{
				AddFirebaseFramework("firebase_firestore");
				AddFrameworkChecked("FirebaseFirestore");
				AddFrameworkChecked("FirebaseFirestoreInternal");
				AddFrameworkChecked("openssl_grpc");
				AddFrameworkChecked("absl");
				AddFrameworkChecked("grpcpp", "Resources/gRPCCertificates-Cpp.bundle");
				AddFrameworkChecked("grpc");
				AddFrameworkChecked("leveldb");
			}

			// Auth Objective-C dependencies.
			if (bEnableAuth)
			{
				AddFirebaseFramework("firebase_auth");
				AddFrameworkChecked("FirebaseAuth");
				AddFrameworkChecked("FirebaseAuthInterop");
				AddFrameworkChecked("GoogleSignIn");
				AddFrameworkChecked("AppAuth");
                AddFrameworkChecked("GTMAppAuth");
				AddFrameworkChecked("GTMSessionFetcher");
				AddFrameworkChecked("RecaptchaInterop");
            }

			// Messaging Objective-C dependencies.
			if (bEnableMessaging)
			{
				AddFirebaseFramework("firebase_messaging");
				AddFrameworkChecked("FirebaseMessaging");
				AddFrameworkChecked("FirebaseMessagingInterop");
				AddFrameworkChecked("FirebaseABTesting");
				AddFrameworkChecked("FirebaseInAppMessaging", "Resources/InAppMessagingDisplayResources.bundle");

				PublicFrameworks.Add("UserNotifications");
			}

			// AdMob Objective-C dependencies.
			if (bEnableAdMob)
			{
				AddFirebaseFramework("firebase_admob", true);
				AddFirebaseFramework("firebase_gma", true);
				AddFrameworkChecked("GoogleMobileAds");
				AddFrameworkChecked("AppAuth");
				AddFrameworkChecked("GTMAppAuth");
				AddFrameworkChecked("GTMSessionFetcher");
				AddFrameworkChecked("UserMessagingPlatform");
			}

			// Analytics Objective-C dependencies.
			if (bEnableAnalytics)
			{
				AddFirebaseFramework("firebase_analytics");
				AddFrameworkChecked("FirebaseAnalytics");
				AddFrameworkChecked("FirebaseCore");
				AddFrameworkChecked("FirebaseInstallations");
				AddFrameworkChecked("GoogleAppMeasurement");
				AddFrameworkChecked("GoogleAppMeasurementIdentitySupport");
				AddFrameworkChecked("GoogleDataTransport");
				AddFrameworkChecked("GoogleUtilities");
				AddFrameworkChecked("nanopb");
            }

			if (bEnableRemoteConfig)
			{
				AddFirebaseFramework("firebase_remote_config");
				AddFrameworkChecked("FirebaseRemoteConfig");
				AddFrameworkChecked("FirebaseABTesting");
                AddFrameworkChecked("FirebaseSharedSwift");
            }		
        }

		// Win64 Build Configuration.
		else if (Target.Platform == UnrealTargetPlatform.Mac)
		{
			// Required system frameworks.
			PublicFrameworks.AddRange(new string[]
			{
				//"UniformTypeIdentifiers", 
				"UserNotifications",
				"CoreFoundation",
				"Foundation",
				"Security",
				"Kerberos",
				"GSS",
				"SystemConfiguration",
				"JavaScriptCore",
				"LocalAuthentication",
            });

            // Firebase includes.
            PublicIncludePaths.Add(Path.Combine(FirebaseSdkRoot, "11.1.0/inc/"));

            LinkFirebaseLibraries();

			AddRuntimeDependency(Path.Combine(GetServicesDir(Target), "google-services.json"));
		}
		else if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			// System libraries.
			AddSystemLibrary("userenv.lib");
			AddSystemLibrary("psapi.lib");

            // Firebase includes.
            PublicIncludePaths.Add(Path.Combine(FirebaseSdkRoot, "11.1.0/inc/"));

            LinkFirebaseLibraries();

			AddRuntimeDependency(Path.Combine(GetServicesDir(Target), "google-services.json"));
		}

		// Linux Build Configuration.
		else if (Target.Platform == UnrealTargetPlatform.Linux)
		{
			// System libraries.
			AddSystemLibrary("pthread");
			AddSystemLibrary("c");

			// Engine's Libcurl
			AddEngineThirdPartyPrivateStaticDependencies(Target, "libcurl");

			// Firebase's includes.
			PublicIncludePaths.Add(Path.Combine(FirebaseSdkRoot, "11.9.0/inc"));

			// Links Firebase C++ libraries.
            LinkFirebaseLibraries();

			// Additional linux-only libraries.
			string LibRoot = GetFirebaseSdkRelativeLibsDir(Target)[0];
			string DependenciesRoot = Path.Combine(FirebaseSdkRoot, LibRoot, "dependencies");
            foreach (string Dependency in Directory.GetFiles(DependenciesRoot))
			{
				PublicAdditionalLibraries.Add(Dependency);
			}
        }
		else
        {
			Console.Error.WriteLine("This platform is not supported by Firebase Features.");
        }
		
		// Module includes
		PublicIncludePaths .Add(Path.Combine(ModuleDirectory, "Public"));
		PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Private"));
		
		AddRuntimeDependency(Path.Combine(GetServicesDir(Target), "google-services.json"));

		// Firebase Performance doesn't have a C++ library.
		// We manually define it here.
		if (bEnablePerformance)
        {
			PublicDefinitions.Add("WITH_FIREBASE_PERFORMANCE=1");
		}
		if (bEnableCrashlytics)
        {
			PublicDefinitions.Add("WITH_FIREBASE_CRASHLYTICS=1");
        }
	}

	private void LinkFirebaseLibraries()
	{ 
		AddFirebaseLibrary("firebase_app");
		if (bEnableAdMob)
		{
			AddFirebaseLibrary("firebase_admob", true);
			AddFirebaseLibrary("firebase_gma", true);
		}
		if (bEnableAppCheck)
		{
            AddFirebaseLibrary("firebase_app_check", true);
        }
		if (bEnableAnalytics)
		{ 
			AddFirebaseLibrary("firebase_analytics");
		}
		if (bEnableAuth)
        {
			AddFirebaseLibrary("firebase_auth");
        }
		if (bEnableDatabase)
        {
			AddFirebaseLibrary("firebase_database");
        }
		if (bEnableDynamicLinks)
        {
			AddFirebaseLibrary("firebase_dynamic_links");
        } 
		if (bEnableFirestore)
        {
			AddFirebaseLibrary("firebase_firestore");
        }
		if (bEnableFunctions)
        {
			AddFirebaseLibrary("firebase_functions");
        }
		if (bEnableMessaging)
        {
			AddFirebaseLibrary("firebase_messaging");
        }
		if (bEnableRemoteConfig)
        {
			AddFirebaseLibrary("firebase_remote_config");
        }
		if (bEnableStorage)
        {
			AddFirebaseLibrary("firebase_storage");
        }
	}

	private void AddSystemLibrary(string Name)
	{
#if UE_4_22_OR_LATER
		PublicSystemLibraries.Add(Name);
#else
		PublicAdditionalLibraries.Add(Name);
#endif
	}

	private string[] GetFirebaseSdkRelativeLibsDir(ReadOnlyTargetRules Target)
	{
		const string LibsDir = "libs/";

		// Removed for UE5 support.
		//if (Target.Platform == UnrealTargetPlatform.Win32)
		//{
		//	return new string[]
		//	{
		//		LibsDir + "/windows/VS2015/MD/x86/" 
		//			+ (bIsDebugBuild ? "Debug" : "Release")
		//	};
		//}

		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			return new string[]
			{
				"11.1.0/libs/windows/VS2019/MD/x64/"
			};
		}

		if (Target.Platform == UnrealTargetPlatform.Mac)
		{
            string MacSDKRoot = "11.1.0/libs/";

            return new string[]
			{
				MacSDKRoot + "darwin/universal",
			};
		}

		if (Target.Platform == UnrealTargetPlatform.Android)
		{
			string AndroidSDKRoot = "11.8.0/libs/android/";

			return new string[]
			{
				// AndroidSDKRoot + "armeabi-v7a/c++",
				AndroidSDKRoot + "arm64-v8a",
				// AndroidSDKRoot + "x86/c++",
				AndroidSDKRoot + "x86_64",
			};
		}

		if (Target.Platform == UnrealTargetPlatform.IOS)
		{
			return new string[]
			{
				LibsDir + "ios/universal/",
			};
		}

		if (Target.Platform == UnrealTargetPlatform.Linux)
		{
            string LinuxSDKRoot = "11.9.0/lib/";
            return new string[]
            {
				LinuxSDKRoot + "linux/x86_64_fpic/"
            };
		}

		throw new System.Exception("The plugin doesn't contain the C++ libraries for this platform.");
	}

	private string GetPlatformLibPrefix()
	{
		return
			Target.Platform == UnrealTargetPlatform.Win64 ?
				"" : "lib";
	}

	private string GetPlatformLibPostfix()
	{
		return
			Target.Platform == UnrealTargetPlatform.Win64 ?
				"lib" : "a";
	}

	private bool IsDebugBuild()
    {
		return false;
	}

#if UE_4_22_OR_LATER
	private Framework CreateFramework(string Name, string Path, string Bundle)
    {
		return new Framework
		(
			Name, Path, Bundle
		);
	}
#else
	private UEBuildFramework CreateFramework(string Name, string Path, string Bundle)
	{
		return new UEBuildFramework
		(
			Name, Path, Bundle
		);
	}
#endif

	private void AddFrameworkChecked(string Name, string Bundle = null)
    {
		string FrameworkRoot	 = Path.Combine(FirebaseSdkRoot, "11.9.0/lib/ios/");
		string FrameworkLocation = Path.Combine(FrameworkRoot, Name + ".framework.zip");

		if (!File.Exists(FrameworkLocation))
        {
			System.Console.Error.WriteLine("Failed to find iOS framework \"" + Name + "\" for Firebase as file " + FrameworkLocation + " doesn't exist.");
			return;
		}

		PublicAdditionalFrameworks.Add(CreateFramework
		(
			Name, FrameworkLocation, Bundle
		));
	}

	private void AddRuntimeDependency(string Path, string Target = null, StagedFileType Type = StagedFileType.UFS)
    {
		if (Target == null)
        {
			RuntimeDependencies.Add(Path, Type);
		}
		else
        {
			RuntimeDependencies.Add(Target, Path, Type);
		}
    }

	private string GetServicesDir(ReadOnlyTargetRules Target)
    {
		return "$(ProjectDir)/Services/";
	}

	private bool GetFeaturesEnabledStatus(string Section, string Key, bool bDefault = false)
	{
		// We want to link everything when the plugin is not built directly.
		if (bIsPluginInEngineDir)
        {
			return true;
        }
		
		bool bValue = bDefault;
		if (EngineConfig.TryGetValue(Section, Key, out bValue))
		{
			return bValue;
		}
		return bDefault;
	}

	private void AddFirebaseLibrary(string LibraryName, bool bAllowFailure = false)
    {
		string[] LibrariesDir = GetFirebaseSdkRelativeLibsDir(Target);
		foreach (string LibraryDir in LibrariesDir)
        {
			string LibLocation = Path.Combine(FirebaseSdkRoot, LibraryDir, GetPlatformLibPrefix() + LibraryName + "." + GetPlatformLibPostfix());
			if (!File.Exists(LibLocation))
			{
				if (!bAllowFailure)
                {
					System.Console.Error.WriteLine("Required library " + LibLocation + " doesn't exist.");
                }
			}
			else
			{
				PublicAdditionalLibraries.Add(LibLocation);
				PublicDefinitions.Add("WITH_" + LibraryName.ToUpper() + "=1");
			}
		}
	}

	private void SetupSwiftLinkage()
	{
        string SDKROOT = Utils.RunLocalProcessAndReturnStdOut("/usr/bin/xcrun", "--sdk iphoneos --show-sdk-path");
        PublicSystemLibraryPaths.Add(SDKROOT + "/usr/lib/swift");
        PublicSystemLibraryPaths.Add(SDKROOT + "../../../../../../Toolchains/XcodeDefault.xctoolchain/usr/lib/swift/iphoneos");
        PublicSystemLibraryPaths.Add(SDKROOT + "../../../../../../Toolchains/XcodeDefault.xctoolchain/usr/lib/swift-5.0/iphoneos");
        PublicSystemLibraryPaths.Add(SDKROOT + "../../../../../../Toolchains/XcodeDefault.xctoolchain/usr/lib/swift-5.5/iphoneos");
    }

	private void AddFirebaseFramework(string Library, bool bAllowFail = false, string DefineOverride = null, bool bForceLib = false)
    {
		bool bUseLibraries = true;

        string FrameworkName = Path.Combine(FirebaseSdkRoot, "11.9.0/lib/ios/", Library + ".framework.zip");
        string LibraryName   = Path.Combine(FirebaseSdkRoot, "11.9.0/lib/ios/", "lib" + Library + ".a");
		 
        if (!bUseLibraries || bForceLib)
        {
			if (!bAllowFail || File.Exists(FrameworkName))
            {
                PublicAdditionalLibraries.Add(FrameworkName);
                PublicDefinitions.Add("WITH_" + (DefineOverride != null ? DefineOverride : Library).ToUpper() + "=1");
            }
        }
        else
        {
            if (!bAllowFail || File.Exists(LibraryName))
            {
				PublicAdditionalLibraries.Add(LibraryName);
				PublicDefinitions.Add("WITH_" + (DefineOverride != null ? DefineOverride : Library).ToUpper() + "=1");
            }
        }
	}

	private void CheckEnvironment(ReadOnlyTargetRules Target)
    {
		// Engine plugin's binaries aren't rebuilt. If we don't link frameworks here, we'll get unresolved
		// external symbols. We just notice the users that they need to actually rebuild the plugin to disable 
		// the module.

		bIsPluginInEngineDir = PluginDirectory.Contains("Engine/Plugins") || PluginDirectory.Contains("Engine\\Plugins");

		// Change true to false if you don't want these messages to appear anymore.
		bool bShowSumary = true;
		if (bShowSumary)
        {
            System.Console.WriteLine("Firebase Features plugin path is " + PluginDirectory);

            if (bIsPluginInEngineDir)
			{
					System.Console.WriteLine("\nFirebase Features is being included in a build as an Engine plugin. " +
						"It means that its binaries won't be rebuilt and thus, C++ changes of disabling or enabling " +
						"features won't take effect. If you want to remove C++ libraries from the build, you have to " +
						"rebuild the plugin.\n");
			}
			else
			{
				// Summary of what's linked or not to help troubleshooting.
				System.Console.WriteLine("Firebase Features is being included in a build as a Project plugin. Linkage will follow the plugin's configuration: ");
				PrintModuleLinkageStatus("Auth",			bEnableAuth);
				PrintModuleLinkageStatus("App Check",		bEnableAppCheck);
				PrintModuleLinkageStatus("Firestore",		bEnableFirestore);
				PrintModuleLinkageStatus("Database",		bEnableDatabase);
				PrintModuleLinkageStatus("AdMob",			bEnableAdMob);
				PrintModuleLinkageStatus("Analytics",		bEnableAnalytics);
				PrintModuleLinkageStatus("Functions",		bEnableFunctions);
				PrintModuleLinkageStatus("Messaging",		bEnableMessaging);
				PrintModuleLinkageStatus("Remote Config",	bEnableRemoteConfig);
				PrintModuleLinkageStatus("Storage",			bEnableStorage);
				PrintModuleLinkageStatus("Crashlytics",		bEnableCrashlytics);
				PrintModuleLinkageStatus("Dynamic Links",	bEnableDynamicLinks);
				PrintModuleLinkageStatus("Performance",		bEnablePerformance);
			}
		}
	}

	private string GetValidPathFrom(string[] Potentials)
	{
		foreach (string Potential in Potentials)
		{
			if (File.Exists(Potential))
			{
				return Potential;
			}
		}

		return null;
	}

	private void PrintModuleLinkageStatus(string Module, bool bLinked)
    { 
		System.Console.WriteLine(("  * Firebase " + Module + ": ").PadRight(30) + (bLinked ? "Linked" : "Disabled"));
	}

	private ConfigHierarchy EngineConfig;

	private bool bEnableAuth;
	private bool bEnableAppCheck;
    private bool bEnableFirestore;
	private bool bEnableDatabase;
	private bool bEnableAdMob;
	private bool bEnableAnalytics;
	private bool bEnableFunctions;
	private bool bEnableMessaging;
	private bool bEnableRemoteConfig;
	private bool bEnableStorage;
	private bool bEnableCrashlytics;
	private bool bEnableDynamicLinks;
	private bool bEnablePerformance;

}

