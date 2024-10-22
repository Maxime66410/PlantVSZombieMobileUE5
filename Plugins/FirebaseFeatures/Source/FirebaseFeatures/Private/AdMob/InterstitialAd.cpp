// Copyright Pandores Marketplace 2024. All Rights Reserved.

#include "AdMob/FbInterstitialAd.h"

THIRD_PARTY_INCLUDES_START
#	include "firebase/version.h"
#	if FIREBASE_VERSION_MAJOR < 11
#    	include "firebase/admob/interstitial_ad.h"
#   endif
#	if FIREBASE_VERSION_MAJOR >= 9
#		include "firebase/gma/interstitial_ad.h"
#	endif
THIRD_PARTY_INCLUDES_END

#include "AdMob/AdMobHelper.h"
#include "AdMob/FbAdMobTypes.h"

#include "Async/Async.h"

#include "FirebaseFeatures.h"

DECLARE_LOG_CATEGORY_CLASS(LogAdMobInterstitialAd, Log, All);

class FInterstitialAdListener final 
#if WITH_FIREBASE_ADMOB
#if FIREBASE_VERSION_MAJOR >= 9
	: public firebase::gma::FullScreenContentListener
	, public firebase::gma::PaidEventListener
#else
	: public firebase::admob::InterstitialAd::Listener
#endif
#endif // WITH_FIREBASE_ADMOB
{
public:
#if WITH_FIREBASE_ADMOB
	FInterstitialAdListener(UInterstitialAd* const InAd)
		: Ad(InAd)
	{
	}

#if FIREBASE_VERSION_MAJOR < 11
	virtual void OnPresentationStateChanged
	(
		firebase::admob::InterstitialAd* interstitial_ad,
		firebase::admob::InterstitialAd::PresentationState state
	)
	{
		UE_LOG(LogAdMobInterstitialAd, Log, TEXT("Interstitial Ad presentation state has changed."));

		if (Ad.IsValid())
		{
			Ad->OnPresentationStateChangedInternal
			(
				static_cast<EAdMobInterstitialAdPresentationState>(state)
			);
		}
	}
#endif

	virtual ~FInterstitialAdListener()
	{
	}

	virtual void OnAdClicked() 
	{
	}

	virtual void OnAdDismissedFullScreenContent() 
	{
	}

#if FIREBASE_VERSION_MAJOR >= 9
	virtual void OnAdFailedToShowFullScreenContent(const firebase::gma::AdError& ad_error) 
	{
	
	}
#endif

	virtual void OnAdImpression() 
	{
	
	}

	virtual void OnAdShowedFullScreenContent() 
	{
	
	}

private:
	TSoftObjectPtr<UInterstitialAd> Ad;
#endif // WITH_FIREBASE_ADMOB
};

UInterstitialAd::UInterstitialAd(FVTableHelper& Helper) 
	: Super(Helper)
#if WITH_FIREBASE_ADMOB
	, Listener(nullptr)
	, InterstitialAd(nullptr)
#endif
{

}

UInterstitialAd::UInterstitialAd()
#if WITH_FIREBASE_ADMOB
	: Listener(nullptr)
	, InterstitialAd(nullptr)
#endif
{
}

UInterstitialAd::~UInterstitialAd()
{
#if WITH_FIREBASE_ADMOB
	if (InterstitialAd)
	{
		delete InterstitialAd;
	}

	if (Listener)
	{
		delete Listener;
	}
#endif // WITH_FIREBASE_ADMOB
}

void UInterstitialAd::OnPresentationStateChangedInternal(const EAdMobInterstitialAdPresentationState PresentationState)
{
	OnPresentationChangedEvent.Broadcast(PresentationState);
}

void UInterstitialAd::Initialize(const FString& AdUnit, const FAdMobCallback& Callback)
{
#if WITH_FIREBASE_ADMOB
	if (!InterstitialAd)
	{
		InterstitialAd = new FFirebaseInterstitial();
	}

	if (!Listener)
	{
		Listener = new FInterstitialAdListener(this);
	}

	if (InterstitialAd->InitializeLastResult().status() == firebase::FutureStatus::kFutureStatusComplete)
	{
		UE_LOG(LogAdMobInterstitialAd, Warning, TEXT("Initialize called on an initialized interstitial ad."));
		Callback.ExecuteIfBound(EFbAdMobError::AlreadyInitialized);
		return;
	}

	if (InterstitialAd->InitializeLastResult().status() == firebase::FutureStatus::kFutureStatusPending)
	{
		UE_LOG(LogAdMobInterstitialAd, Warning, TEXT("Initialize called on an interstitial ad that is being initialized."));
		Callback.ExecuteIfBound(EFbAdMobError::AlreadyInitialized);
		return;
	}

	TWeakObjectPtr<UInterstitialAd> Self;

#if FIREBASE_VERSION_MAJOR >= 9
	AdUnitStr = TCHAR_TO_UTF8(*AdUnit);
	InterstitialAd->Initialize((firebase::gma::AdParent)FAdMobHelper::GetAdParent())
#else
	std::string AdUnitStr = TCHAR_TO_UTF8(*AdUnit);
	InterstitialAd->Initialize((firebase::admob::AdParent)FAdMobHelper::GetAdParent(), AdUnitStr.c_str())
#endif

	.OnCompletion([AdUnitStr = MoveTemp(AdUnitStr), Callback, Self = MoveTemp(Self)](const firebase::Future<void>& _Future) mutable -> void
	{
		const EFbAdMobError Error = (EFbAdMobError)_Future.error();
		if (Error != EFbAdMobError::None)
		{
			UE_LOG(LogAdMobInterstitialAd, Error, TEXT("Failed to initialize Interstitial Ad: Code %d. Message: %s"), _Future.error(), UTF8_TO_TCHAR(_Future.error_message()));
		}
		else
		{
			UE_LOG(LogAdMobInterstitialAd, Log, TEXT("New Interstitial Ad initialized."));
		}

		AsyncTask(ENamedThreads::GameThread, [Error, Callback, Self = MoveTemp(Self)]() -> void
		{
			if (Self.IsValid() && Self->InterstitialAd)
			{
#if FIREBASE_VERSION_MAJOR >= 9
				Self->InterstitialAd->SetFullScreenContentListener(Self->Listener);
				Self->InterstitialAd->SetPaidEventListener(Self->Listener);
#else
				Self->InterstitialAd->SetListener(Self->Listener);
#endif
			}

			Callback.ExecuteIfBound(Error);
		});
	});
#else
	UE_LOG(LogAdMobInterstitialAd, Warning, TEXT("Interstial Ad created but AdMob is disabled."));
	Callback.ExecuteIfBound((EFbAdMobError)0);
#endif
}

void UInterstitialAd::LoadAd(const FAdMobAdRequest& AdRequest, const FAdMobCallback& Callback)
{
#if WITH_FIREBASE_ADMOB
	if (InterstitialAd)
	{
#if FIREBASE_VERSION_MAJOR >= 9
		InterstitialAd->LoadAd(AdUnitStr.c_str(), FAdMobHelper::CreateRequest(AdRequest)).
			OnCompletion([Callback](const firebase::Future<firebase::gma::AdResult> _Future)
#else
		RequestData = MakeUnique<FAdMobHelper::FInitAdRequestMemoryHandler>(AdRequest);
		InterstitialAd->LoadAd(RequestData->Get()).
			OnCompletion([Callback](const firebase::Future<void> _Future)
#endif
		{
			const EFbAdMobError Error = (EFbAdMobError)_Future.error();
			if (Error != EFbAdMobError::None)
			{
				UE_LOG(LogAdMobInterstitialAd, Error, TEXT("Failed to load ad for Interstitial Ad: Code %d. Message: %s"),
					_Future.error(), UTF8_TO_TCHAR(_Future.error_message()));
			}
			AsyncTask(ENamedThreads::GameThread, [Error, Callback]() -> void
			{
				UE_LOG(LogAdMobInterstitialAd, Error, TEXT("Interstital ad loaded."));
				Callback.ExecuteIfBound(Error);
			});
		});
	}
#else
	Callback.ExecuteIfBound((EFbAdMobError)0);
#endif
}

void UInterstitialAd::Show(const FAdMobCallback& Callback)
{
#if WITH_FIREBASE_ADMOB 
	if (InterstitialAd)
	{
		InterstitialAd->Show().
			
		OnCompletion([Callback](const firebase::Future<void> _Future)
		{
			const EFbAdMobError Error = (EFbAdMobError)_Future.error();
			if (Error != EFbAdMobError::None)
			{
				UE_LOG(LogAdMobInterstitialAd, Error, TEXT("Failed to Show Interstitial Ad: Code %d. Message: %s"), _Future.error(), UTF8_TO_TCHAR(_Future.error_message()));
			}
			AsyncTask(ENamedThreads::GameThread, [Error, Callback]() -> void
			{
				Callback.ExecuteIfBound(Error);
			});
		});
	}
#else
	Callback.ExecuteIfBound((EFbAdMobError)0);
#endif
}

EAdMobInterstitialAdPresentationState UInterstitialAd::GetPresentationState() const
{
#if WITH_FIREBASE_ADMOB && FIREBASE_VERSION_MAJOR < 9
	if (InterstitialAd)
	{
		return static_cast<EAdMobInterstitialAdPresentationState>(InterstitialAd->presentation_state());
	}
#endif // WITH_FIREBASE_ADMOB
	return EAdMobInterstitialAdPresentationState::PresentationStateHidden;
}


