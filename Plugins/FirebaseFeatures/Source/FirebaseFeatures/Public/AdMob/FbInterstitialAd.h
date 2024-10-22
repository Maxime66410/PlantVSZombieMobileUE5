// Copyright Pandores Marketplace 2024. All Rights Reserved.

#pragma once

#include "FbAdMobTypes.h"
#include "CoreMinimal.h"
#include "AdMobNative.h"
#include <string>
#include "FbInterstitialAd.generated.h"

UENUM(BlueprintType)
enum class EAdMobInterstitialAdPresentationState : uint8
{
	/// InterstitialAd is not currently being shown.
	PresentationStateHidden = 0,
	/// InterstitialAd is being shown or has caused focus to leave the
	/// application (for example, when opening an external browser during a
	/// clickthrough).
	PresentationStateCoveringUI,
};

DECLARE_MULTICAST_DELEGATE_OneParam(FInterstitialAdOnPresentationStateChanged, const EAdMobInterstitialAdPresentationState);

UCLASS(BlueprintType)
class FIREBASEFEATURES_API UInterstitialAd final : public UObject
{
	GENERATED_BODY()
private:
	friend class FInterstitialAdListener;

public:
	UInterstitialAd();
	UInterstitialAd(FVTableHelper& Helper);
	~UInterstitialAd();

	void Initialize	(const FString&          AdUnit,    const FAdMobCallback& Callback = FAdMobCallback());
	void LoadAd		(const FAdMobAdRequest&	 AdRequest, const FAdMobCallback& Callback = FAdMobCallback());
	void Show		(const FAdMobCallback& Callback = FAdMobCallback());

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AdMob|InterstitialAd", meta = (DeprecatedFunction))
	UPARAM(DisplayName = "Presentation State") EAdMobInterstitialAdPresentationState GetPresentationState() const;

	FORCEINLINE FInterstitialAdOnPresentationStateChanged& OnPresentationStateChanged()
	{
		return OnPresentationChangedEvent;
	}

private:
	void OnPresentationStateChangedInternal(const EAdMobInterstitialAdPresentationState PresentationState);

private:
#if FIREBASE_VERSION_MAJOR >= 9
	std::string AdUnitStr;
#endif

	FInterstitialAdOnPresentationStateChanged OnPresentationChangedEvent;

#if WITH_FIREBASE_ADMOB
	class FInterstitialAdListener* Listener;

	FFirebaseInterstitial* InterstitialAd;

	FAdMobAdRequestDataPtr RequestData;
#endif
};