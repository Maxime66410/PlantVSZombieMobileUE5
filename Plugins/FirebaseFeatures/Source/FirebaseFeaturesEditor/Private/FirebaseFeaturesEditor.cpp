// Copright Pandores Marketplace 2024. All Rights Reserved.

#include "FirebaseFeaturesEditor.h"
#include "Editor.h"
#include "Interfaces/IPluginManager.h"
#include "Review/MarketplaceReview.h"
#include "Misc/EngineVersionComparison.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"

#if UE_VERSION_NEWER_THAN(5, 3, 0)
#	include "EngineAnalytics.h"
#	include "IPluginWardenModule.h"
#	include "Account/IPortalUser.h"
#endif

DEFINE_LOG_CATEGORY_STATIC(LogFirebaseFeaturesEditor, Log, All);

static TArray<const TCHAR*> RSyncRequiredConfigs =
{
	TEXT("\nprotect /Plugins/**/Intermediate/**"),
	TEXT("\nprotect /Plugins/**/Binaries/**"),
	TEXT("\nprotect /Binaries/IOS/**"),
	TEXT("\nprotect /Saved/"),
};

/**
* For remote compiling iOS on Windows, makes sure the plugin intermediate folder
* is not removed between builds to prevent losing the GoogleService-Info.plist file.
**/
static void SetupRSync()
{
#if PLATFORM_WINDOWS
	const FString RSyncConfigPath = FPaths::ProjectDir() / TEXT("Build/Rsync/RsyncProject.txt");

	FString RSyncConfigContent;

	if (FPaths::FileExists(RSyncConfigPath))
	{
		FFileHelper::LoadFileToString(RSyncConfigContent, *RSyncConfigContent);
	}

	bool bRequireOverwrite = false;
	for (const auto& Config : RSyncRequiredConfigs)
	{
		if (!RSyncConfigContent.Contains(Config))
		{
			bRequireOverwrite = true;
			RSyncConfigContent += Config;
		}
	}

	if (bRequireOverwrite)
	{
		FFileHelper::SaveStringToFile(RSyncConfigContent, *RSyncConfigPath);
	}
#endif
}


void FFirebaseFeaturesEditorModule::StartupModule()
{
	FEditorDelegates::BeginPIE.AddRaw(this, &FFirebaseFeaturesEditorModule::BeginPIE);

	RequestReview();

	SetupRSync();
}

void FFirebaseFeaturesEditorModule::ShutdownModule()
{
	FEditorDelegates::BeginPIE.RemoveAll(this);
}

void FFirebaseFeaturesEditorModule::BeginPIE(bool bIsSimulating)
{
#if UE_VERSION_NEWER_THAN(5, 3, 0)
	// PluginWarden is brocken.
	return;

	if (!FEngineAnalytics::IsAvailable())
	{
		UE_LOG(LogFirebaseFeaturesEditor, Warning, TEXT("Engine analytics is unavailable."));
	}
	else
	{
		auto Plugin = IPluginManager::Get().FindPlugin("FirebaseFeatures");
		if (Plugin)
		{
			IPluginWardenModule::Get().CheckEntitlementForPlugin(
				FText::FromString(Plugin->GetFriendlyName()),
				TEXT("firebase-features"),
				TEXT("81111c9d88144ab0968fd8953872a3d8"),
				EEntitlementCacheLevelRequest::Disk,
				{},
				IPluginWardenModule::EUnauthorizedErrorHandling::ShowMessageOpenStore,
				[]()
				{
					UE_LOG(LogFirebaseFeaturesEditor, Log, TEXT("Firebase Features use is authorized."));
				});
		}
		else
		{
			UE_LOG(LogFirebaseFeaturesEditor, Warning, TEXT("Failed to find Firebase Features plugin."));
		}
	}
#endif
}

IMPLEMENT_MODULE(FFirebaseFeaturesEditorModule, FirebaseFeaturesEditor);
