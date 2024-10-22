// Copyright Pandores Marketplace 2024. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "FirebaseFeatures.h"

#include "Interfaces/IAnalyticsProvider.h"

DECLARE_LOG_CATEGORY_EXTERN(LogFirebaseAnalytics, Log, All);

class FIREBASEFEATURES_API FFirebaseAnalyticsProvider : public IAnalyticsProvider
{
public:
	FFirebaseAnalyticsProvider();
	virtual ~FFirebaseAnalyticsProvider();

	virtual bool	StartSession(const TArray<FAnalyticsEventAttribute>& Attributes = TArray<FAnalyticsEventAttribute>()) override;
	virtual void	EndSession() override;
	virtual void	RecordEvent(const FString& EventName, const TArray<FAnalyticsEventAttribute>& Attributes = TArray<FAnalyticsEventAttribute>()) override;
	virtual FString GetSessionID() const override;
	virtual bool	SetSessionID(const FString& InSessionID) override;
	virtual void	FlushEvents() override;
	virtual void	SetUserID(const FString& InUserID) override;
	virtual FString GetUserID() const override;
	virtual void SetDefaultEventAttributes(TArray<FAnalyticsEventAttribute>&& Attributes);
	virtual TArray<FAnalyticsEventAttribute> GetDefaultEventAttributesSafe() const;
	virtual int32 GetDefaultEventAttributeCount() const;
	virtual FAnalyticsEventAttribute GetDefaultEventAttribute(int AttributeIndex) const;

	FORCEINLINE bool HasAnalyticSessionStarted() const
	{
		return bAnalyticSessionStarted;
	}

private:
	bool bAnalyticSessionStarted;
};
