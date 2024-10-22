// Copright Pandores Marketplace 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FirebaseSdk/FirebaseErrors.h"
#include "Modules/ModuleManager.h"
#include "Runtime/Launch/Resources/Version.h"
#include "Misc/EngineVersionComparison.h"


THIRD_PARTY_INCLUDES_START
#if PLATFORM_WINDOWS
#	include "Windows/AllowWindowsPlatformTypes.h"
#	if UE_VERSION_OLDER_THAN(5, 4, 0)
#		include "Windows/PreWindowsApi.h"
#	endif
#endif
#include "firebase/version.h"
#if PLATFORM_WINDOWS
#	if UE_VERSION_OLDER_THAN(5, 4, 0)
#		include "Windows/PostWindowsApi.h"
#	endif
#	include "Windows/HideWindowsPlatformTypes.h"
#endif
THIRD_PARTY_INCLUDES_END

#define FIREBASE_SDK_SMALLER_THAN(Major, Minor, Patch) \
	((FIREBASE_VERSION_MAJOR < Major) || \
	(FIREBASE_VERSION_MAJOR == Major && FIREBASE_VERSION_MINOR < Minor) || \
	(FIREBASE_VERSION_MAJOR == Major && FIREBASE_VERSION_MINOR == Minor && FIREBASE_VERSION_REVISION < Patch))

#include "Interfaces/IAnalyticsProviderModule.h"

#include "FirebaseFeatures.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogFirebaseSdk,			 Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogAdMob,				 Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogFirebaseAuth,		 Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogFirebaseDatabase,	 Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogFirebaseDynamicLinks, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogFirebaseFunctions,	 Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogFirestore,			 Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogFirebaseMessaging,	 Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogFirebaseRemoteConfig, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogFirebaseStorage,		 Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogFirebaseCrashlytics,  Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogFirebasePerformance,  Log, All);

// Force disable AdMob in shipping builds on desktop.
#if UE_BUILD_SHIPPING
#	if PLATFORM_DESKTOP
#		ifdef WITH_FIREBASE_GMA
#			undef WITH_FIREBASE_GMA
#			define WITH_FIREBASE_GMA 0
#		endif
#		ifdef WITH_FIREBASE_ADMOB
#			undef WITH_FIREBASE_ADMOB
#			define WITH_FIREBASE_ADMOB 0
#		endif
#	endif
#endif

#ifndef WITH_FIREBASE_CRASHLYTICS
#	define WITH_FIREBASE_CRASHLYTICS 0
#endif

#ifndef WITH_FIREBASE_APP_CHECK
#	define WITH_FIREBASE_APP_CHECK 0
#endif

#ifndef WITH_FIREBASE_PERFORMANCE
#	define WITH_FIREBASE_PERFORMANCE 0
#endif

#ifndef WITH_FIREBASE_AUTH
#	define WITH_FIREBASE_AUTH 0
#endif

#ifndef WITH_FIREBASE_MESSAGING
#	define WITH_FIREBASE_MESSAGING 0
#endif

#ifndef WITH_FIREBASE_GMA
#	define WITH_FIREBASE_GMA 0
#endif

#ifndef WITH_FIREBASE_ADMOB
#	define WITH_FIREBASE_ADMOB WITH_FIREBASE_GMA
#endif

#ifndef WITH_FIREBASE_FIRESTORE
#	define WITH_FIREBASE_FIRESTORE 0
#endif

#ifndef WITH_FIREBASE_REMOTE_CONFIG
#	define WITH_FIREBASE_REMOTE_CONFIG 0
#endif

#ifndef WITH_FIREBASE_STORAGE
#	define WITH_FIREBASE_STORAGE 0
#endif

#ifndef WITH_FIREBASE_DYNAMIC_LINKS
#	define WITH_FIREBASE_DYNAMIC_LINKS 0
#endif

#ifndef WITH_FIREBASE_ANALYTICS
#	define WITH_FIREBASE_ANALYTICS 0
#endif

#ifndef WITH_FIREBASE_DATABASE
#	define WITH_FIREBASE_DATABASE 0
#endif

#ifndef WITH_FIREBASE_DYNAMIC_LINKS
#	define WITH_FIREBASE_DYNAMIC_LINKS 0
#endif

#ifndef WITH_FIREBASE_FUNCTIONS
#	define WITH_FIREBASE_FUNCTIONS 0
#endif

#ifndef WITH_FIREBASE_STORAGE
#	define WITH_FIREBASE_STORAGE 0
#endif

// The class path of the game activity.
extern const char* const FirebaseFeaturesGameActivityPath;

// Enum describing the strength of a dynamic links match.
UENUM(BlueprintType)
enum class ELinkMatchStrength : uint8
{
	// No match has been achieved
	NoMatch = 0,

	// The match between the Dynamic Link and device is not perfect.  You should
	// not reveal any personal information related to the Dynamic Link.
	WeakMatch,

	// The match between the Dynamic Link and this device has a high confidence,
	// but there is a small possibility of error.
	StrongMatch,

	// The match between the Dynamic Link and the device is exact.  You may
	// safely reveal any personal information related to this Dynamic Link.
	PerfectMatch
};

// Options that control the creation of a Firebase App.
USTRUCT(BlueprintType)
struct FIREBASEFEATURES_API FFirebaseAppOptions
{
	GENERATED_BODY()
public:
	// This is the mobilesdk_app_id in the Android google - services.json config
	// file or GOOGLE_APP_ID in the GoogleService-Info.plist.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|App|Config")
	FString AppID;

	// API key used to authenticate requests from your app.
	// For example, "AIzaSyDdVgKwhZl0sTTTLZ7iTmt1r3N2cJLnaDk" used to identify
	// your app to Google servers.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|App|Config", Meta = (DisplayName = "API Key"))
	FString APIKey;
	
	// The Firebase Cloud Messaging sender ID.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|App|Config")
	FString MessagingSenderID;
	
	// The database root URL, e.g. @"http://abc-xyz-123.firebaseio.com".
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|App|Config")
	FString DatabaseURL;
	
	// The tracking ID for Google Analytics, e.g. @"UA-12345678-1".
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|App|Config", Meta = (DisplayName = "GA Tracking ID"))
	FString GATrackingID;
	
	// The Google Cloud Storage bucket name,
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|App|Config")
	FString StorageBucket;

	// This is the project_id in the Android google-services.json config
    // file or PROJECT_ID in the GoogleService-Info.plist.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|App|Config")
	FString ProjectID;
};

DECLARE_MULTICAST_DELEGATE(FOnAuthEvent);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnDynamicLinkReceived, const FString&, const ELinkMatchStrength);
DECLARE_MULTICAST_DELEGATE(FOnFirebaseSDKInitialized);

namespace firebase { class App; };

class FIREBASEFEATURES_API FFirebaseFeaturesModule 	: public IAnalyticsProviderModule
{
private:
	friend class UFirebaseAnalyticsLibrary;

public:
	 FFirebaseFeaturesModule();
	~FFirebaseFeaturesModule();

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	static FORCEINLINE FOnAuthEvent& OnAuthStateChanged()
	{
		return OnAuthStateChangedEvent;
	}

	static FORCEINLINE FOnAuthEvent& OnIdTokenChanged()
	{
		return OnIdTokenChangedEvent;
	}

	static FORCEINLINE FOnDynamicLinkReceived& OnDynamicLinkReceived()
	{
		return OnDynamicLinkReceivedEvent;
	}

	static FORCEINLINE FOnFirebaseSDKInitialized& OnSDKInitialized()
	{
		return OnFirebaseSDKInitialized;
	}

	static bool IsFirebaseSDKInitialized();

	/**
	 * Creates a new Firebase App.
	 * @param AppName The name of this new instance.
	 * @param Options The options for this app.
	 * @return A new firebase app.
	*/
	static firebase::App* CreateFirebaseApp(const FString& AppName, const FFirebaseAppOptions& Options);

	/**
	 * Gets the internal Firebase Application.
	 * @return The internal Firebase App or nullptr if it doesn't exist.
	*/
	static firebase::App* GetFirebaseApp(const FString& AppName = TEXT(""));

	/**
	 * Makes the project use this app as the default app.
	 * @param NewAppName The name of the app to switch to.
	 * @return The app we switched to or nullptr if it doesn't exist.
	*/
	static firebase::App* SwitchFirebaseApp(FString NewAppName);

	/**
	 * Returns the current Firebase App. It is the default app or the app specified with
	 * SwitchFirebaseApp().
	 * @return The current Firebase App.
	*/
	static firebase::App* GetCurrentFirebaseApp();

private:
	bool HandleSettingsSaved();

	virtual TSharedPtr<IAnalyticsProvider> CreateAnalyticsProvider(
		const FAnalyticsProviderConfigurationDelegate& GetConfigValue) const override;

#if WITH_EDITOR
	bool IsCooking() const;
#endif // WITH_EDITOR

private:
	void InitializeFirebaseComponents();

	void InitAnalytics();
	void InitAdMob();
	void InitAuth();
	void InitDatabase();
	void InitDynamicLinks();
	void InitFunctions();
	void InitMessaging();
	void InitRemoteConfig();
	void InitStorage();

	static firebase::App* GetApp();
	static void CreateApp();

	static FString GetServicesLocation();

private:
#if WITH_FIREBASE_AUTH
	TUniquePtr<class FAuthStateListener>	AuthListener;
	TUniquePtr<class FIdTokenListener>		IdTokenListener;
#endif

	TUniquePtr<class FDynamicLinksListener> DynamicLinkListener;
	TUniquePtr<class FMessagingListener>	MessagingListener;

	TSharedPtr<class FFirebaseAnalyticsProvider> AnalyticsProvider;


	static FOnAuthEvent OnAuthStateChangedEvent;
	static FOnAuthEvent OnIdTokenChangedEvent;

	static FOnDynamicLinkReceived OnDynamicLinkReceivedEvent;

	static FOnFirebaseSDKInitialized OnFirebaseSDKInitialized;

	static bool bIsSDKInitialized;

	static FString CurrentFirebaseAppName;
};

