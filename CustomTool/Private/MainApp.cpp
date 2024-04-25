// Copyright Epic Games, Inc. All Rights Reserved.

#include "MainApp.h"
#include "RequiredProgramMainCPPInclude.h"
#include "Widgets/Testing/STestSuite.h"
#include "ISourceCodeAccessModule.h"
#include "Widgets/Testing/SPerfSuite.h"
#include "Widgets/Docking/SDockTab.h"
#include "Framework/Application/SlateApplication.h"
#include "IWebBrowserWindow.h"
#include "IWebBrowserPopupFeatures.h"

#include "SImageConverter.h"

IMPLEMENT_APPLICATION(HelloSlate, "HelloSlate");

#define LOCTEXT_NAMESPACE "HelloSlate"

namespace WorkspaceMenu
{
	TSharedRef<FWorkspaceItem> DeveloperMenu = FWorkspaceItem::NewGroup(LOCTEXT("DeveloperMenu", "Developer"));
}

int RunSimpleGUI(const TCHAR* CommandLine)
{
	// start up the main loop
	GEngineLoop.PreInit(CommandLine);

	FSlateApplication::InitializeAsStandaloneApplication(GetStandardStandaloneRenderer());

	// create a test window
	FGlobalTabmanager::Get()->SetApplicationTitle(LOCTEXT("AppTitle", "Hello Slate"));
	TSharedPtr<SWindow> SlateWin = SNew(SWindow)
		.Title(LOCTEXT("CustomTool", "Custom Tool"))
		.ClientSize(FVector2D(500, 300))
		.AutoCenter(EAutoCenter::None);

	TSharedPtr<SVerticalBox> Contents;
	SlateWin->SetContent(
		SNew(SBorder)
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Fill)
		.Padding(0)
		[
			SAssignNew(Contents, SVerticalBox)
		]);

	Contents->AddSlot()
		[
			SNew(SImageConverter)
		];

	FSlateApplication::Get().AddWindow(SlateWin.ToSharedRef());

	while (!IsEngineExitRequested())
	{
		FSlateApplication::Get().Tick();
		FSlateApplication::Get().PumpMessages();
		FPlatformProcess::Sleep(0.01);
	}
	FCoreDelegates::OnExit.Broadcast();
	FSlateApplication::Shutdown();
	FModuleManager::Get().UnloadModulesAtShutdown();
	
	return 0;
}

int RunHelloSlate(const TCHAR* CommandLine)
{
	// Set your engine dir
	GForeignEngineDir = TEXT("/Users/david/UnrealEngine/Engine/");
	printf("Engine: > %s : %s : %s\n", UE_ENGINE_DIRECTORY, TCHAR_TO_ANSI(GForeignEngineDir),
	       TCHAR_TO_ANSI(*FPaths::EngineDir()));

	
	return RunSimpleGUI(CommandLine);
}

#undef LOCTEXT_NAMESPACE
