// Copyright Pandores Marketplace 2024. All Rights Reserved.

#include "AuthHelper.h"

#if WITH_FIREBASE_AUTH

#include "Auth/Auth.h"
#include "Auth/User.h"


FUserProfileDataManager::FUserProfileDataManager(const FUserProfile* const Profile)
	: bAllocatedDisplayName(false)
	, bAllocatedPhotoUrl   (false)
{
	if (Profile->bResetDisplayName)
	{
		DisplayName = (char*)FMemory::Malloc(1);
		DisplayName[0] = '\0';
		bAllocatedDisplayName = true;
	}
	else if (Profile->DisplayName.IsEmpty())
	{
		DisplayName = nullptr;
	}
	else
	{
		const int32 AllocSize = Profile->DisplayName.Len() * sizeof(ANSICHAR);
		DisplayName = (char*)FMemory::Malloc(AllocSize + 1);
		bAllocatedDisplayName = true;

		FMemory::Memcpy(DisplayName, TCHAR_TO_UTF8(*Profile->DisplayName), AllocSize);

		DisplayName[AllocSize] = '\0';
	}

	if (Profile->bResetPhotoUrl)
	{
		PhotoUrl = (char*)FMemory::Malloc(1);
		PhotoUrl[0] = '\0';
		bAllocatedPhotoUrl = true;
	}
	else if (Profile->PhotoUrl.IsEmpty())
	{
		PhotoUrl = nullptr;
	}
	else
	{
		const int32 AllocSize = Profile->PhotoUrl.Len() * sizeof(ANSICHAR);
		PhotoUrl = (char*)FMemory::Malloc(AllocSize + 1);
		bAllocatedPhotoUrl = true;

		FMemory::Memcpy(PhotoUrl, TCHAR_TO_UTF8(*Profile->PhotoUrl), AllocSize);

		PhotoUrl[AllocSize] = '\0';
	}
}

FUserProfileDataManager::~FUserProfileDataManager()
{
	if (bAllocatedDisplayName)
	{
		FMemory::Free(DisplayName);
	}
	if (bAllocatedPhotoUrl)
	{
		FMemory::Free(PhotoUrl);
	}
}

firebase::auth::User::UserProfile FUserProfileDataManager::Get()
{
	firebase::auth::User::UserProfile Profile;

	Profile.display_name = DisplayName;
	Profile.photo_url	 = PhotoUrl;

	return Profile;
}

#if FIREBASE_VERSION_MAJOR >= 11
FSignInResult FAuthHelper::ConvertSignInResult(const firebase::auth::AuthResult* const SignInResult)
{
	FSignInResult Result;
	if (SignInResult)
	{
		if (SignInResult->user.is_valid())
		{
			Result.User = NewObject<UUser>();
			Result.User->SetUser(&SignInResult->user);
		}


		for (const auto& Profile : SignInResult->additional_user_info.profile)
		{
			Result.Info.Profile.Add(FFirebaseVariant(Profile.first), FFirebaseVariant(Profile.second));
		}

		if (!SignInResult->additional_user_info.provider_id.empty())
			Result.Info.ProviderId = UTF8_TO_TCHAR(SignInResult->additional_user_info.provider_id.c_str());
		if (!SignInResult->additional_user_info.user_name.empty())
			Result.Info.UserName = UTF8_TO_TCHAR(SignInResult->additional_user_info.user_name.c_str());
		Result.Info.UpdatedCredential = FCredential(new firebase::auth::Credential(SignInResult->additional_user_info.updated_credential));
	}

	return Result;
}
#endif

FSignInResult FAuthHelper::ConvertSignInResult(const firebase::auth::SignInResult* const SignInResult)
{
	FSignInResult Result;

	if (SignInResult)
	{
		if (SignInResult->user)
		{
			Result.User = NewObject<UUser>();
			Result.User->SetUser(SignInResult->user);
		}

		Result.Meta.CreationTtimestamp  = SignInResult->meta.creation_timestamp;
		Result.Meta.LastSignInTimestamp = SignInResult->meta.last_sign_in_timestamp;

		for (const auto& Profile : SignInResult->info.profile)
		{
			Result.Info.Profile.Add(FFirebaseVariant(Profile.first), FFirebaseVariant(Profile.second));
		}

		if (!SignInResult->info.provider_id.empty())
			Result.Info.ProviderId			= UTF8_TO_TCHAR(SignInResult->info.provider_id.c_str());
		if (!SignInResult->info.user_name.empty())
			Result.Info.UserName			= UTF8_TO_TCHAR(SignInResult->info.user_name.c_str());
		Result.Info.UpdatedCredential	= FCredential(new firebase::auth::Credential(SignInResult->info.updated_credential));
	}

	return Result;
}

#if FIREBASE_VERSION_MAJOR >= 11
UUser* FAuthHelper::ConvertUser(const firebase::auth::AuthResult* User)
{
	UUser* _User = nullptr;

	if (User && User->user.is_valid())
	{
		_User = NewObject<UUser>();
		_User->SetUser(&User->user);
	}

	return _User;
}
#endif

UUser* FAuthHelper::ConvertUser(firebase::auth::User* const * const User)
{
	return ConvertUser(User ? *User : nullptr);
}

UUser* FAuthHelper::ConvertUser(firebase::auth::User* const User)
{
	UUser* _User = nullptr;

	if (User)
	{
		_User = NewObject<UUser>();
		_User->SetUser(User);
	}

	return _User;
}

#endif

