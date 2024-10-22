// Copyright Pandores Marketplace 2024. All Rights Reserved.

#include "DynamicLinks/FirebaseDynamicLinksNodes.h"
#include "FirebaseFeatures.h"

FGeneratedDynamicLink UFirebaseDynamicLinksBlueprintLibrary::GetLongLink(const FDynamicLinkComponents& Components)
{
	return FDynamicLinksLibrary::GetLongLink(Components);
}

bool UFirebaseDynamicLinksBlueprintLibrary::DidReceiveUnhandledDynamicLink()
{
	return FDynamicLinksLibrary::DidReceiveUnhandledDynamicLink();
}

void UFirebaseDynamicLinksBlueprintLibrary::ClearUnhandledDynamicLink()
{
	FDynamicLinksLibrary::ClearUnhandledDynamicLink();
}

FString UFirebaseDynamicLinksBlueprintLibrary::GetLastDynamicLinkURL()
{
	return FDynamicLinksLibrary::GetLastDynamicLinkURL();
}

ELinkMatchStrength UFirebaseDynamicLinksBlueprintLibrary::GetLastDynamicLinkMatchStrength()
{
	return FDynamicLinksLibrary::GetLastDynamicLinkMatchStrength();
}

UBlueprintFirebaseDynamicLinksProxy* UBlueprintFirebaseDynamicLinksProxy::ListenOnDynamicLinkReceived()
{
	return NewObject<ThisClass>();
}

void UBlueprintFirebaseDynamicLinksProxy::Activate()
{
	FFirebaseFeaturesModule::OnDynamicLinkReceived().AddUObject(this, &ThisClass::OnLink);

	// If we missed the DL, we fire it globally. Note that we broadcast it and not consume it locally
	// to make sure all others (C++) listeners will also get it.
	if (FDynamicLinksLibrary::DidReceiveUnhandledDynamicLink())
	{
		FFirebaseFeaturesModule::OnDynamicLinkReceived()
			.Broadcast(FDynamicLinksLibrary::GetLastDynamicLinkURL(), FDynamicLinksLibrary::GetLastDynamicLinkMatchStrength());
		FDynamicLinksLibrary::ClearUnhandledDynamicLink();
	}
}

void UBlueprintFirebaseDynamicLinksProxy::OnLink(const FString& URL, const ELinkMatchStrength MatchStrength)
{
	OnDynamicLinkReceived.Broadcast(URL, MatchStrength);
}

void UDynamicLinksGetShortLinkBase::OnTaskOver(bool bSuccess, const FGeneratedDynamicLink& Link)
{
	(bSuccess ? Success : Failed).Broadcast(Link);
	SetReadyToDestroy();
}

FDynamicLinksCallback UDynamicLinksGetShortLinkBase::GetCallback()
{
	return FDynamicLinksCallback::CreateUObject(this, &ThisClass::OnTaskOver);
}

void UDynamicLinksGetShortLinkWithComponents::Activate()
{
	FDynamicLinksLibrary::GetShortLink(Components, GetCallback());
}

UDynamicLinksGetShortLinkWithComponents* UDynamicLinksGetShortLinkWithComponents::GetShortLink(const FDynamicLinkComponents& Components)
{
	ThisClass* const Proxy = NewObject<ThisClass>();

	Proxy->Components = Components;

	return Proxy;
}

void UDynamicLinksGetShortLinkWithComponentsAndOptions::Activate()
{
	FDynamicLinksLibrary::GetShortLink(Components, Options, GetCallback());
}

UDynamicLinksGetShortLinkWithComponentsAndOptions* UDynamicLinksGetShortLinkWithComponentsAndOptions::GetShortLink(const FDynamicLinkComponents& Components, const FDynamicLinkOptions& Options)
{
	ThisClass* const Proxy = NewObject<ThisClass>();

	Proxy->Components = Components;
	Proxy->Options = Options;

	return Proxy;
}

void UDynamicLinksGetShortLinkWithString::Activate()
{
	FDynamicLinksLibrary::GetShortLink(LongDynamicLink, GetCallback());
}

UDynamicLinksGetShortLinkWithString* UDynamicLinksGetShortLinkWithString::GetShortLink(const FString& LongDynamicLink)
{
	ThisClass* const Proxy = NewObject<ThisClass>();

	Proxy->LongDynamicLink = LongDynamicLink;

	return Proxy;
}

void UDynamicLinksGetShortLinkWithStringAndOptions::Activate()
{
	FDynamicLinksLibrary::GetShortLink(LongDynamicLink, Options, GetCallback());
}

UDynamicLinksGetShortLinkWithStringAndOptions* UDynamicLinksGetShortLinkWithStringAndOptions::GetShortLink(const FString& LongDynamicLink, const FDynamicLinkOptions& Options)
{
	ThisClass* const Proxy = NewObject<ThisClass>();

	Proxy->LongDynamicLink = LongDynamicLink;
	Proxy->Options = Options;

	return Proxy;
}
