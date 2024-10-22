// Copyright Pandores Marketplace 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AdMob/FbBannerView.h"
#include "FirebaseFeatures.h"

#if WITH_FIREBASE_ADMOB

#if PLATFORM_ANDROID
#	include "Android/AndroidJNI.h"
#	include "Android/AndroidApplication.h"
#endif // PLATFORM_ANDROID

#if PLATFORM_IOS
#	include "IOS/IOSAppDelegate.h"
#endif // PLATFORM_IOS

THIRD_PARTY_INCLUDES_START
#	include "firebase/version.h"
#	if FIREBASE_VERSION_MAJOR < 11
#	    include "firebase/admob/types.h"
#   endif
#	if FIREBASE_VERSION_MAJOR >= 9
#		include "firebase/gma/types.h"
#	endif
THIRD_PARTY_INCLUDES_END

#include "FirebaseSdk/FirebaseConfig.h"

#include <string>

#define ADMOB_CONVERT_AD_REQUEST(Req) FAdMobHelper::FInitAdRequestMemoryHandler(Req).Get()

namespace firebase { namespace admob { struct AdRequest; } }

#if FIREBASE_VERSION_MAJOR >= 9
using FFirebaseAdRequest = firebase::gma::AdRequest;
#else
using FFirebaseAdRequest = firebase::admob::AdRequest;
#endif

namespace FAdMobHelper
{
	class FInitAdRequestMemoryHandler
	{
	public:
		FInitAdRequestMemoryHandler(const FAdMobAdRequest& InReq);
		FInitAdRequestMemoryHandler(const FInitAdRequestMemoryHandler&) = delete;
		FInitAdRequestMemoryHandler(FInitAdRequestMemoryHandler&&) = delete;

		~FInitAdRequestMemoryHandler();

		const FFirebaseAdRequest& Get();

	private:
		const FAdMobAdRequest& Req;
		FFirebaseAdRequest RawReq;
		TArray<std::string> Keywords;
	};

#if FIREBASE_VERSION_MAJOR >= 9 && WITH_FIREBASE_ADMOB
	firebase::gma::AdRequest CreateRequest(const FAdMobAdRequest& InReq);
#endif

	void* GetAdParent();
};

#endif

