// Copyright Pandores Marketplace 2024. All Rights Reserved.

#include "AdMob/FbRewardedVideo.h"

THIRD_PARTY_INCLUDES_START
#	include "firebase/version.h"
#	if FIREBASE_VERSION_MAJOR < 11
#	    include "firebase/admob/rewarded_video.h"
#   endif
#	if FIREBASE_VERSION_MAJOR >= 9
#		include "firebase/gma/rewarded_ad.h"
#	endif
THIRD_PARTY_INCLUDES_END

#include "AdMob/AdMobNative.h"
#include "FirebaseFeatures.h"
#include "FirebaseSdk/FirebaseConfig.h"
#include "AdMob/AdMobHelper.h"
#include "Async/Async.h"
#include "FirebaseFeatures.h"

#if WITH_FIREBASE_ADMOB && FIREBASE_VERSION_MAJOR >= 9
static std::string GRewardedVideoAdUnit;
static TUniquePtr<firebase::gma::RewardedAd> GRewardedVideo;
static TUniquePtr<firebase::gma::UserEarnedRewardListener> GRewardedVideoListener;
#endif

class FPaidEventListener final
#if WITH_FIREBASE_ADMOB && FIREBASE_VERSION_MAJOR >= 11
	: public firebase::gma::PaidEventListener
#endif
{
public:

#if WITH_FIREBASE_ADMOB && FIREBASE_VERSION_MAJOR >= 11
	virtual void OnPaidEvent(const firebase::gma::AdValue& value) override
	{
		FAdMobRewardItem Item;
		Item.Amount		= value.value_micros();
		Item.RewardType = UTF8_TO_TCHAR(value.currency_code().c_str());

		UE_LOG(LogAdMob, Log, TEXT("Received Rewarded Video's reward. Type: %s. Amount: %f."), *Item.RewardType, Item.Amount);

		if (FRewardedVideo::OnRewardedEvent.IsBound())
		{
			AsyncTask(ENamedThreads::GameThread, [Item]() -> void 
			{
				FRewardedVideo::OnRewardedEvent.ExecuteIfBound(Item.RewardType, Item.Amount);
			});
		}	
	}
#endif
};

class FRewardedVideoListener final 
#if WITH_FIREBASE_ADMOB && FIREBASE_VERSION_MAJOR < 11
	: public FFirebaseRewardedVideo::Listener
#endif
{
public:
#if WITH_FIREBASE_ADMOB && FIREBASE_VERSION_MAJOR < 11
	virtual void OnRewarded(FFirebaseRewardedVideo::RewardItem reward) final
	{
		FAdMobRewardItem Item;
		Item.Amount		= reward.amount;
		Item.RewardType = UTF8_TO_TCHAR(reward.reward_type.c_str());

		UE_LOG(LogAdMob, Log, TEXT("Received Rewarded Video's reward. Type: %s. Amount: %f."), *Item.RewardType, Item.Amount);

		if (FRewardedVideo::OnRewardedEvent.IsBound())
		{
			AsyncTask(ENamedThreads::GameThread, [Item]() -> void 
			{
				FRewardedVideo::OnRewardedEvent.ExecuteIfBound(Item.RewardType, Item.Amount);
			});
		}
	}

	virtual void OnPresentationStateChanged(FFirebaseRewardedVideo::PresentationState state) final
	{
		UE_LOG(LogAdMob, Log, TEXT("Rewarded video state has changed."));

		if (FRewardedVideo::OnPresentationStateChangedEvent.IsBound())
		{
			AsyncTask(ENamedThreads::GameThread, [state]() -> void
			{
				FRewardedVideo::OnPresentationStateChangedEvent.ExecuteIfBound(static_cast<EAdMobPresentationState>(state));
			});
		}
	}
#endif // WITH_FIREBASE_ADMOB

	virtual ~FRewardedVideoListener() final
	{
	}
};

FAdMobVideoOnRewarded					FRewardedVideo::OnRewardedEvent;
FAdMobVideoOnPresentationStateChanged	FRewardedVideo::OnPresentationStateChangedEvent;
TUniquePtr<FRewardedVideoListener>		FRewardedVideo::Listener = MakeUnique<FRewardedVideoListener>();

void FRewardedVideo::LoadAd(const FString& AdUnitId, const FAdMobAdRequest& AdRequest, const FAdMobCallback& Callback)
{
#if WITH_FIREBASE_ADMOB
	// Interface for rewarded video has changed in SDK version 9.0.
	// We keep the same interface for backward compatibility.
#if FIREBASE_VERSION_MAJOR >= 9
	GRewardedVideo = MakeUnique<firebase::gma::RewardedAd>();
	GRewardedVideoAdUnit = TCHAR_TO_UTF8(*AdUnitId);

	GRewardedVideo->Initialize((firebase::gma::AdParent)FAdMobHelper::GetAdParent())
		.OnCompletion([AdUnitId, AdRequest, Callback](const firebase::Future<void>& _Future) -> void
	{
		const EFbAdMobError Error = (EFbAdMobError)_Future.error();
		if (Error != EFbAdMobError::None)
		{
			UE_LOG(LogAdMob, Error, TEXT("Failed to load ad for rewarded video: Code %d, Message: %s."), _Future.error(), UTF8_TO_TCHAR(_Future.error_message()));

			AsyncTask(ENamedThreads::GameThread, [Callback, Error]() -> void
			{
				Callback.ExecuteIfBound(Error);
			});
			return;
		}

		GRewardedVideo->LoadAd(GRewardedVideoAdUnit.c_str(), FAdMobHelper::CreateRequest(AdRequest))
			.OnCompletion([Callback](const firebase::Future<firebase::gma::AdResult>& _Future2) -> void
		{
			const EFbAdMobError Error2 = (EFbAdMobError)_Future2.error();
			if (Error2 != EFbAdMobError::None)
			{
				UE_LOG(LogAdMob, Error, TEXT("Failed to load ad for rewarded video: Code %d, Message: %s."), _Future2.error(), UTF8_TO_TCHAR(_Future2.error_message()));
			}

			AsyncTask(ENamedThreads::GameThread, [Callback, Error2]() -> void
			{
				Callback.ExecuteIfBound(Error2);
			});
		});
	});

#else
	// Firebase functions have to be copiable.
	auto Request = MakeShared<FAdMobHelper::FInitAdRequestMemoryHandler>(AdRequest);

	FFirebaseRewardedVideo::LoadAd(TCHAR_TO_UTF8(*AdUnitId), Request->Get()).
	
	OnCompletion([Callback, Request = MoveTemp(Request)](const firebase::Future<void>& _Future) -> void
	{
		const EFbAdMobError Error = (EFbAdMobError)_Future.error();
		if (Error != EFbAdMobError::None)
		{
			UE_LOG(LogAdMob, Error, TEXT("Failed to load ad for rewarded video: Code %d, Message: %s."), _Future.error(), UTF8_TO_TCHAR(_Future.error_message()));
		}

		AsyncTask(ENamedThreads::GameThread, [Callback, Error]() -> void
		{
			Callback.ExecuteIfBound(Error);
		});
	});
#endif
#else  // !WITH_FIREBASE_ADMOB
	Callback.ExecuteIfBound(EFbAdMobError::None);
#endif // !WITH_FIREBASE_ADMOB
}

void FRewardedVideo::Pause(const FAdMobCallback& Callback)
{
#if WITH_FIREBASE_ADMOB
#if FIREBASE_VERSION_MAJOR < 9
	FFirebaseRewardedVideo::Pause().
	
	OnCompletion([Callback](const firebase::Future<void>& _Future) -> void
	{
		const EFbAdMobError Error = (EFbAdMobError)_Future.error();
		if (Error != EFbAdMobError::None)
		{
			UE_LOG(LogAdMob, Error, TEXT("Failed to pause ad for rewarded video: Code %d, Message: %s."), Error, UTF8_TO_TCHAR(_Future.error_message()));
		}
		if (Callback.IsBound()) AsyncTask(ENamedThreads::GameThread, [Callback, Error]() -> void
		{
			Callback.ExecuteIfBound(Error);
		});
	});
#else
	Callback.ExecuteIfBound(EFbAdMobError::None);
#endif
#else  // !WITH_FIREBASE_ADMOB
	Callback.ExecuteIfBound(EFbAdMobError::None);
#endif // !WITH_FIREBASE_ADMOB
}

void FRewardedVideo::Resume(const FAdMobCallback& Callback)
{
#if WITH_FIREBASE_ADMOB
#if FIREBASE_VERSION_MAJOR < 9
	FFirebaseRewardedVideo::Resume().
	
	OnCompletion([Callback](const firebase::Future<void>& _Future) -> void
	{
		const EFbAdMobError Error = (EFbAdMobError)_Future.error();
		if (Error != EFbAdMobError::None)
		{
			UE_LOG(LogAdMob, Error, TEXT("Failed to resume ad for rewarded video: Code %d, Message: %s"), Error, UTF8_TO_TCHAR(_Future.error_message()));
		}
		if (Callback.IsBound()) AsyncTask(ENamedThreads::GameThread, [Callback, Error]() -> void
		{
			Callback.ExecuteIfBound(Error);
		});
	});
#endif
#else  // !WITH_FIREBASE_ADMOB
	Callback.ExecuteIfBound(EFbAdMobError::None);
#endif // !WITH_FIREBASE_ADMOB
}

void FRewardedVideo::SetListener()
{
#if WITH_FIREBASE_ADMOB
#if FIREBASE_VERSION_MAJOR < 9
	FFirebaseRewardedVideo::SetListener(Listener.Get());
#endif
#endif // WITH_FIREBASE_ADMOB
}

void FRewardedVideo::Show(const FAdMobCallback& Callback)
{
#if WITH_FIREBASE_ADMOB
#if FIREBASE_VERSION_MAJOR >= 9
	class FRewardListenerAdapter final : public firebase::gma::UserEarnedRewardListener
	{
	public:
		virtual void OnUserEarnedReward(const firebase::gma::AdReward& reward) override
		{
			AsyncTask(ENamedThreads::GameThread, [Type = FString(UTF8_TO_TCHAR(reward.type().c_str())),
				Amount = static_cast<float>(reward.amount())]() -> void
			{
				FRewardedVideo::OnRewarded().ExecuteIfBound(Type, Amount);
			});
		}
	};

	if (GRewardedVideo)
	{
		GRewardedVideoListener = MakeUnique<FRewardListenerAdapter>();
		GRewardedVideo->Show(GRewardedVideoListener.Get())
			.OnCompletion([Callback](const firebase::Future<void>& _Future) -> void
		{
			const EFbAdMobError Error = (EFbAdMobError)_Future.error();
			if (Error != EFbAdMobError::None)
			{
				UE_LOG(LogAdMob, Error, TEXT("Failed to show rewarded video: Code %d, Message: %s"), Error, UTF8_TO_TCHAR(_Future.error_message()));
			}

			AsyncTask(ENamedThreads::GameThread, [Callback, Error]() -> void
			{
				Callback.ExecuteIfBound(Error);
			});				
		});
	}
	else
	{
		Callback.ExecuteIfBound(EFbAdMobError::Uninitialized);
	}
#else
	FFirebaseRewardedVideo::Show((firebase::admob::AdParent)FAdMobHelper::GetAdParent()).
	
	OnCompletion([Callback](const firebase::Future<void>& _Future) -> void
	{
		const EFbAdMobError Error = (EFbAdMobError)_Future.error();
		if (Error != EFbAdMobError::None)
		{
			UE_LOG(LogAdMob, Error, TEXT("Failed to show rewarded video: Code %d, Message: %s"), Error, UTF8_TO_TCHAR(_Future.error_message()));
		}

		if (Callback.IsBound()) AsyncTask(ENamedThreads::GameThread, [Callback, Error]() -> void
		{
			Callback.ExecuteIfBound(Error);
		});
	});
#endif
#else  // !WITH_FIREBASE_ADMOB
	Callback.ExecuteIfBound(EFbAdMobError::None);
#endif // !WITH_FIREBASE_ADMOB
}
