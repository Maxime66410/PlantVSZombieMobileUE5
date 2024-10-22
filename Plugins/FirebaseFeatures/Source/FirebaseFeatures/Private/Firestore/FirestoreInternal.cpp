// Copyright Pandores Marketplace 2024. All Rights Reserved.

#include "FirestoreInternal.h"
#include "FirebaseFeatures.h"
#include "FirebaseSdk/FirebaseConfig.h"

#if WITH_FIREBASE_FIRESTORE
THIRD_PARTY_INCLUDES_START
#	include "firebase/app.h"
#	include "firebase/log.h"
#	include "firebase/firestore.h"
#	if PLATFORM_IOS
#		import <FirebaseFirestore/FirebaseFirestore.h>
#		import <FirebaseCore/FirebaseCore.h>
#	endif
THIRD_PARTY_INCLUDES_END
#endif

void FirestoreInternal::InitializeFirestore() 
{
#if WITH_FIREBASE_FIRESTORE
	UE_LOG(LogFirestore, Log, TEXT("Initializing Firestore."));

	if (!UFirebaseConfig::Get()->bEnableFirestore)
	{
		UE_LOG(LogFirestore, Log, TEXT("Skipping Firestore initialization as the module is disabled."));
		return;
	}

	firebase::InitResult Result = firebase::InitResult::kInitResultFailedMissingDependency;
	
#if UE_BUILD_SHIPPING || PLATFORM_IOS
	firebase::firestore::Firestore::set_log_level(firebase::LogLevel::kLogLevelInfo);
#else
	firebase::firestore::Firestore::set_log_level(firebase::LogLevel::kLogLevelVerbose);
#endif

#if PLATFORM_IOS
	// iOS 16 Firestore crashes with Debug logging enabled.
	[FIRConfiguration.sharedInstance setLoggerLevel: FIRLoggerLevelInfo];
	[FIRFirestore enableLogging: NO];
#endif

	auto* const App = firebase::App::GetInstance();
	if (!App)
	{
		UE_LOG(LogFirestore, Error, TEXT("Firestore initialization skipped as the Firebase app is invalid."));
		return;
	}

	firebase::firestore::Firestore* const Firestore = firebase::firestore::Firestore::GetInstance(App, &Result);

	switch (Result)
	{
	case firebase::InitResult::kInitResultSuccess:
	{
		UE_LOG(LogFirestore, Log, TEXT("Firestore initialized."));

		auto Settings = Firestore->settings();

		auto* const Config = UFirebaseConfig::Get();

		UE_LOG(LogFirestore, Log, TEXT("Firestore config: { persistence: %d, SSL: %d, Host: %s }."),
			Config->bPersistenceEnabled, Config->bSslEnabled, *Config->Host);

#if WITH_EDITOR
		// If we start in PIE with multiple instances, each instance tries to use persistence and one 
		// will crash. We disable it in Editor to avoid this problem.
		Settings.set_persistence_enabled(false);
#else
		Settings.set_persistence_enabled(Config->bPersistenceEnabled);
#endif
		Settings.set_ssl_enabled(Config->bSslEnabled);

		if (!Config->Host.IsEmpty())
		{
			Settings.set_host(TCHAR_TO_UTF8(*Config->Host));
		}

		Firestore->set_settings(Settings);

	} break;
	case firebase::InitResult::kInitResultFailedMissingDependency:
		UE_LOG(LogFirestore, Error, TEXT("Firestore initialization failed: InitResultFailedMisingDependency."));
		break;

	default:
		UE_LOG(LogFirestore, Warning, TEXT("Firestore initialization unknown result: %d."), (int32)Result);
	};
#else
	UE_LOG(LogFirestore, Log, TEXT("Firestore is disabled, skipping initialization."));
#endif // WITH_FIREBASE_FIRESTORE
}

void FirestoreInternal::TerminateFirestore()
{
#if WITH_FIREBASE_FIRESTORE
	if (firebase::firestore::Firestore::GetInstance())
	{
		delete firebase::firestore::Firestore::GetInstance();
	}
#endif
}

