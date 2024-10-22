// Copyright Pandores Marketplace 2024. All Rights Reserved.
#include "FirebaseSdk/FirebaseConfig.h"

UFirebaseConfig::UFirebaseConfig()
{
	LoadConfig();
}

const UFirebaseConfig* UFirebaseConfig::Get()
{
	return GetDefault<UFirebaseConfig>();
}
