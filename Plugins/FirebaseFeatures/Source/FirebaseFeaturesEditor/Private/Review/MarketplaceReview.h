// Copyright Pandores Marketplace 2023. All Rights Reserved.

#if WITH_EDITOR

#include "CoreMinimal.h"
#include "Editor.h"
#include "Editor/EditorEngine.h"
#include "Misc/ConfigCacheIni.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/SBoxPanel.h"
#include "Interfaces/IPluginManager.h"
#include "Framework/Application/SlateApplication.h"
#include "Misc/CommandLine.h"

#define PLUGIN_NAME "Firebase Features"
#define PLUGIN_URL	"https://www.unrealengine.com/marketplace/en-US/product/81111c9d88144ab0968fd8953872a3d8/reviews"

static const auto SettingsSection = TEXT("PMR") TEXT(PLUGIN_NAME);
static const auto ReviewRequestLaunchCount = 50;

/*
		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.AddRange(new string[]
			{
				"UnrealEd",
				"SlateCore",
				"Slate",
				"DesktopPlatform",
			});
		}
*/

static inline void ShowFeedbackWindow()
{
	TSharedPtr<SWindow> Window = SNew(SWindow)
		.Title(INVTEXT("What do you think of " PLUGIN_NAME))
		.SizingRule(ESizingRule::UserSized)
		.ClientSize({ 500.f, 300.f });

	TSharedPtr<SMultiLineEditableTextBox> ReviewText;

	Window->SetContent(
		SNew(SVerticalBox)
		
		+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(FMargin(40, 20, 40, 5))
		[
			SNew(STextBlock)
				.Text(NSLOCTEXT("PMR", "Review", "What could be improved for the plugin?"))
		]
		+ SVerticalBox::Slot()
			.Padding(FMargin(40, 5, 40, 5))
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.FillHeight(1.f)
		[
			SAssignNew(ReviewText, SMultiLineEditableTextBox)
				.HintText(NSLOCTEXT("PMR", "Feedback Hint", "Your thoughts..."))
		]
		+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(FMargin(40, 5, 40, 40))
			.HAlign(HAlign_Right)
		[
			SNew(SButton)
				.OnClicked(FOnClicked::CreateLambda([ReviewText, Window]() -> FReply
				{
					FPlatformProcess::LaunchURL(
						*FString::Printf(
							TEXT("mailto:pandores.marketplace@gmail.com?subject=%s&body=%s"),
							TEXT(PLUGIN_NAME),
							*ReviewText->GetText().ToString()),
						nullptr, nullptr);
					Window->RequestDestroyWindow();
					return FReply::Handled();
				}))
			[
				SNew(STextBlock)
					.Text(NSLOCTEXT("PMR", "Submit", "Submit"))
			]
		]
	);
	GEditor->EditorAddModalWindow(Window.ToSharedRef());
}

static inline void ShowRateWindow()
{
	TSharedPtr<SWindow> Window = SNew(SWindow)
		.Title(NSLOCTEXT("PMR", "Title", "Wee need your feedback"))
		.SizingRule(ESizingRule::UserSized)
		.ClientSize({ 700.f, 200.f });

	const FString DefaultFontPath = FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf");
	const FSlateFontInfo TitleFont = FSlateFontInfo{ DefaultFontPath , 20 };

	TSharedPtr<SMultiLineEditableTextBox> ReviewText;

	Window->SetContent(
		SNew(SVerticalBox)
		
		+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(FMargin(40, 20, 40, 5))
		[
			SNew(STextBlock)
				.Font(TitleFont)
				.Text(NSLOCTEXT("PMR", "Review", "What do you think of the " PLUGIN_NAME " plugin?"))
		]
		+ SVerticalBox::Slot()
			.FillHeight(1)
			.Padding(FMargin(40, 5, 40, 0))
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
		[
			SNew(SHorizontalBox)

			+SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.AutoWidth()
				.Padding(5.f)
			[
				SNew(SButton)
					.ContentPadding(FMargin{ 10.f, 5.f })
					.HAlign(HAlign_Center)
					.OnClicked(FOnClicked::CreateLambda([ReviewText, Window]() -> FReply
					{
						Window->RequestDestroyWindow();
						ShowFeedbackWindow();
						return FReply::Handled();
					}))
				[
					SNew(STextBlock)
						.Text(NSLOCTEXT("PMR", "Submit", "Very Bad"))
				]
			]
			+SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.AutoWidth()
				.Padding(5.f)
			[
				SNew(SButton)
					.ContentPadding(FMargin{10.f, 5.f})
					.HAlign(HAlign_Center)
					.OnClicked(FOnClicked::CreateLambda([ReviewText, Window]() -> FReply
					{
						Window->RequestDestroyWindow();
						ShowFeedbackWindow();
						return FReply::Handled();
					}))
				[
					SNew(STextBlock)
						.Text(NSLOCTEXT("PMR", "Submit", "Bad"))
				]
			]
			+SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.AutoWidth()
				.Padding(5.f)
			[
				SNew(SButton)
					.ContentPadding(FMargin{ 10.f, 5.f })
					.HAlign(HAlign_Center)
					.OnClicked(FOnClicked::CreateLambda([ReviewText, Window]() -> FReply
					{
						Window->RequestDestroyWindow();
						ShowFeedbackWindow();
						return FReply::Handled();
					}))
				[
					SNew(STextBlock)
						.Text(NSLOCTEXT("PMR", "Submit", "Ok"))
				]
			]
			+SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.AutoWidth()
				.Padding(5.f)
			[
				SNew(SButton)
					.ContentPadding(FMargin{ 10.f, 5.f })
					.HAlign(HAlign_Center)
					.OnClicked(FOnClicked::CreateLambda([ReviewText, Window]() -> FReply
					{
						Window->RequestDestroyWindow();
						FPlatformProcess::LaunchURL(TEXT(PLUGIN_URL), nullptr, nullptr);
						return FReply::Handled();
					}))
				[
					SNew(STextBlock)
						.Text(NSLOCTEXT("PMR", "Submit", "Good"))
				]
			]
			+SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.AutoWidth()
				.Padding(5.f)
			[
				SNew(SButton)
					.ContentPadding(FMargin{ 10.f, 5.f })
					.HAlign(HAlign_Center)
					.OnClicked(FOnClicked::CreateLambda([ReviewText, Window]() -> FReply
					{
						Window->RequestDestroyWindow();
						FPlatformProcess::LaunchURL(TEXT(PLUGIN_URL), nullptr, nullptr);
						return FReply::Handled();
					}))
				[
					SNew(STextBlock)
						.Text(NSLOCTEXT("PMR", "Submit", "Very Good"))
				]
			]
		]
		
		+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(FMargin(40, 5, 40, 40))
			.HAlign(HAlign_Right)
		[
			SNew(SButton)
				.ContentPadding(FMargin{ 10.f, 5.f })
				.OnClicked(FOnClicked::CreateLambda([ReviewText, Window]() -> FReply
				{
					Window->RequestDestroyWindow();
					GConfig->SetBool(SettingsSection, TEXT("HideReview"), true, GEditorIni);
					return FReply::Handled();
				}))
			[
				SNew(STextBlock)
					.Text(NSLOCTEXT("PMR", "Never Show", "Never Show Again"))
			]
		]
	);

	FSlateApplication::Get().AddModalWindow(Window.ToSharedRef(), nullptr);
	// GEditor->EditorAddModalWindow(Window.ToSharedRef());
}

#endif

inline void RequestReview()
{
#if WITH_EDITOR
	if (
		IsRunningCommandlet() || 
		GIsRunningUnattendedScript ||
		FString(FCommandLine::GetOriginal()).Contains(TEXT("nullrhi")) ||
		!FSlateApplication::IsInitialized() ||
		!FSlateApplication::Get().CanDisplayWindows()
		)
	{
		return;
	}

	bool bHideReview = false;
	bool bDidReview = false;
	GConfig->GetBool(SettingsSection, TEXT("HideReview"), bHideReview, GEditorIni);
	GConfig->GetBool(SettingsSection, TEXT("DidReview"), bDidReview, GEditorIni);

	if (!bHideReview && !bDidReview)
	{
		int32 LaunchCount = 0;
		GConfig->GetInt(SettingsSection, TEXT("StartupCount"), LaunchCount, GEditorIni);

		if (LaunchCount % ReviewRequestLaunchCount == ReviewRequestLaunchCount - 1)
		{
			ShowRateWindow();
		}

		GConfig->SetInt(SettingsSection, TEXT("StartupCount"), LaunchCount + 1, GEditorIni);
	}
#endif
}



