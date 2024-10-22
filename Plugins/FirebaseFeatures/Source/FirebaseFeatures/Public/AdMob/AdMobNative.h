// Copyright Pandores Marketplace 2024. All Rights Reserved.

#pragma once

THIRD_PARTY_INCLUDES_START
#	include "firebase/version.h"
THIRD_PARTY_INCLUDES_END

namespace firebase
{
	namespace gma
	{
		class AdView;
		class InterstitialAd;
		namespace rewarded_video {}
		using BannerView = AdView;
		class AdRequest;
		class RewardedAd;
	}

	namespace admob
	{
		class BannerView;
		class InterstitialAd;
		namespace rewarded_video {}
		struct AdRequest;
	}
};

#if FIREBASE_VERSION_MAJOR >= 9
using FFirebaseBannerView 		 = firebase::gma::BannerView;
using FFirebaseInterstitial      = firebase::gma::InterstitialAd;;
#if FIREBASE_VERSION_MAJOR >= 11
using FFirebaseRewardedVideo = firebase::gma::RewardedAd;
#else
// Still use old AdMob, API needs to change.
namespace FFirebaseRewardedVideo = firebase::admob::rewarded_video; 
#endif
#else
using FFirebaseBannerView 		 = firebase::admob::BannerView;
using FFirebaseInterstitial      = firebase::admob::InterstitialAd;;
namespace FFirebaseRewardedVideo = firebase::admob::rewarded_video;
#endif